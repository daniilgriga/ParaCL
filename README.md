![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![macOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

# ParaCL

Educational mini-interpreter for a C-like language.

## How to Install

```bash
git clone https://github.com/daniilgriga/ParaCL.git
cd ParaCL/
```

# How to Build


```bash
# Debug version:
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DSANITIZE=ON
cmake --build build/debug

# Release version:
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release -DSANITIZE=OFF -DBUILD_TESTS=OFF
cmake --build build/release
```

# How to Run

## Input Format

The program accepts a source file as a command-line argument:

```bash
./build/debug/paracl program.pcl
```

## Language Features

- Arithmetic: `+`, `-`, `*`, `/`, `%`, unary `-`
- Comparisons: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Variables: assignment and read
- Input/Output: `?` (read from stdin), `print` (write to stdout)
- Control flow: `if/else`, `while`
- Blocks: `{ }`

## Example Program

```bash
x = 10;
y = ?;

if (x > y)
{
    print x - y;
}
else
{
    print y - x;
}

i = 0;
while (i < 5)
{
    print i;
    i = i + 1;
}
```

## Example Output

```bash
# with input y = 3:
7
0
1
2
3
4
```

# Unit-tests

## How to Build

```bash
cmake -S . -B build/tests -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build/tests
```

# How to Run

```bash
cd build/tests
ctest --output-on-failure
```

# End-to-end tests
```
./tests/e2e/run_e2e.sh ./build/debug/paracl
```
