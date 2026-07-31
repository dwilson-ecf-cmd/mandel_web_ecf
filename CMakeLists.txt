cmake_minimum_required(VERSION 3.16)
project(fractal_studio_spine VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FRACTAL_WARNINGS -Wall -Wextra -Wpedantic -Werror)
add_library(fractal_core STATIC
 shared/src/result.c shared/src/render_spec.c shared/src/render_progress.c
 shared/src/render_job.c shared/src/worker_status.c shared/src/render_artifact.c
 shared/src/render_failure.c runtime/src/memory_backend.c
 server/renderer/src/fractal_renderer.c)
target_include_directories(fractal_core PUBLIC shared/include runtime/include server/renderer/include)
target_compile_options(fractal_core PRIVATE ${FRACTAL_WARNINGS})

add_library(fractal_renderer_adapter STATIC server/renderer/src/fractal_cpp_adapter.cpp)
target_include_directories(fractal_renderer_adapter PUBLIC server/renderer/include)
target_link_libraries(fractal_renderer_adapter PUBLIC fractal_core)
target_compile_options(fractal_renderer_adapter PRIVATE ${FRACTAL_WARNINGS})

include(CTest)
if(BUILD_TESTING)
 add_executable(fractal_core_tests tests/native/test_core.c)
 target_link_libraries(fractal_core_tests PRIVATE fractal_core fractal_renderer_adapter)
 target_compile_options(fractal_core_tests PRIVATE ${FRACTAL_WARNINGS})
 add_test(NAME fractal_core_tests COMMAND fractal_core_tests)
 set_tests_properties(fractal_core_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
