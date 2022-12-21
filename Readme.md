# STDBUILD

Stdbuild is a prototype build system for C++ meant to remove the hassle of using third-party systems and scripting languages by giving a common
interface that allows you to write build scripts in C++.

Its goals are:
- Clean and easy to use scripting-like syntax.
- Cross compiler support.
- Cross platform support.
- Multithreaded compilation.
- Build caching.
- Package management.

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

struct pkg1 : stdbuild::package {
	pkg1(stdbuild::build_path _dir) {
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

struct project : stdbuild::package {
	project() : package("project") {
		name = "project";
		flags = { "-std=c++20" };
		sources = { "src/main.cpp" };
		pkgs = { pkg1("packages/pkg1/") };
	}
};

int main() {
	auto proj = project();
	stdbuild::create_executable(proj);
}
```
If the stdbuild executable is in your PATH and you are in the root directory of your project then simply calling
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

const stdbuild::package_list dependencies = {
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