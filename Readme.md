# STDBUILD

Stdbuild is a prototype C++ build system meant to remove the hassle of using third-party systems and scripting languages by giving a common interface written in C++.

Its goals are:
- As close to standard compliant as possible.
- Clean and easy to use scripting-like syntax.
- Little to no loss in features when compared to third-party build systems.
- Cross compiler support
- Cross platform support
- Multithreaded compilation
- Package build caching

Examples:  
This is a simple sample project consisting of the main project and one dependency called pkg1.  
Given the following project structue:  
```
build.cpp
include
src
│   main.cpp
packages
└───pkg1
    │   build.h
    └───include
    │   |   pkg1.h
    └───src
        |   pkg1.cpp
```

pkg1/build.h:
```cpp
#include <stdbuild>

struct pkg1 : std::build::package {
	pkg1(std::build::build_path _dir) {
		name = "pkg1";
		dir = _dir;
		flags = { "-std=c++20" };
		sources = { "src/pkg1.cpp" };
	}
};
```
build.cpp:
```cpp
#include <stdbuild>
// packages should always have their own build.h file
#include "packages/pkg1/build.h"

struct project : std::build::package {
	project() : package("project") {
		name = "project";
		flags = { "-std=c++20" };
		sources = { "src/main.cpp" };
		pkgs = { pkg1("packages/pkg1/") };
	}
};

int main() {
	auto proj = project();
	std::build::create_executable(proj);
}
```
If the stdbuild executable is in your PATH and you are in the root directory then simply calling
```shell
stdbuild
```
from the console will build the project.  
You can also specify a different build file by calling:  
```shell
stdbuild my_build_script.cpp
```
Run the project after it finishes building by adding the -r or -run flags:
```shell
stdbuild build.cpp -r
stdbuild build.cpp -run
```
Adding the -v or -verbose commands will give you more detailed information during the build process:
```shell
stdbuild build.cpp -v
stdbuild build.cpp -verbose
```
A separate dependency list can also be made by creating a header file like so:
```cpp
#include <stdbuild>
#include "pkg1/build.h"
#include "pkg2/build.h"
#include "pkg3/build.h"

const std::build::package_list dependencies = {
	pkg1("dir_to_pkg1"),
	pkg2("dir_to_pkg2"),
	pkg3("dir_to_pkg3")
};
```
and adding it to your build.cpp file:
```cpp
#include "path_to_dependencies_header"

pkgs = { dependencies };
```