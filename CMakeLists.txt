cmake_minimum_required(VERSION 3.14)
project(ExampleLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Java COMPONENTS Runtime REQUIRED)

set(ANTLR_JAR "${CMAKE_CURRENT_SOURCE_DIR}/antlr-4.9.2-complete.jar")
if(NOT EXISTS "${ANTLR_JAR}")
    message(FATAL_ERROR "antlr-4.9.2-complete.jar not found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

set(GRAMMAR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/grammar/Example.g4)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Генерация ANTLR файлов
set(ANTLR_GENERATED_SOURCES
    ${GENERATED_DIR}/ExampleLexer.cpp
    ${GENERATED_DIR}/ExampleParser.cpp
    ${GENERATED_DIR}/ExampleBaseVisitor.cpp
    ${GENERATED_DIR}/ExampleVisitor.cpp
)

add_custom_command(
    OUTPUT ${ANTLR_GENERATED_SOURCES}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_JAR}
            -Dlanguage=Cpp
            -visitor
            -no-listener
            -o ${GENERATED_DIR}
            ${GRAMMAR_FILE}
    DEPENDS ${GRAMMAR_FILE}
    COMMENT "Generating ANTLR4 C++ parser"
)

# Заглушка для antlr4-runtime (мы подключаем исходники runtime)
# Скачиваем runtime один раз
set(RUNTIME_DIR ${CMAKE_CURRENT_BINARY_DIR}/antlr4_runtime)
if(NOT EXISTS ${RUNTIME_DIR}/lib/antlr4-runtime.a)
    message(STATUS "Downloading antlr4 runtime...")
    file(DOWNLOAD https://github.com/antlr/antlr4/archive/refs/tags/4.9.2.tar.gz
         ${CMAKE_CURRENT_BINARY_DIR}/antlr4-4.9.2.tar.gz
         SHOW_PROGRESS)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_CURRENT_BINARY_DIR}/antlr4-4.9.2.tar.gz
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} -E rename antlr4-4.9.2 antlr4_src
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    
    # Компилируем runtime
    file(MAKE_DIRECTORY ${RUNTIME_DIR}/build)
    execute_process(COMMAND ${CMAKE_COMMAND} 
                    -DCMAKE_CXX_STANDARD=17
                    -DCMAKE_BUILD_TYPE=Release
                    -DCMAKE_INSTALL_PREFIX=${RUNTIME_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/antlr4_src/runtime/Cpp
                    WORKING_DIRECTORY ${RUNTIME_DIR}/build)
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${RUNTIME_DIR}/build --target install
                    WORKING_DIRECTORY ${RUNTIME_DIR}/build)
endif()

add_executable(example
    src/main.cpp
    src/ExampleInterpreter.h
    ${ANTLR_GENERATED_SOURCES}
)

target_include_directories(example PRIVATE
    ${GENERATED_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${RUNTIME_DIR}/include/antlr4-runtime
)

target_link_directories(example PRIVATE
    ${RUNTIME_DIR}/lib
)

target_link_libraries(example PRIVATE
    antlr4-runtime
)