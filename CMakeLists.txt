cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(ExampleLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(ANTLR_BUILD_CPP_TESTS OFF CACHE BOOL "" FORCE)
set(ANTLR_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(DISABLE_WARNINGS ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    antlr4-runtime
    GIT_REPOSITORY https://github.com/antlr/antlr4.git
    GIT_TAG        4.13.1
    SOURCE_SUBDIR  runtime/Cpp
)
FetchContent_MakeAvailable(antlr4-runtime)

find_package(Java COMPONENTS Runtime REQUIRED)
set(ANTLR_JAR ${CMAKE_CURRENT_BINARY_DIR}/antlr-4.13.1-complete.jar)
if(NOT EXISTS "${ANTLR_JAR}")
    message(STATUS "Downloading ANTLR 4.13.1 JAR...")
    file(DOWNLOAD 
        "https://www.antlr.org/download/antlr-4.13.1-complete.jar" 
        "${ANTLR_JAR}" 
        SHOW_PROGRESS TLS_VERIFY ON
    )
endif()

set(GRAMMAR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/grammar/Example.g4)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

set(ANTLR_GENERATED_SOURCES
    ${GENERATED_DIR}/ExampleLexer.cpp
    ${GENERATED_DIR}/ExampleParser.cpp
    ${GENERATED_DIR}/ExampleBaseVisitor.cpp
    ${GENERATED_DIR}/ExampleVisitor.cpp
    ${GENERATED_DIR}/ExampleBaseListener.cpp
    ${GENERATED_DIR}/ExampleListener.cpp
)

add_custom_command(
    OUTPUT ${ANTLR_GENERATED_SOURCES}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_JAR}
        -Dlanguage=Cpp
        -visitor
        -listener
        -o ${GENERATED_DIR}
        ${GRAMMAR_FILE}
    DEPENDS ${GRAMMAR_FILE} ${ANTLR_JAR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating ANTLR4 C++ parser from Example.g4"
    VERBATIM
)

add_executable(main 
    src/main.cpp 
    ${ANTLR_GENERATED_SOURCES}
)

target_include_directories(main PRIVATE 
    ${GENERATED_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${antlr4-runtime_SOURCE_DIR}/runtime/Cpp/runtime/src
)

target_link_libraries(main PRIVATE antlr4_static)

if(MSVC)
    target_compile_options(main PRIVATE /wd4251 /wd4275 /wd4100 /wd4244)
endif()
