#include <iostream>
#include "version.h"
#include "cache.h"

int main() {
	test_version();
	cache_tests();
	std::cout << "\x1B[32mTests complete.\033[0m\n";
}