#include "codegen/expr_codegen.hpp"
#include "codegen/codegen_context.hpp"

#include <cassert>
#include <stdexcept>
#include <string>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

#include "ast/expr_nodes.hpp"
#include "ast/operators.hpp"

namespace paracl::codegen
{

    namespace
    {
        llvm::Value* to_bool (CodegenContext& cg, llvm::Value* value)
        {
            return cg.builder().CreateICmpNE(
                value,
                llvm::ConstantInt::get(cg.get_i32_type(), 0),
                "tobool"
            );
        }

    } // namespace

    ExprCodegen::ExprCodegen (CodegenContext& cg)
        : cg_ (cg)
    {
    }

    llvm::Value* ExprCodegen::emit (const Expr* expr)
    {
        assert (expr && "ExprCodegen::emit: expr must not be null");
        result_ = nullptr;

        expr->accept (*this);
        assert (result_ && "ExprCodegen: visit() did not set result_");

        return result_;
    }

    // -----------------------------------------------------------------------
    // helper: get-or-create the alloca slot for a named variable.
    // -----------------------------------------------------------------------
    llvm::AllocaInst* ExprCodegen::get_or_create_slot (const std::string& name)
    {
        auto& named = cg_.named_values();
        auto  it = named.find (name);

        if (it != named.end())
            return it->second;

        llvm::AllocaInst* slot = cg_.create_alloca_in_entry (name);
        named[name] = slot;

        return slot;
    }

    llvm::Value* ExprCodegen::emit_short_circuit (const BinaryExpr& node)
    {
        auto& b = cg_.builder();
        llvm::Function* fn = cg_.current_function();
        assert (fn && "ExprCodegen::emit_short_circuit: no active function");
        assert ((node.op() == BinOp::And || node.op() == BinOp::Or) &&
                "ExprCodegen::emit_short_circuit: expected && or ||");

        llvm::Value* lhs = emit (node.lhs());
        llvm::Value* lhs_bool = to_bool (cg_, lhs);

        const std::string prefix = (node.op() == BinOp::Or) ? "or" : "and";
        const int short_result = (node.op() == BinOp::Or) ? 1 : 0;

        llvm::BasicBlock* rhs_bb =
            llvm::BasicBlock::Create (cg_.llvm_context(), prefix + ".rhs", fn);
        llvm::BasicBlock* short_bb =
            llvm::BasicBlock::Create (cg_.llvm_context(), prefix + ".short", fn);
        llvm::BasicBlock* merge_bb =
            llvm::BasicBlock::Create (cg_.llvm_context(), prefix + ".merge", fn);

        if (node.op() == BinOp::Or)
            b.CreateCondBr (lhs_bool, short_bb, rhs_bb);
        else
            b.CreateCondBr (lhs_bool, rhs_bb, short_bb);

        b.SetInsertPoint (short_bb);
        llvm::Value* short_value =
            llvm::ConstantInt::get (cg_.get_i32_type(), short_result);
        b.CreateBr (merge_bb);
        short_bb = b.GetInsertBlock();

        b.SetInsertPoint (rhs_bb);
        llvm::Value* rhs = emit (node.rhs());
        llvm::Value* rhs_bool = to_bool (cg_, rhs);
        llvm::Value* rhs_i32 =
            b.CreateZExt (rhs_bool, cg_.get_i32_type(), prefix + ".rhs.i32");
        b.CreateBr (merge_bb);
        rhs_bb = b.GetInsertBlock();

        b.SetInsertPoint (merge_bb);
        llvm::PHINode* phi = b.CreatePHI (cg_.get_i32_type(), 2, prefix);
        phi->addIncoming (short_value, short_bb);
        phi->addIncoming (rhs_i32, rhs_bb);

        return phi;
    }

    // -----------------------------------------------------------------------
    // IntLiteral: produce an LLVM i32 constant
    //   99  -->  i32 99
    // -----------------------------------------------------------------------
    void ExprCodegen::visit (const IntLiteral& node)
    {
        result_ = llvm::ConstantInt::get (cg_.get_i32_type(), node.value(), /*isSigned=*/true);
    }

    // -----------------------------------------------------------------------
    // VarRef: load the current value from the variable's alloca slot
    //   x  -->  %tmp = load i32, ptr %x
    // -----------------------------------------------------------------------
    void ExprCodegen::visit (const VarRef& node)
    {
        llvm::AllocaInst* slot = get_or_create_slot (std::string (node.name()));
        result_ = cg_.builder().CreateLoad (cg_.get_i32_type(), slot, node.name());
    }

    // -----------------------------------------------------------------------
    // ReadExpr (?): call the runtime scan function and return its i32 result
    //   ?  -->  %scan = call i32 @__pcl_scan()
    // -----------------------------------------------------------------------
    void ExprCodegen::visit (const ReadExpr&)
    {
        llvm::Function* scan_fn =
            cg_.get_or_insert_extern_function ("__pcl_scan", cg_.get_i32_type(), {});

        result_ = cg_.builder().CreateCall (scan_fn, {}, "scan");
    }

