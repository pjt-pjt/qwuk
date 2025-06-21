cmake_minimum_required(VERSION 3.15)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)
set(PROJECT qwuk)

project(${PROJECT} VERSION 0.1
        DESCRIPTION "Quake 1 viewer"
        LANGUAGES CXX C)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
add_compile_options(-Wall -Wextra -Wpedantic -Werror -pedantic-errors) 

set(APP_NAME qwuk)
set(IMGUI_LIB imgui)

set(SYTEM_INCLUDE_ROOT $ENV{MINGW64_HOME}/include)
set(ENGINE_DIR engine)
set(BACKEND_DIR opengl)
set(GAME_DIR game)

set(QUAKE_SRC application.cpp quake.cpp bsp.cpp bspfile.cpp entities.cpp filebuffer.cpp gameinterface.cpp gamemodule.cpp
              application.h quake.h bsp.h bspfile.h entities.h filebuffer.h gameinterface.h gamemodule.h vertex.h plane.h math.h)
list(TRANSFORM QUAKE_SRC PREPEND "${ENGINE_DIR}/")

set(BACKEND_SRC pipeline.cpp program.cpp storagebuffer.cpp graphics.cpp application_opengl.cpp)
set(BACKEND_HDR pipeline.h program.h storagebuffer.h graphics.h )
list(TRANSFORM  BACKEND_SRC PREPEND "${BACKEND_DIR}/")
list(TRANSFORM  BACKEND_HDR PREPEND "${BACKEND_DIR}/")

set(GAME_HDR game.h stringtools.h)
list(TRANSFORM GAME_HDR PREPEND "${GAME_DIR}/")

include_directories(SYSTEM ${SYTEM_INCLUDE_ROOT})
include_directories($ENV{IMGUI_HOME})
include_directories($ENV{IMGUI_HOME}/backends)

add_executable(${APP_NAME} ${QUAKE_SRC} ${BACKEND_SRC} ${BACKEND_HDR} ${GAME_HDR} main.cpp version.h)
target_include_directories(${APP_NAME} PRIVATE ${PROJECT_SOURCE_DIR} ${ENGINE_DIR} ${BACKEND_DIR} ${GAME_DIR})
target_link_libraries(${APP_NAME} PUBLIC ${IMGUI_LIB} mingw32 ltdl SDL2main SDL2 glew32 freeglut glu32 opengl32 comdlg32 ole32)

target_compile_features(${APP_NAME} PUBLIC cxx_std_17)


# imgui library
add_library(${IMGUI_LIB}    $ENV{IMGUI_HOME}/imgui.cpp $ENV{IMGUI_HOME}/imgui_draw.cpp $ENV{IMGUI_HOME}/imgui_tables.cpp $ENV{IMGUI_HOME}/imgui_widgets.cpp
                            $ENV{IMGUI_HOME}/backends/imgui_impl_sdl2.cpp $ENV{IMGUI_HOME}/backends/imgui_impl_opengl3.cpp)
target_include_directories(${IMGUI_LIB} PRIVATE ${SYTEM_INCLUDE_ROOT}/SDL2 ${SYTEM_INCLUDE_ROOT}/GL)
target_compile_features(${IMGUI_LIB} PUBLIC cxx_std_17)

#set(IMGUI_DEMO 1)
if(IMGUI_DEMO)
    target_compile_definitions(${APP_NAME} PRIVATE IMGUI_DEMO)
    target_sources(${IMGUI_LIB} PRIVATE $ENV{IMGUI_HOME}/imgui_demo.cpp)
endif()


# game code
add_library(game SHARED game/game.c ${GAME_HDR})


# create version.h
add_custom_command(
    OUTPUT  ${PROJECT_SOURCE_DIR}/version.h
    DEPENDS ${PROJECT_SOURCE_DIR}/version.txt
    COMMAND bash -c '${PROJECT_SOURCE_DIR}/createversion.h.sh ${PROJECT_SOURCE_DIR}/version.txt ${PROJECT_SOURCE_DIR}/version.h'
    USES_TERMINAL
    VERBATIM
)
