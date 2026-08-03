if(NOT DEFINED SOURCE_DIR OR NOT IS_DIRECTORY "${SOURCE_DIR}")
 message(FATAL_ERROR "SOURCE_DIR не указывает на корень репозитория")
endif()

function(require_file path)
 if(NOT EXISTS "${SOURCE_DIR}/${path}")
  message(FATAL_ERROR "Отсутствует обязательный файл локализации: ${path}")
 endif()
 file(SIZE "${SOURCE_DIR}/${path}" size)
 if(size EQUAL 0)
  message(FATAL_ERROR "Обязательный файл локализации пуст: ${path}")
 endif()
endfunction()

function(validate_utf8 path)
 file(READ "${SOURCE_DIR}/${path}" bytes HEX)
 string(LENGTH "${bytes}" byte_text_length)
 math(EXPR remainder "${byte_text_length} % 2")
 if(NOT remainder EQUAL 0)
  message(FATAL_ERROR "Нечётная HEX-длина при проверке UTF-8: ${path}")
 endif()
 set(position 0)
 while(position LESS byte_text_length)
  string(SUBSTRING "${bytes}" ${position} 2 lead_hex)
  math(EXPR lead "0x${lead_hex}")
  set(continuations 0)
  set(first_min 128)
  set(first_max 191)
  if(lead LESS_EQUAL 127)
   math(EXPR position "${position} + 2")
   continue()
  elseif(lead GREATER_EQUAL 194 AND lead LESS_EQUAL 223)
   set(continuations 1)
  elseif(lead EQUAL 224)
   set(continuations 2)
   set(first_min 160)
  elseif(lead GREATER_EQUAL 225 AND lead LESS_EQUAL 236)
   set(continuations 2)
  elseif(lead EQUAL 237)
   set(continuations 2)
   set(first_max 159)
  elseif(lead GREATER_EQUAL 238 AND lead LESS_EQUAL 239)
   set(continuations 2)
  elseif(lead EQUAL 240)
   set(continuations 3)
   set(first_min 144)
  elseif(lead GREATER_EQUAL 241 AND lead LESS_EQUAL 243)
   set(continuations 3)
  elseif(lead EQUAL 244)
   set(continuations 3)
   set(first_max 143)
  else()
   message(FATAL_ERROR "Недопустимый начальный байт UTF-8 в ${path} на смещении ${position}")
  endif()

  math(EXPR sequence_hex_length "(${continuations} + 1) * 2")
  math(EXPR sequence_end "${position} + ${sequence_hex_length}")
  if(sequence_end GREATER byte_text_length)
   message(FATAL_ERROR "Незавершённая последовательность UTF-8 в ${path}")
  endif()
  math(EXPR next_position "${position} + 2")
  string(SUBSTRING "${bytes}" ${next_position} 2 continuation_hex)
  math(EXPR continuation "0x${continuation_hex}")
  if(continuation LESS first_min OR continuation GREATER first_max)
   message(FATAL_ERROR "Недопустимый второй байт UTF-8 в ${path} на смещении ${next_position}")
  endif()
  if(continuations GREATER 1)
   foreach(index RANGE 2 ${continuations})
    math(EXPR continuation_position "${position} + ${index} * 2")
    string(SUBSTRING "${bytes}" ${continuation_position} 2 continuation_hex)
    math(EXPR continuation "0x${continuation_hex}")
    if(continuation LESS 128 OR continuation GREATER 191)
     message(FATAL_ERROR "Недопустимый последующий байт UTF-8 в ${path} на смещении ${continuation_position}")
    endif()
   endforeach()
  endif()
  set(position ${sequence_end})
 endwhile()
endfunction()

function(require_text path token)
 file(READ "${SOURCE_DIR}/${path}" content)
 string(FIND "${content}" "${token}" found)
 if(found EQUAL -1)
  message(FATAL_ERROR "В ${path} отсутствует зафиксированный токен: ${token}")
 endif()
endfunction()

set(authoritative_documents
 "README.md"
 "docs/architecture.md"
 "docs/renderer_evaluation.md"
 "docs/roadmap/next_attack_plan.md"
 "docs/architecture/FOUNDATION.md"
 "docs/architecture/repository_hygiene_audit.md"
 "docs/architecture/runtime_registry.md"
 "docs/architecture/socketable_runtime.md"
 "docs/architecture/artifact_sink.md"
 "docs/architecture/artifact_lifecycle.md"
 "docs/architecture/analysis_socket.md"
 "docs/architecture/analysis_records.md"
 "docs/architecture/analysis_pipeline.md"
 "docs/architecture/escape_classification_summary.md"
 "docs/architecture/iteration_histogram_analyzer.md"
 "docs/architecture/spatial_workload_grid_analyzer.md"
 "docs/architecture/bounded_analyzer_chain.md"
 "docs/architecture/job_specification.md"
 "docs/architecture/deterministic_thread_pool_scheduler.md"
 "docs/architecture/computation_socket.md"
 "docs/architecture/binary64_numeric_socket.md"
 "docs/architecture/formula_socket.md"
 "docs/architecture/field_ownership.md"
 "docs/architecture/module_compatibility.md"
 "docs/architecture/language_policy.md"
 "docs/architecture/terminology_glossary.md"
 "docs/architecture/localization_inventory.md"
 "docs/architecture/translation_ledger.md"
 "docs/maintainer_guide.en.md"
 "docs/architecture/localization_summary.en.md")

