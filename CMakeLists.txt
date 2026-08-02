cmake_minimum_required(VERSION 3.16)
project(fractal_studio_spine VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FRACTAL_WARNINGS -Wall -Wextra -Wpedantic -Werror)
find_package(Threads REQUIRED)
add_library(fractal_core STATIC
 shared/src/result.c shared/src/render_spec.c shared/src/render_manifest.c shared/src/render_progress.c
 shared/src/render_job.c shared/src/worker_status.c shared/src/render_artifact.c
 shared/src/render_failure.c runtime/src/memory_backend.c
 shared/src/module.c shared/src/module_registry.c shared/src/analysis.c runtime/src/socketable_runtime.c
 runtime/src/installed_modules.c runtime/src/artifact_sink.c runtime/src/scheduler.c
 server/computation/src/fractal_computation.c server/computation/cdc/src/fractal_cdc_computation.c
 server/computation/cdc/src/fractal_cdc_certificate.c
 server/computation/cdc/src/fractal_cdc_region_study.c
 server/computation/cdc/src/fractal_cdc_two_child.c
 server/computation/cdc/src/fractal_cdc_depth_two.c
 server/computation/cdc/src/fractal_cdc_refinement_contract.c
 server/renderer/src/fractal_renderer.c
 server/renderer/cdc/src/fractal_cdc_renderer.c server/renderer/cdc/src/fractal_cdc_model.c
 server/renderer/cdc/src/fractal_cdc_metrics.c server/renderer/cdc/src/fractal_cdc_trace.c)
target_include_directories(fractal_core PUBLIC shared/include runtime/include server/computation/include server/computation/cdc/include server/renderer/include server/renderer/cdc/include)
target_link_libraries(fractal_core PUBLIC Threads::Threads)
target_compile_options(fractal_core PRIVATE ${FRACTAL_WARNINGS})

add_library(fractal_renderer_adapter STATIC server/renderer/src/fractal_cpp_adapter.cpp)
target_include_directories(fractal_renderer_adapter PUBLIC server/renderer/include server/renderer/cdc/include)
target_link_libraries(fractal_renderer_adapter PUBLIC fractal_core)
target_compile_options(fractal_renderer_adapter PRIVATE ${FRACTAL_WARNINGS})

include(CTest)
if(BUILD_TESTING)
 add_executable(fractal_core_tests tests/native/test_core.c)
 target_link_libraries(fractal_core_tests PRIVATE fractal_core fractal_renderer_adapter)
 target_compile_options(fractal_core_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME fractal_core_tests COMMAND fractal_core_tests)
 add_executable(socketable_runtime_tests tests/native/test_socketable_runtime.c)
 target_link_libraries(socketable_runtime_tests PRIVATE fractal_core)
 target_compile_options(socketable_runtime_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME socketable_runtime_tests COMMAND socketable_runtime_tests)
 add_executable(thread_pool_scheduler_tests tests/native/test_thread_pool_scheduler.c)
 target_link_libraries(thread_pool_scheduler_tests PRIVATE fractal_core)
 target_compile_options(thread_pool_scheduler_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME thread_pool_scheduler_tests COMMAND thread_pool_scheduler_tests)
 add_executable(computation_socket_tests tests/native/test_computation_socket.c)
 target_link_libraries(computation_socket_tests PRIVATE fractal_core)
 target_compile_options(computation_socket_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME computation_socket_tests COMMAND computation_socket_tests)
 add_test(NAME milestone_boundaries COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/cmake/validate_milestone.cmake)
 set_tests_properties(fractal_core_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
