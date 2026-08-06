find_package(Qt6 6.11 REQUIRED COMPONENTS Core Gui Qml Quick)

qt_standard_project_setup(REQUIRES 6.11)

qt_add_executable(mandel_qt_smoke
 main.cpp)

qt_add_qml_module(mandel_qt_smoke
 URI Mandel.ToolchainSmoke
 VERSION 1.0
 QML_FILES Main.qml)

target_link_libraries(mandel_qt_smoke PRIVATE
 Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick fractal_core)

set_target_properties(mandel_qt_smoke PROPERTIES
 WIN32_EXECUTABLE TRUE
 MACOSX_BUNDLE TRUE)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
 set_target_properties(mandel_qt_smoke PROPERTIES
  QT_ANDROID_APP_NAME "Mandel Qt Toolchain Smoke"
  QT_ANDROID_COMPILE_SDK_VERSION 36
  QT_ANDROID_MIN_SDK_VERSION 28
  QT_ANDROID_PACKAGE_NAME "org.fractalstudio.mandel.toolchainsmoke"
  QT_ANDROID_SDK_BUILD_TOOLS_REVISION "36.0.0"
  QT_ANDROID_TARGET_SDK_VERSION 36
  QT_ANDROID_VERSION_CODE 1
  QT_ANDROID_VERSION_NAME "0.1.0")
endif()

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
 add_test(NAME qt_toolchain_smoke
  COMMAND mandel_qt_smoke --smoke-auto-exit -platform offscreen)
 set_tests_properties(qt_toolchain_smoke PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()
