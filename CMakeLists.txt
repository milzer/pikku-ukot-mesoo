cmake_minimum_required(VERSION 3.16)
project(pum LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(INPUT_DATA_DIR "${PROJECT_SOURCE_DIR}/assets")
set(OUTPUT_DATA_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(pum
    src/main.cpp
    src/pixelapp.cpp
    src/pum.cpp
)
target_include_directories(pum PRIVATE src)
target_link_libraries(pum PRIVATE sfml-graphics)
target_compile_features(pum PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET pum
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:pum>
        VERBATIM)
endif()

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${INPUT_DATA_DIR} ${OUTPUT_DATA_DIR}
    COMMENT "Copying assets to output directory"
)

add_dependencies(pum copy_assets)

install(TARGETS pum)
