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
add_library(fractal_shared_contracts STATIC
 shared/src/result.c shared/src/workspace.c shared/src/render_spec.c shared/src/compatibility.c shared/src/connection.c shared/src/retry_policy.c shared/src/reacquire.c shared/src/intent_replay.c shared/src/protocol_security.c)
target_include_directories(fractal_shared_contracts PUBLIC shared/include)
target_compile_options(fractal_shared_contracts PRIVATE ${FRACTAL_WARNINGS})
add_library(fractal_android_client_contracts INTERFACE)
target_link_libraries(fractal_android_client_contracts INTERFACE fractal_shared_contracts)
add_library(fractal_desktop_client_core INTERFACE)
target_link_libraries(fractal_desktop_client_core INTERFACE fractal_shared_contracts)
add_library(fractal_backend_core INTERFACE)
target_link_libraries(fractal_backend_core INTERFACE fractal_shared_contracts)
add_library(fractal_platform_test STATIC platform/test/fake_transport.c platform/test/fake_tls.c)
target_include_directories(fractal_platform_test PUBLIC platform/test)
target_link_libraries(fractal_platform_test PUBLIC fractal_shared_contracts)
target_compile_options(fractal_platform_test PRIVATE ${FRACTAL_WARNINGS})

add_library(fractal_core STATIC
 shared/src/result.c shared/src/render_spec.c shared/src/render_manifest.c shared/src/render_progress.c
 shared/src/render_job.c shared/src/worker_status.c shared/src/render_artifact.c
 shared/src/render_failure.c runtime/src/memory_backend.c
 shared/src/module.c shared/src/module_registry.c shared/src/analysis.c shared/src/formula.c
 shared/src/workspace.c shared/src/pairing.c
 runtime/src/numeric_binary64.c runtime/src/formula_quadratic.c
 runtime/src/formula_mandelbrot.c runtime/src/formula_julia.c
 runtime/src/scalar_computation.c runtime/src/socketable_runtime.c runtime/src/thread_pool_scheduler.c
 runtime/src/installed_modules.c runtime/src/artifact_sink.c
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
target_link_libraries(fractal_core PUBLIC Threads::Threads fractal_shared_contracts)
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
 add_executable(numeric_socket_tests tests/native/test_numeric_socket.c)
 target_link_libraries(numeric_socket_tests PRIVATE fractal_core)
 target_compile_options(numeric_socket_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME numeric_socket_tests COMMAND numeric_socket_tests)
 add_executable(formula_socket_tests tests/native/test_formula_socket.c)
 target_link_libraries(formula_socket_tests PRIVATE fractal_core)
 target_compile_options(formula_socket_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME formula_socket_tests COMMAND formula_socket_tests)
 add_executable(header_contract_tests tests/native/test_header_contract.c)
 target_link_libraries(header_contract_tests PRIVATE fractal_core)
 target_compile_options(header_contract_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME header_contract_tests COMMAND header_contract_tests)
 add_executable(workspace_contract_tests tests/native/test_workspace.c)
 target_link_libraries(workspace_contract_tests PRIVATE fractal_core)
 target_compile_options(workspace_contract_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME workspace_contract_tests COMMAND workspace_contract_tests)
 add_executable(pairing_contract_tests tests/native/test_pairing.c)
 target_link_libraries(pairing_contract_tests PRIVATE fractal_core)
 target_compile_options(pairing_contract_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME pairing_contract_tests COMMAND pairing_contract_tests)
 add_executable(platform_contract_tests tests/native/test_platform_contracts.c)
 target_link_libraries(platform_contract_tests PRIVATE fractal_shared_contracts fractal_platform_test fractal_backend_core fractal_android_client_contracts fractal_desktop_client_core)
 target_compile_options(platform_contract_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME platform_contract_tests COMMAND platform_contract_tests)
 add_executable(security_stack_tests tests/native/test_security_stack.c)
 target_link_libraries(security_stack_tests PRIVATE fractal_shared_contracts fractal_platform_test fractal_backend_core fractal_android_client_contracts)
 target_compile_options(security_stack_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME security_stack_tests COMMAND security_stack_tests)
 add_test(NAME milestone_boundaries COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/cmake/validate_milestone.cmake)
 add_test(NAME legacy_isolation COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/cmake/validate_legacy_isolation.cmake)
 add_test(NAME localization_baseline COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/cmake/validate_localization.cmake)
 set_tests_properties(fractal_core_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
