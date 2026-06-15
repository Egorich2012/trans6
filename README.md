# Транслятор для языка с do-while, print, инкрементами/декрементами и сравнениями

## Грамматика
- do-while цикл
- print с несколькими аргументами
- префиксные ++ и --
- сравнения < и >
- блоки { ... }
- переменные и числа

## Сборка
```bash
mkdir build && cd build
cmake ..
cmake --build .

3. Автоматическая проверка всех тестов:
    ```
    chmod +x examples/run_tests.sh
    ./examples/run_tests.sh ./build/Release/main.exe
    ```
