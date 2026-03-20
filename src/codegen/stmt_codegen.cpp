#include "codegen/stmt_codegen.hpp"
#include "codegen/codegen_context.hpp"
#include "codegen/expr_codegen.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>

#include "ast/stmt_nodes.hpp"

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

    StmtCodegen::StmtCodegen (CodegenContext& cg)
        : cg_ (cg)
    {
    }

    void StmtCodegen::emit (const Stmt* stmt)
    {
        assert (stmt && "StmtCodegen::emit: stmt must not be null");
        stmt->accept (*this);
    }

    // -----------------------------------------------------------------------
    // ExprStmt: evaluate the expression and discard the result
    //   x = 5;   -->  (store side-effect, result unused)
    //   ?;       -->  (call __pcl_scan, result unused)
    // -----------------------------------------------------------------------
    void StmtCodegen::visit (const ExprStmt& node)
    {
        ExprCodegen expr_cg { cg_ };
        expr_cg.emit (node.expr()); // Value* discarded
    }

    // -----------------------------------------------------------------------
    // PrintStmt: evaluate expr and call __pcl_print(i32)
    //   print x + 1  -->  %tmp = <expr>
    //                     call void @__pcl_print(i32 %tmp)
    // -----------------------------------------------------------------------
    void StmtCodegen::visit (const PrintStmt& node)
    {
        ExprCodegen expr_cg { cg_ };
        llvm::Value* val = expr_cg.emit (node.expr());

        std::array<llvm::Type*, 1> param_types = { cg_.get_i32_type() };
        llvm::Function* print_fn = cg_.get_or_insert_extern_function (
            "__pcl_print", cg_.get_void_type(), param_types);

        cg_.builder().CreateCall (print_fn, { val });
    }

    // -----------------------------------------------------------------------
    // BlockStmt: lower each child statement in order
    // -----------------------------------------------------------------------
    void StmtCodegen::visit (const BlockStmt& node)
    {
        for (const Stmt* s : node.stmts())
            emit (s);
    }

    // -----------------------------------------------------------------------
    // IfStmt: lower structured if/else into CFG
    //   cond -> if.then / if.else(or if.merge) -> if.merge
    // -----------------------------------------------------------------------
    void StmtCodegen::visit (const IfStmt& node)
    {
        ExprCodegen expr_cg { cg_ };
        llvm::Function* fn = cg_.current_function ();
        assert (fn && "StmtCodegen::visit(IfStmt): no active function");


        llvm::Value* cond_value = expr_cg.emit (node.cond());
        llvm::Value* cond_bool = to_bool (cg_, cond_value);

        llvm::BasicBlock* then_bb =
            llvm::BasicBlock::Create (cg_.llvm_context(), "if.then", fn);
        
        llvm::BasicBlock* else_bb = nullptr;
        if (node.else_branch())
            else_bb = llvm::BasicBlock::Create (cg_.llvm_context(), "if.else", fn);

        llvm::BasicBlock* merge_bb =
        llvm::BasicBlock::Create (cg_.llvm_context(), "if.merge", fn);

        cg_.builder().CreateCondBr (
            cond_bool,
            then_bb,
            node.else_branch() ? else_bb : merge_bb
        );

        cg_.builder().SetInsertPoint (then_bb);
        emit (node.then_branch());
        if (!cg_.builder().GetInsertBlock()->getTerminator())
            cg_.builder().CreateBr (merge_bb);

        if (else_bb)
        {
            cg_.builder().SetInsertPoint (else_bb);
            emit (node.else_branch());
            if (!cg_.builder().GetInsertBlock()->getTerminator())
                cg_.builder().CreateBr (merge_bb);
        }

        cg_.builder().SetInsertPoint (merge_bb);
    }

    // -----------------------------------------------------------------------
    // WhileStmt: lower loop into CFG with a condition block and back-edge
    //   current -> while.cond
    //   while.cond -> while.body / while.after
    //   while.body -> while.cond
    // -----------------------------------------------------------------------
    void StmtCodegen::visit (const WhileStmt& node)
    {
        ExprCodegen expr_cg { cg_ };
        llvm::Function* fn = cg_.current_function ();
        assert (fn && "StmtCodegen::visit(WhileStmt): no active function");

        llvm::BasicBlock* cond_bb =
            llvm::BasicBlock::Create (cg_.llvm_context(), "while.cond", fn);

        llvm::BasicBlock* body_bb =
            llvm::BasicBlock::Create (cg_.llvm_context(), "while.body", fn);
        
        llvm::BasicBlock* after_bb =
            llvm::BasicBlock::Create (cg_.llvm_context(), "while.after", fn);

        cg_.builder().CreateBr (cond_bb);
        cg_.builder().SetInsertPoint (cond_bb);

        llvm::Value* cond_value = expr_cg.emit (node.cond());
        llvm::Value* cond_bool = to_bool (cg_, cond_value);
            
        cg_.builder().CreateCondBr (
            cond_bool,
            body_bb,
            after_bb
        );

        cg_.builder().SetInsertPoint (body_bb);
        emit (node.body());
        if (!cg_.builder().GetInsertBlock()->getTerminator())
            cg_.builder().CreateBr (cond_bb);
        
        cg_.builder().SetInsertPoint (after_bb);
    }

} // namespace paracl::codegen
