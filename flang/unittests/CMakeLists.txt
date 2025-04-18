include(AddFlangOffloadRuntime)

if (FLANG_EXPERIMENTAL_CUDA_RUNTIME)
  # If Fortran runtime is built as CUDA library, the linking
  # of targets that link flang_rt.runtime must be done
  # with CUDA_RESOLVE_DEVICE_SYMBOLS.
  # CUDA language must be enabled for CUDA_RESOLVE_DEVICE_SYMBOLS
  # to take effect.
  enable_language(CUDA)
endif()

add_custom_target(FlangUnitTests)
set_target_properties(FlangUnitTests PROPERTIES FOLDER "Flang/Tests")

include_directories("${FLANG_RT_SOURCE_DIR}/include")

function(add_flang_unittest_offload_properties target)
  # Do not apply runtime properties if not even compiling the runtime.
  if (NOT FLANG_INCLUDE_RUNTIME)
    return ()
  endif ()

  # Set CUDA_RESOLVE_DEVICE_SYMBOLS.
  if (FLANG_EXPERIMENTAL_CUDA_RUNTIME)
    set_target_properties(${target}
      PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON
      )
  endif()
  # Enable OpenMP offload during linking. We may need to replace
  # LINK_OPTIONS with COMPILE_OPTIONS when there are OpenMP offload
  # unittests.
  #
  # FIXME: replace 'native' in --offload-arch option with the list
  #        of targets that Fortran Runtime was built for.
  #        Common code must be moved from flang/runtime/CMakeLists.txt.
  if (NOT FLANG_EXPERIMENTAL_OMP_OFFLOAD_BUILD STREQUAL "off")
    set_target_properties(${target}
      PROPERTIES LINK_OPTIONS
      "-fopenmp;--offload-arch=native"
      )
  endif()
endfunction()

function(add_flang_unittest test_dirname)
  add_unittest(FlangUnitTests ${test_dirname} ${ARGN})
  add_flang_unittest_offload_properties(${test_dirname})
endfunction()

if (CXX_SUPPORTS_SUGGEST_OVERRIDE_FLAG)
  add_compile_options("-Wno-suggest-override")
endif()

function(add_flang_nongtest_unittest test_name)
  cmake_parse_arguments(ARG
    "SLOW_TEST"
    ""
    ""
    ${ARGN})

  if(ARG_SLOW_TEST)
      set(suffix .slow)
  else()
      set(suffix .test)
  endif()

  # Sources for runtime tests are in Flang-RT.
  set(test_filepath "${FLANG_RT_SOURCE_DIR}/unittests/Evaluate/${test_name}.cpp")
  if (NOT EXISTS "${test_filepath}")
    set(test_filepath "${test_name}.cpp")
  endif ()

  add_executable(${test_name}${suffix} "${test_filepath}")
  set_target_properties(${test_name}${suffix} PROPERTIES FOLDER "Flang/Tests/Unit")

  if (LLVM_LINK_LLVM_DYLIB AND NOT ARG_DISABLE_LLVM_LINK_LLVM_DYLIB)
    set(llvm_libs LLVM)
  else()
    llvm_map_components_to_libnames(llvm_libs Support)
  endif()
  target_link_libraries(${test_name}${suffix} ${llvm_libs} ${ARG_UNPARSED_ARGUMENTS})

  if(NOT ARG_SLOW_TEST)
    add_dependencies(FlangUnitTests ${test_name}${suffix})
  endif()

  add_flang_unittest_offload_properties(${test_name}${suffix})
endfunction()

add_subdirectory(Optimizer)
add_subdirectory(Common)
add_subdirectory(Decimal)
add_subdirectory(Evaluate)
if (FLANG_INCLUDE_RUNTIME)
  add_subdirectory(Runtime)
endif ()
add_subdirectory(Frontend)
