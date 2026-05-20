cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(antlr4_lab LANGUAGES CXX)

# C++17 обязателен для antlr4-runtime (std::any, std::string_view)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ────────────────────────────────────────────────────────────────────────────
# 1. Подключение ANTLR4 C++ Runtime
#
#    Стратегия:
#      a) Ищем предустановленный runtime
#      b) Если не найден — скачиваем через FetchContent (локальная разработка)
# ────────────────────────────────────────────────────────────────────────────
find_library(ANTLR4_LIB antlr4-runtime)
find_path(ANTLR4_INCLUDE antlr4-runtime.h PATH_SUFFIXES antlr4-runtime)

if(ANTLR4_LIB AND ANTLR4_INCLUDE)
    message(STATUS "Found pre-installed ANTLR4 runtime: ${ANTLR4_LIB}")
    message(STATUS "ANTLR4 headers: ${ANTLR4_INCLUDE}")

    add_library(antlr4_static STATIC IMPORTED)
    set_target_properties(antlr4_static PROPERTIES
        IMPORTED_LOCATION "${ANTLR4_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${ANTLR4_INCLUDE}"
    )
else()
    message(STATUS "Pre-installed ANTLR4 runtime not found — using FetchContent")
    message(STATUS "(первая сборка займёт некоторое время — компилируется ANTLR4 runtime)")

    include(FetchContent)

    set(ANTLR4_WITH_STATIC_CRT OFF CACHE BOOL "" FORCE)
    set(ANTLR_BUILD_CPP_TESTS OFF CACHE BOOL "" FORCE)
    set(ANTLR_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(DISABLE_WARNINGS ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        antlr4-runtime
        GIT_REPOSITORY https://github.com/antlr/antlr4.git
        GIT_TAG        dev
        SOURCE_SUBDIR  runtime/Cpp
    )
    FetchContent_MakeAvailable(antlr4-runtime)
endif()

# ────────────────────────────────────────────────────────────────────────────
# 2. ANTLR4 Tool (JAR-файл для генерации парсера из .g4)
#    Java нужна ТОЛЬКО на этапе генерации.
#    В итоговом бинарнике Java не участвует.
#
#    Если в окружении задан ANTLR_JAR_PATH (Docker), используем его.
#    Иначе скачиваем в build/.
# ────────────────────────────────────────────────────────────────────────────
find_package(Java COMPONENTS Runtime REQUIRED)

if(DEFINED ENV{ANTLR_JAR_PATH} AND EXISTS "$ENV{ANTLR_JAR_PATH}")
    set(ANTLR_JAR "$ENV{ANTLR_JAR_PATH}")
    message(STATUS "Using pre-downloaded ANTLR JAR: ${ANTLR_JAR}")
else()
    set(ANTLR_JAR ${CMAKE_CURRENT_BINARY_DIR}/antlr-4.13.2-complete.jar)
    if(NOT EXISTS "${ANTLR_JAR}")
        message(STATUS "Downloading ANTLR 4.13.2 JAR...")
        file(DOWNLOAD
            "https://www.antlr.org/download/antlr-4.13.2-complete.jar"
            "${ANTLR_JAR}"
            SHOW_PROGRESS
        )
    endif()
endif()

# ────────────────────────────────────────────────────────────────────────────
# 3. Генерация парсера из Example.g4 (чистая грамматика, для Visitor)
# ────────────────────────────────────────────────────────────────────────────
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
    COMMENT "Generating ANTLR4 C++ parser from Example.g4"
)

# ────────────────────────────────────────────────────────────────────────────
# 4. Основной исполняемый файл (Visitor-подход)
# ────────────────────────────────────────────────────────────────────────────
add_executable(main
    src/main.cpp
    ${ANTLR_GENERATED_SOURCES}
)

