#include "codegen/stmt_codegen.hpp"
#include "codegen/codegen_context.hpp"
#include "codegen/expr_codegen.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

#include <llvm/IR/Value.h>

#include "ast/stmt_nodes.hpp"

namespace paracl::codegen
{

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
    // IfStmt / WhileStmt: require BasicBlock branching - CFG zone
    // -----------------------------------------------------------------------
    void StmtCodegen::visit (const IfStmt&)
    {
        throw std::logic_error (
            "StmtCodegen::visit(IfStmt): CFG lowering not implemented");
    }

    void StmtCodegen::visit (const WhileStmt&)
    {
        throw std::logic_error (
            "StmtCodegen::visit(WhileStmt): CFG lowering not implemented");
    }

} // namespace paracl::codegen
