if(NOT DEFINED SOURCE_DIR OR NOT IS_DIRECTORY "${SOURCE_DIR}")
 message(FATAL_ERROR "SOURCE_DIR не указывает на корень репозитория")
endif()

set(retired_paths
 "server.py" "legacy/servers/server.py"
 "server.cpp" "legacy/servers/server.cpp"
 "app_server" "legacy/binaries/app_server"
 "render" "legacy/binaries/render"
 "render_engine" "legacy/binaries/render_engine"
 "render.cpp" "legacy/applications/render.cpp"
 "render_engine.cpp" "legacy/applications/render_engine.cpp")
foreach(path IN LISTS retired_paths)
 if(EXISTS "${SOURCE_DIR}/${path}")
  message(FATAL_ERROR "Намеренно снятый компонент снова присутствует: ${path}")
 endif()
endforeach()

set(retirement_records
 "docs/architecture/retired_python_server.md|81fa70a1a47321a4b122b0048f72ce2c3c9422f55a6d30a69d51394f7733f214"
 "docs/architecture/retired_native_http_server.md|ff2c3d6464962298dc040ece72ae3a631da55b4ba6a3195b6b4f3a7074e5bbe1"
 "docs/architecture/retired_foreign_executable_bundle.md|3258c1c944adaee9cf60de606b9629f29b04f15b259e3df3556ec6862bdcf749"
 "docs/architecture/retired_foreign_executable_bundle.md|69a50f78f8787ded0f56d1b8570d44366d1f824ce814204477eed75a1c33be08"
 "docs/architecture/retired_foreign_executable_bundle.md|6d5868a29e4f9be0b2d65bd55040bf9249af27324087266832ea75d3f47da19f"
 "docs/architecture/retired_standalone_renderers.md|6e40c8c23a52f8deffde730b6c34816152ee29cfa7b8e9830c8b3fafd94a1b60"
 "docs/architecture/retired_standalone_renderers.md|e9f683a9396ccf722db55d8f3276c16f7431738d43bd03d74e5e8ffb2ab668c5")
foreach(entry IN LISTS retirement_records)
 string(REPLACE "|" ";" fields "${entry}")
 list(GET fields 0 path)
 if(NOT EXISTS "${SOURCE_DIR}/${path}")
  message(FATAL_ERROR "Отсутствует запись о снятии с эксплуатации: ${path}")
 endif()
 file(READ "${SOURCE_DIR}/${path}" record)
 string(FIND "${record}" "RETIRED" retired_status)
 string(FIND "${record}" "git log" recovery)
 if(retired_status EQUAL -1 OR recovery EQUAL -1)
  message(FATAL_ERROR "Запись не содержит статус или способ восстановления: ${path}")
 endif()
 list(REMOVE_AT fields 0)
 foreach(checksum IN LISTS fields)
  string(FIND "${record}" "${checksum}" documented)
  if(documented EQUAL -1)
   message(FATAL_ERROR "Запись не содержит историческую сумму ${checksum}: ${path}")
  endif()
 endforeach()
endforeach()

set(preserved_data
 "index.html|cb043186dea5285592a1f5839f19f8786441c4feea2869a32e53b512caaa1fed"
 "style.css|f41fe2d8b7564e2ce9d4ec9b761ab9417388b83be1cb5e6c346b530eb956f269"
 "render.bmp|258b2b02823b7322b837d1e01002265de6bb8058c25b5a61e2c014aef4995f49"
 "render_hd.bmp|92d444a6a618c1a3501b36d2c2e679121a62b379b70b21cb247f5fc81ef37f60"
 "render.mp4|691886656b8f564341b4646865388d86c2c8cc367603c877156095ac5861857b"
 "frames/frame_0000.bmp|f69e6545fa7801dd86f67bb2ed056c356312f0131a63c98922b67e817821661b")
foreach(entry IN LISTS preserved_data)
 string(REPLACE "|" ";" fields "${entry}")
 list(GET fields 0 path)
 list(GET fields 1 expected)
 file(SHA256 "${SOURCE_DIR}/${path}" actual)
 if(NOT actual STREQUAL expected)
  message(FATAL_ERROR "Изменилась сохранённая поверхность UI или носитель: ${path}")
 endif()
endforeach()

foreach(path IN ITEMS shared runtime server/renderer tests/native
 server/renderer/src/fractal_renderer.c server/renderer/src/fractal_cpp_adapter.cpp)
 if(NOT EXISTS "${SOURCE_DIR}/${path}")
  message(FATAL_ERROR "Отсутствует авторитетная или совместимая поверхность: ${path}")
 endif()
endforeach()
file(READ "${SOURCE_DIR}/CMakeLists.txt" active_text)
file(GLOB_RECURSE active_sources "${SOURCE_DIR}/runtime/*.c" "${SOURCE_DIR}/runtime/*.h" "${SOURCE_DIR}/shared/*.c" "${SOURCE_DIR}/shared/*.h" "${SOURCE_DIR}/server/renderer/*.c" "${SOURCE_DIR}/server/renderer/*.h" "${SOURCE_DIR}/server/renderer/*.cpp")
foreach(source IN LISTS active_sources)
 file(READ "${source}" content)
 string(APPEND active_text "${content}")
endforeach()
foreach(token IN ITEMS "legacy/servers/server.cpp" "legacy/binaries/app_server" "legacy/binaries/render" "legacy/binaries/render_engine" "legacy/applications/render.cpp" "legacy/applications/render_engine.cpp")
 string(FIND "${active_text}" "${token}" found)
 if(NOT found EQUAL -1)
  message(FATAL_ERROR "Активная сборка ссылается на снятый компонент: ${token}")
 endif()
endforeach()
file(SHA256 "${SOURCE_DIR}/CDC.pdf" cdc_sha256)
if(NOT cdc_sha256 STREQUAL "5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c")
 message(FATAL_ERROR "Изменилась контрольная сумма CDC.pdf")
endif()
