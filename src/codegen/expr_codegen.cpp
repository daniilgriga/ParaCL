#include "codegen/expr_codegen.hpp"
#include "codegen/codegen_context.hpp"

#include <stdexcept>

namespace paracl::codegen
{

    ExprCodegen::ExprCodegen (CodegenContext& cg)
        : cg_ (cg)
    {
    }

    llvm::Value* ExprCodegen::emit (const Expr* expr)
    {
        (void) expr;
        throw std::logic_error ("ExprCodegen::emit is not implemented yet");
    }

} // namespace paracl::codegen
