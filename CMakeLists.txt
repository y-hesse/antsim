cmake_minimum_required(VERSION 3.16)
project(antsim LANGUAGES CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(antsim src/main.cpp)
target_link_libraries(antsim PRIVATE sfml-graphics)
target_compile_features(antsim PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
            TARGET antsim
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:antsim>
            VERBATIM)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(antsim PUBLIC OpenMP::OpenMP_CXX)
endif()

install(TARGETS antsim)
