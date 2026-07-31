set(required
 "CDC.pdf" "server.py" "server.cpp" "render.cpp" "render_engine.cpp" "index.html"
 "app_server" "render" "render_engine" "frames/frame_0000.bmp" "render.mp4"
 "docs/cdc_reference_index.md" "docs/cdc_glossary.md"
 "docs/cdc_interpretation_ledger.md" "docs/cdc_experiment_0.md"
 "docs/computation_substrate_inspection.md" "docs/computation_architecture.md")
list(APPEND required
 "docs/research/cdc_object_candidates.md" "docs/research/fractal_classification_predicates.md"
 "docs/research/cdc_region_study_0.md" "docs/research/cdc_region_guards.md"
 "docs/research/cdc_region_progress.md" "docs/research/cdc_ownership_boundary.md"
 "docs/research/cdc_region_certificate.md" "docs/research/cdc_object_of_descent_decision.md")
foreach(path IN LISTS required)
 if(NOT EXISTS "${SOURCE_DIR}/${path}")
  message(FATAL_ERROR "required preserved/traceability file missing: ${path}")
 endif()
 file(SIZE "${SOURCE_DIR}/${path}" size)
 if(size EQUAL 0)
  message(FATAL_ERROR "required file empty: ${path}")
 endif()
endforeach()
file(READ "${SOURCE_DIR}/CMakeLists.txt" build_definition)
if(build_definition MATCHES "[Pp]ython|[Tt]ermux|[Gg]radle|[Aa]ndroid|app_server|render_engine")
 message(FATAL_ERROR "native build definition has a forbidden legacy runtime dependency")
endif()
file(SHA256 "${SOURCE_DIR}/CDC.pdf" cdc_sha256)
if(NOT cdc_sha256 STREQUAL "5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c")
 message(FATAL_ERROR "CDC.pdf checksum changed")
endif()
