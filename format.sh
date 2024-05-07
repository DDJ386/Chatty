find . -name "*.c" -o -name "*.cpp" -o -name "*.h" | xargs clang-format -style=file -i
