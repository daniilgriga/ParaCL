#pragma once

namespace paracl
{
    class Stmt;
}

namespace paracl::codegen
{
    class CodegenContext;

    class StmtCodegen final
    {
    public:
        explicit StmtCodegen (CodegenContext& cg);

        StmtCodegen (const StmtCodegen&) = delete;
        StmtCodegen& operator= (const StmtCodegen&) = delete;

        StmtCodegen (StmtCodegen&&) = delete;
        StmtCodegen& operator= (StmtCodegen&&) = delete;

        void emit (const Stmt* stmt);

    private:
        CodegenContext& cg_;
    };

} // namespace paracl::codegen
