cmake_minimum_required(VERSION 3.12)
project(transform_playground VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
include_directories(${CMAKE_SOURCE_DIR}/include)

# ImGui library
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
include_directories(${IMGUI_DIR})
set(imgui_sources 
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
)

# Raylib library
include_directories(${CMAKE_SOURCE_DIR}/libs/raylib/src)
include_directories(${CMAKE_SOURCE_DIR}/libs/rlImGui/)
add_subdirectory(libs/raylib EXCLUDE_FROM_ALL)

# JSON library
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(libs/json EXCLUDE_FROM_ALL)

# FMT library
include_directories(${CMAKE_SOURCE_DIR}/libs/fmt/include)
add_subdirectory(libs/fmt EXCLUDE_FROM_ALL)

# nativefiledialog-extended library
set(nfd "")
if (NOT EMSCRIPTEN)
  set(nfd "nfd")
  add_subdirectory(libs/nativefiledialog-extended EXCLUDE_FROM_ALL)
endif()

# Executable
set(sources
    src/app.cpp
    src/camera.cpp
    src/main.cpp
    src/scene.cpp
    src/nodes/lists.cpp
    src/nodes/nodes.cpp
    src/nodes/primitives.cpp
    src/nodes/transforms.cpp
    src/ui/about.cpp
    src/ui/camera.cpp
    src/ui/editor.cpp
    src/ui/menubar.cpp
    src/ui/oss.cpp
    src/ui/scenetree.cpp
    src/ui/ui.cpp
)

add_executable(${PROJECT_NAME} ${sources} libs/rlImGui/rlImGui.cpp ${imgui_sources})
target_link_libraries(${PROJECT_NAME} raylib nlohmann_json::nlohmann_json fmt ${nfd})

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} "-framework AppKit")
    target_link_libraries(${PROJECT_NAME} "-framework UniformTypeIdentifiers")
endif()

if (EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_FUNCTIONS='[\"_resize_event\", \"_hotkey_down\", \"_hotkey_up\", \"_main\"]' -sEXPORTED_RUNTIME_METHODS=ccall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASYNCIFY_IMPORTS=['emscripten_asm_const_int']")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${CMAKE_SOURCE_DIR}/src/wasm_template.html")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()