#include <iostream>
#include <test.h>
#include <module2.h>

int main() {
	test_func();
#ifdef TEST_DEF
	std::cout << "TEST_DEF is defined.\n";
#endif
	module2();
}