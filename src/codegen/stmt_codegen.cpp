#include "codegen/stmt_codegen.hpp"
#include "codegen/codegen_context.hpp"

namespace paracl::codegen
{

    StmtCodegen::StmtCodegen (CodegenContext& cg)
        : cg_ (cg)
    {
    }

    void StmtCodegen::emit (const Stmt* stmt)
    {
        (void) stmt;
    }

} // namespace paracl::codegen
