extern "C" {
#include "llvm/Demangle/DemangleCapi.h"
}
#include "llvm/Demangle/Demangle.h"

#include <stdlib.h>
#include <string.h>

using namespace llvm;

const char *capi_llvm_demangle(char *mangledName) {
  auto name = llvm::demangle(mangledName);
  auto demangledName = name.c_str();
  char *result = (char *)malloc(strlen(demangledName) + 1);
  if (result != NULL) {
    strcpy(result, demangledName);
  }
  return result;
}

void capi_llvm_free(char *mangledName) {
  free(mangledName);
}
