#include "codegen/codegen_context.hpp"

#include <utility>

namespace paracl::codegen
{

    CodegenContext::CodegenContext (std::string module_name)
        : llvm_context_ (std::make_unique<llvm::LLVMContext>()),
          module_ (std::make_unique<llvm::Module> (std::move (module_name), *llvm_context_)),
          builder_ (std::make_unique<llvm::IRBuilder<>> (*llvm_context_))
    {
    }

    CodegenContext::~CodegenContext() = default;

} // namespace paracl::codegen