    // -----------------------------------------------------------------------
    // AssignExpr: evaluate rhs, store into the alloca, return the value
    // Assignment is an expression in ParaCL: (x = 5) evaluates to 5
    //   x = expr  -->  store i32 %val, ptr %x   (result_ = %val)
    // -----------------------------------------------------------------------
    void ExprCodegen::visit (const AssignExpr& node)
    {
        llvm::Value* val  = emit (node.rhs());
        llvm::AllocaInst* slot = get_or_create_slot (std::string (node.name()));

        cg_.builder().CreateStore (val, slot);
        result_ = val; // assignment-as-expression returns the stored value
    }

    // -----------------------------------------------------------------------
    // UnaryExpr:
    //   -x  -->  %neg = sub i32 0, %x
    //   !x  -->  %cmp = icmp eq i32 %x, 0
    //            %not = zext i1 %cmp to i32
    // -----------------------------------------------------------------------
    void ExprCodegen::visit (const UnaryExpr& node)
    {
        llvm::Value* val = emit (node.child());
        auto& b = cg_.builder();

        switch (node.op())
        {
            case UnOp::Neg:
                result_ = b.CreateNeg (val, "neg");
                return;

            case UnOp::Not:
            {
                llvm::Value* is_zero = b.CreateICmpEQ (
                    val, llvm::ConstantInt::get (cg_.get_i32_type(), 0), "not_cmp");
                result_ = b.CreateZExt (is_zero, cg_.get_i32_type(), "not");
                return;
            }
        }

        throw std::logic_error ("ExprCodegen::visit(UnaryExpr): unknown UnOp");
    }

    // -----------------------------------------------------------------------
    // BinaryExpr: arithmetic, comparisons, logical xor and short-circuit
    // logical operators.
    // -----------------------------------------------------------------------
    void ExprCodegen::visit (const BinaryExpr& node)
    {
        if (node.op() == BinOp::Or || node.op() == BinOp::And)
        {
            result_ = emit_short_circuit (node);
            return;
        }

        auto& b = cg_.builder();

        llvm::Value* lhs = emit (node.lhs());
        llvm::Value* rhs = emit (node.rhs());

        switch (node.op())
        {
            case BinOp::Add: result_ = b.CreateAdd  (lhs, rhs, "add"); return;
            case BinOp::Sub: result_ = b.CreateSub  (lhs, rhs, "sub"); return;
            case BinOp::Mul: result_ = b.CreateMul  (lhs, rhs, "mul"); return;
            case BinOp::Div: result_ = b.CreateSDiv (lhs, rhs, "div"); return;
            case BinOp::Mod: result_ = b.CreateSRem (lhs, rhs, "mod"); return;

            case BinOp::Lt:
                result_ = b.CreateZExt (b.CreateICmpSLT (lhs, rhs, "lt_cmp"), cg_.get_i32_type(), "lt");
                return;

            case BinOp::Le:
                result_ = b.CreateZExt (b.CreateICmpSLE (lhs, rhs, "le_cmp"), cg_.get_i32_type(), "le");
                return;

            case BinOp::Gt:
                result_ = b.CreateZExt (b.CreateICmpSGT (lhs, rhs, "gt_cmp"), cg_.get_i32_type(), "gt");
                return;

            case BinOp::Ge:
                result_ = b.CreateZExt (b.CreateICmpSGE (lhs, rhs, "ge_cmp"), cg_.get_i32_type(), "ge");
                return;

            case BinOp::Eq:
                result_ = b.CreateZExt (b.CreateICmpEQ  (lhs, rhs, "eq_cmp"), cg_.get_i32_type(), "eq");
                return;

            case BinOp::Ne:
                result_ = b.CreateZExt (b.CreateICmpNE  (lhs, rhs, "ne_cmp"), cg_.get_i32_type(), "ne");
                return;

            case BinOp::Xor:
            {
                // logical xor: (lhs != 0) ^ (rhs != 0), result 0 or 1.
                llvm::Value* l_bool = b.CreateICmpNE (
                    lhs, llvm::ConstantInt::get (cg_.get_i32_type(), 0), "xor_l");
                llvm::Value* r_bool = b.CreateICmpNE (
                    rhs, llvm::ConstantInt::get (cg_.get_i32_type(), 0), "xor_r");
                result_ = b.CreateZExt (b.CreateXor (l_bool, r_bool, "xor_cmp"),
                                        cg_.get_i32_type(), "xor");
                return;
            }

            case BinOp::And:
            case BinOp::Or:
                break; // unreachable: caught above
        }

        throw std::logic_error ("ExprCodegen::visit(BinaryExpr): unhandled BinOp");
    }

} // namespace paracl::codegen
