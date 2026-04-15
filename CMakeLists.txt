cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(antlr4_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(ANTLR4_WITH_STATIC_CRT OFF CACHE BOOL "" FORCE)
set(ANTLR_BUILD_CPP_TESTS OFF CACHE BOOL "" FORCE)
set(ANTLR_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(DISABLE_WARNINGS ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    antlr4
    GIT_REPOSITORY https://github.com/antlr/antlr4.git
    GIT_TAG        4.13.2
)

FetchContent_GetProperties(antlr4)
if(NOT antlr4_POPULATED)
    FetchContent_Populate(antlr4)
endif()

add_subdirectory(${antlr4_SOURCE_DIR}/runtime/Cpp ${antlr4_BINARY_DIR}/runtime/Cpp)

# Java для ANTLR
find_package(Java COMPONENTS Runtime REQUIRED)

set(ANTLR_JAR ${CMAKE_CURRENT_BINARY_DIR}/antlr-4.13.2-complete.jar)

if(NOT EXISTS "${ANTLR_JAR}")
    file(DOWNLOAD
        "https://www.antlr.org/download/antlr-4.13.2-complete.jar"
        "${ANTLR_JAR}"
    )
endif()

# ─────────────── ГРАММАТИКА (НО имя как у него) ───────────────
set(GRAMMAR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/grammar/Example.g4)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

set(ANTLR_GENERATED_SOURCES
    ${GENERATED_DIR}/ExampleLexer.cpp
    ${GENERATED_DIR}/ExampleParser.cpp
    ${GENERATED_DIR}/ExampleBaseVisitor.cpp
    ${GENERATED_DIR}/ExampleBaseListener.cpp
    ${GENERATED_DIR}/ExampleVisitor.cpp
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
    DEPENDS ${GRAMMAR_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# ─────────────── ОСНОВНОЙ EXEC ───────────────
add_executable(main
    src/main.cpp
    src/Analyzer.cpp
    ${ANTLR_GENERATED_SOURCES}
)

target_include_directories(main PRIVATE
    ${GENERATED_DIR}
    src
)

target_link_libraries(main PRIVATE antlr4_static)