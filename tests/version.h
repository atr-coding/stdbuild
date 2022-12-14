#pragma once
#include "../stdbuild"
#include <cassert>

void test_version() {
	const std::build::version zero{ 0, 0, 0 };

	assert(std::build::version(1, 0, 0) > zero);
	assert(std::build::version(0, 1, 0) > zero);
	assert(std::build::version(0, 0, 1) > zero);
	assert(std::build::version(1, 1, 1) > zero);

	assert(zero >= zero);
	assert(std::build::version(1, 0, 0) >= zero);
	assert(std::build::version(0, 1, 0) >= zero);
	assert(std::build::version(0, 0, 1) >= zero);
	assert(std::build::version(1, 1, 1) >= zero);

	assert(zero < std::build::version(1, 0, 0));
	assert(zero < std::build::version(0, 1, 0));
	assert(zero < std::build::version(0, 0, 1));
	assert(zero < std::build::version(1, 1, 1));

	assert(zero <= std::build::version(1, 0, 0));
	assert(zero <= std::build::version(0, 1, 0));
	assert(zero <= std::build::version(0, 0, 1));
	assert(zero <= std::build::version(1, 1, 1));

	assert(zero <= zero);
	assert(zero <= std::build::version(1, 0, 0));
	assert(zero <= std::build::version(0, 1, 0));
	assert(zero <= std::build::version(0, 0, 1));
	assert(zero <= std::build::version(1, 1, 1));

	assert(zero == zero);
	assert(zero != std::build::version(1, 0, 0));

	assert(std::build::version(3, 89, 10).string() == "3.89.10");
}