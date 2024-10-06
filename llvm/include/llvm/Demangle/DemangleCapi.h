#pragma once
#ifdef __cplusplus
extern "C" {
#endif
const char * capi_llvm_demangle(char *mangledName);
void capi_llvm_free(char *mangledName);
#ifdef __cplusplus
}
#endif
