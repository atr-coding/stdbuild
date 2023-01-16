#pragma once
#include "../include/stdbuild.h"
#include <cassert>

using namespace stdbuild;

void test_version() {
	const version zero{ 0, 0, 0 };

	assert(version(1, 0, 0) > zero);
	assert(version(0, 1, 0) > zero);
	assert(version(0, 0, 1) > zero);
	assert(version(1, 1, 1) > zero);

	assert(zero >= zero);
	assert(version(1, 0, 0) >= zero);
	assert(version(0, 1, 0) >= zero);
	assert(version(0, 0, 1) >= zero);
	assert(version(1, 1, 1) >= zero);

	assert(zero < version(1, 0, 0));
	assert(zero < version(0, 1, 0));
	assert(zero < version(0, 0, 1));
	assert(zero < version(1, 1, 1));

	assert(zero <= version(1, 0, 0));
	assert(zero <= version(0, 1, 0));
	assert(zero <= version(0, 0, 1));
	assert(zero <= version(1, 1, 1));

	assert(zero <= zero);
	assert(zero <= version(1, 0, 0));
	assert(zero <= version(0, 1, 0));
	assert(zero <= version(0, 0, 1));
	assert(zero <= version(1, 1, 1));

	assert(zero == zero);
	assert(zero != version(1, 0, 0));

	assert(version(3, 89, 10).string() == "3.89.10");
}