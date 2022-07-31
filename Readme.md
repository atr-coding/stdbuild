# What does a build system in C++ need to do?
A C++ build system needs to be able to bring together a collection of information such as:
* Source files
* Flags
* Include Directories
* Library Directories
* Libraries

It also needs to be able to support a plethera of compilers and operating systems:
* MSVC, GCC, Clang
* Windows, Linux

C++ applications often rely on dependencies, these dependencies are libraries that take on different forms:
* Header only
* Pre-compiled Static/Dynamic
* Compile from source

# How do we go about doing this?

Starting with dependencies, the information they produce is always the same as for the main project itself, include/library directories, flags, libraries, and dependencies of their own.
 
These projects can be broken up into separate parts, and a build hierarchy can be established.
 
Once a dependency is compiled, it needs to create a export structure that will pass the neccessary information up to the next step in the build system.