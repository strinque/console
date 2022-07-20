# console library
A windows console c++ header-only library.

## Install
The `console` library can be used using modern cmake:

- copy vcpkg/ports/console to your vcpkg ports directory
- install `console` using `vcpkg.exe install console`

To import the library in cmake:
```cmake
    find_package(console CONFIG REQUIRED)
    target_link_libraries(main PRIVATE console::console)
```

## Examples

### Initialize Windows Console
- [x] activate utf-8 console mode
- [x] enable escape sequence (ex: display colors)
- [x] resize console if necessary

Code:
```cpp
#include <console/init.hpp>

// initialize windows console with size: 1080x620
console::init(1080, 620);
```

### Command Line Parser
- [x] use getopt to parse c command-line arguments
- [x] use fmt for formating/colors
- [x] handle boolean flags
- [x] handle text arguments
- [x] display usage automaticaly when no arguments are provided

Code:
```cpp
#include <console/parser.hpp>

// parse command-line arguments
bool debug = false;
std::string file;
std::string output;
console::parser parser("program", "1.0");
parser.add("d", "debug", "activate the debug mode", debug); // flag (default: false)
parser.add("f:", "file", "read the file", file, true);      // std::string argument (mandatory)
parser.add("o:", "output", "set the output file", output);  // std::string argument (optional)
bool ok = parser.parse(argc, argv);
```

Usage (without arguments):
```bash
./program.exe
usage: program.exe [options]
  -h,  --help        display the program help
  -v,  --version     display the program version
  -d,  --debug       activate the debug mode
  -f,  --file        read the file                 [mandatory]
  -o,  --output      set the output file
```

Usage (with arguments):
```bash
./program.exe -f test.cpp
debug: false
file: test.cpp
```

### Full Example

```cpp
#include <iostream>
#include <string>
#include <stdbool.h>
#include <console/init.hpp>
#include <console/parser.hpp>

int main(int argc, char** argv)
{
  // initialize windows console with size: 1080x620
  console::init(1080, 620);

  // parse command-line arguments
  bool debug = false;
  std::string file;
  std::string output;
  console::parser parser("program", "1.0");
  parser.add("d", "debug", "activate the debug mode", debug); // flag (default: false)
  parser.add("f:", "file", "read the file", file, true);      // std::string argument (mandatory)
  parser.add("o:", "output", "set the output file", output);  // std::string argument (optional)
  if (!parser.parse(argc, argv))
  {
    parser.print_usage();
    return -1;
  }

  std::cout << "debug: " << (debug ? "true" : "false" )<< std::endl;
  std::cout << "file: " << file << std::endl;
  if(!output.empty())
    std::cout << "output: " << output << std::endl;
  return 0;
}
```