target_include_directories(main PRIVATE
    ${GENERATED_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(main PRIVATE antlr4_static)

# ────────────────────────────────────────────────────────────────────────────
# 5. (Опционально) Генерация из ExampleActions.g4 (встроенные действия)
# ────────────────────────────────────────────────────────────────────────────
set(ACTIONS_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/grammar/ExampleActions.g4)
set(ACTIONS_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_actions)
file(MAKE_DIRECTORY ${ACTIONS_DIR})

set(ACTIONS_GENERATED_SOURCES
    ${ACTIONS_DIR}/ExampleActionsLexer.cpp
    ${ACTIONS_DIR}/ExampleActionsParser.cpp
    ${ACTIONS_DIR}/ExampleActionsBaseVisitor.cpp
    ${ACTIONS_DIR}/ExampleActionsBaseListener.cpp
    ${ACTIONS_DIR}/ExampleActionsVisitor.cpp
    ${ACTIONS_DIR}/ExampleActionsListener.cpp
)

add_custom_command(
    OUTPUT ${ACTIONS_GENERATED_SOURCES}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_JAR}
        -Dlanguage=Cpp
        -visitor
        -listener
        -o ${ACTIONS_DIR}
        ${ACTIONS_GRAMMAR}
    DEPENDS ${ACTIONS_GRAMMAR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating ANTLR4 C++ parser from ExampleActions.g4"
)

add_executable(main_actions
    src/main_actions.cpp
    ${ACTIONS_GENERATED_SOURCES}
)

target_include_directories(main_actions PRIVATE
    ${ACTIONS_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(main_actions PRIVATE antlr4_static)

# ────────────────────────────────────────────────────────────────────────────
# 6. Тесты (позитивные и негативные)
# ────────────────────────────────────────────────────────────────────────────

# Включить тестирование CTest
enable_testing()

# Функция для добавления одного теста
function(add_trans_test TEST_NAME TEST_FILE SHOULD_PASS)
    add_test(NAME ${TEST_NAME}
        COMMAND main ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FILE}
    )
    if(NOT SHOULD_PASS)
        set_tests_properties(${TEST_NAME} PROPERTIES WILL_FAIL TRUE)
    endif()
endfunction()

# --- Позитивные тесты (должны пройти успешно) ---
add_trans_test(positive_do_while_basic "examples/positive/do_while_basic.trans" TRUE)
add_trans_test(positive_nested_loops "examples/positive/nested_loops.trans" TRUE)
add_trans_test(positive_inc_dec "examples/positive/inc_dec.trans" TRUE)
add_trans_test(positive_comparisons "examples/positive/comparisons.trans" TRUE)
add_trans_test(positive_mixed_print "examples/positive/mixed_print.trans" TRUE)
add_trans_test(positive_complex "examples/positive/complex.trans" TRUE)
add_trans_test(positive_variables "examples/positive/variables.trans" TRUE)

# --- Негативные тесты (должны завершаться с ошибкой) ---
add_trans_test(negative_syntax_do_while "examples/negative/syntax_do_while.trans" FALSE)
add_trans_test(negative_missing_condition "examples/negative/missing_condition.trans" FALSE)
add_trans_test(negative_invalid_var_name "examples/negative/invalid_var_name.trans" FALSE)
add_trans_test(negative_type_mismatch "examples/negative/type_mismatch.trans" FALSE)
add_trans_test(negative_uninit_var "examples/negative/uninit_var.trans" FALSE)
add_trans_test(negative_infinite_loop "examples/negative/infinite_loop.trans" FALSE)
add_trans_test(negative_unmatched_brace "examples/negative/unmatched_brace.trans" FALSE)
add_trans_test(negative_wrong_do_while_order "examples/negative/wrong_do_while_order.trans" FALSE)
add_trans_test(negative_empty_loop_body "examples/negative/empty_loop_body.trans" FALSE)

# Вывод информации о тестах
message(STATUS "Added ${CMAKE_TESTING_ENABLED} tests: 7 positive, 9 negative")