foreach(path IN LISTS authoritative_documents)
 require_file("${path}")
 validate_utf8("${path}")
endforeach()

set(translated_comment_files
 "runtime/include/fractal/installed_modules.h"
 "runtime/src/memory_backend.c"
 "server/computation/cdc/include/fractal_cdc_substrate.h"
 "server/computation/cdc/src/fractal_cdc_region_study.c"
 "server/computation/cdc/src/fractal_cdc_two_child.c"
 "server/renderer/cdc/include/fractal_cdc_metrics.h"
 "server/renderer/cdc/include/fractal_cdc_model.h"
 "server/renderer/cdc/include/fractal_cdc_renderer.h"
 "server/renderer/cdc/include/fractal_cdc_trace.h"
 "server/renderer/cdc/src/fractal_cdc_metrics.c"
 "server/renderer/cdc/src/fractal_cdc_model.c"
 "server/renderer/cdc/src/fractal_cdc_trace.c"
 "server/renderer/include/fractal_cpp_adapter.h"
 "shared/include/fractal/analysis.h"
 "shared/include/fractal/formula.h"
 "shared/include/fractal/scheduler.h"
 "shared/include/fractal/socketable_runtime.h"
 "shared/src/analysis.c"
 "runtime/src/formula_quadratic.c"
 "shared/src/render_spec.c"
 "tests/native/test_core.c")
foreach(path IN LISTS translated_comment_files)
 validate_utf8("${path}")
endforeach()

set(machine_sources)
file(GLOB_RECURSE machine_sources
 "${SOURCE_DIR}/shared/*.c" "${SOURCE_DIR}/shared/*.h"
 "${SOURCE_DIR}/runtime/*.c" "${SOURCE_DIR}/runtime/*.h")
set(machine_text "")
foreach(path IN LISTS machine_sources)
 file(READ "${path}" content)
 string(APPEND machine_text "${content}")
endforeach()

set(required_machine_ids
 "fractal.formula.mandelbrot.v1"
 "fractal.formula.julia.v1"
 "formula.mandelbrot.quadratic"
 "formula.julia.quadratic"
 "fractal.numeric.binary64.v1"
 "fractal.compute.scalar.v1"
 "refinement.none"
 "refinement.cdc.unavailable"
 "scheduler.serial.row-major"
 "fractal.scheduler.serial.v1"
 "fractal.scheduler.thread-pool.v1"
 "raster.native.iteration-bgr8"
 "encoder.bmp.v3"
 "memory.system.scoped"
 "memory.ouro.unavailable"
 "telemetry.noop"
 "platform.host.c11"
 "sink.memory.bounded"
 "sink.file.host"
 "fractal.analyzer.passthrough"
 "fractal.analyzer.escape-classification-summary"
 "fractal.analyzer.iteration-histogram"
 "fractal.analyzer.spatial-workload-grid"
 "fractal.analysis.escape-classification-summary.v1"
 "fractal.analysis.iteration-histogram.v1"
 "fractal.analysis.spatial-workload-grid.v1"
 "fractal.scheduler.work-unit.contiguous-rows.v1")
foreach(module_id IN LISTS required_machine_ids)
 string(FIND "${machine_text}" "${module_id}" found)
 if(found EQUAL -1)
  message(FATAL_ERROR "Машинный ID отсутствует или изменён переводом: ${module_id}")
 endif()
endforeach()

require_text("tests/native/test_numeric_socket.c" "616b9a3b92e787e5")
require_text("tests/native/test_numeric_socket.c" "2f300bf0f7ea5244")
require_text("tests/native/test_socketable_runtime.c" "0116ac94c4b64d99")
foreach(identity IN ITEMS
 "99ec88c2a0f8bac3" "4866aacc38290b5f" "fb1a83bd5ca28e5f"
 "0fb4458e08bad6e1" "b272f08b0bbdca2b" "4d4aa95bd137ec87")
 require_text("tests/native/test_socketable_runtime.c" "${identity}")
endforeach()

file(SHA256 "${SOURCE_DIR}/CDC.pdf" cdc_sha256)
if(NOT cdc_sha256 STREQUAL "5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c")
 message(FATAL_ERROR "Изменилась контрольная сумма CDC.pdf")
endif()

execute_process(
 COMMAND git diff --quiet HEAD -- CDC.pdf app_server render render_engine
  render.bmp render_hd.bmp render.mp4 frames
 WORKING_DIRECTORY "${SOURCE_DIR}"
 RESULT_VARIABLE generated_diff)
if(NOT generated_diff EQUAL 0)
 message(FATAL_ERROR "Изменён сохранённый или сгенерированный артефакт")
endif()

execute_process(
 COMMAND git ls-files -- build
 WORKING_DIRECTORY "${SOURCE_DIR}"
 OUTPUT_VARIABLE tracked_build
 OUTPUT_STRIP_TRAILING_WHITESPACE
 RESULT_VARIABLE tracked_build_result)
if(NOT tracked_build_result EQUAL 0 OR NOT tracked_build STREQUAL "")
 message(FATAL_ERROR "В каталоге build отслеживается вывод сборки")
endif()
