cmake_minimum_required(VERSION 3.26)

project(pikku-ukot-mesoo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PUM_VENDORED_SDL "Build SDL3 and SDL3_mixer from source and link them statically" OFF)

if(PUM_VENDORED_SDL)
    include(FetchContent)

    set(BUILD_SHARED_LIBS OFF)
    set(SDL_SHARED OFF)
    set(SDL_STATIC ON)
    set(SDL_TEST_LIBRARY OFF)

    # Only WAV (sound effects) and libxmp (the Impulse Tracker music) are needed
    set(SDLMIXER_VENDORED ON)
    set(SDLMIXER_DEPS_SHARED OFF)
    set(SDLMIXER_TESTS OFF)
    set(SDLMIXER_EXAMPLES OFF)
    set(SDLMIXER_INSTALL OFF)
    foreach(codec AIFF VOC AU FLAC GME MP3 MIDI OPUS VORBIS_STB VORBIS_VORBISFILE WAVPACK)
        set(SDLMIXER_${codec} OFF)
    endforeach()

    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.16
        GIT_SHALLOW ON)
    FetchContent_Declare(SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-3.2.4
        GIT_SHALLOW ON
        GIT_SUBMODULES external/libxmp)
    FetchContent_MakeAvailable(SDL3 SDL3_mixer)
else()
    find_package(SDL3 REQUIRED CONFIG)
    find_package(SDL3_mixer REQUIRED CONFIG)
endif()

# Stand-ins for the proprietary DieselEngine, FMOD 3 and the Win32/MSVC runtime
add_library(port STATIC
    port/compat/pathcompat.cpp
    port/diesel/DieselApplication.cpp
    port/diesel/DieselInput.cpp
    port/diesel/DieselSurface.cpp
    port/fmod/fmod.cpp
)
target_include_directories(port PUBLIC port/compat port/diesel port/fmod)
target_include_directories(port PRIVATE port/third_party)
if(NOT WIN32)
    target_include_directories(port PUBLIC port/compat/posix)
endif()
if(MSVC)
    target_compile_definitions(port PUBLIC _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
endif()
target_link_libraries(port PUBLIC SDL3::SDL3 SDL3_mixer::SDL3_mixer)

add_executable(pum WIN32
    src/Main.cpp
    src/pumApp.cpp
    src/pumGlobals.cpp
    src/pumLevel.cpp
    src/pumLilfont.cpp
    src/pumParticle.cpp
    src/pumPlayer.cpp
    src/pumSound.cpp
    src/pumWeapon.cpp
    src/resource.cpp
)
target_include_directories(pum PRIVATE src)
target_link_libraries(pum PRIVATE port)

# The game reads (and rewrites) its data relative to the executable
set(PUM_DATA_DIRS musiq plv sfx uudet)
set(PUM_DATA_FILES aeaenet.puo aseet.puo menubg.jpg)

foreach(dir IN LISTS PUM_DATA_DIRS)
    add_custom_command(TARGET pum POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
                ${CMAKE_SOURCE_DIR}/${dir} $<TARGET_FILE_DIR:pum>/${dir})
endforeach()
foreach(file IN LISTS PUM_DATA_FILES)
    add_custom_command(TARGET pum POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/${file} $<TARGET_FILE_DIR:pum>)
endforeach()
# Copy SDL DLLs next to the executable; the vendored build links statically instead
if(WIN32 AND NOT PUM_VENDORED_SDL)
    add_custom_command(TARGET pum POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:pum> $<TARGET_FILE_DIR:pum>
        COMMAND_EXPAND_LISTS)
endif()

install(TARGETS pum RUNTIME DESTINATION .)
install(DIRECTORY ${PUM_DATA_DIRS} DESTINATION .)
install(FILES ${PUM_DATA_FILES} DESTINATION .)
if(WIN32 AND NOT PUM_VENDORED_SDL)
    install(FILES $<TARGET_RUNTIME_DLLS:pum> DESTINATION .)
endif()
