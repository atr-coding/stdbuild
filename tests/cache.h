#pragma once
#include "test.h"
#include "../include/stdbuild.h"
// #include <cassert>

using namespace stdbuild;

void find_differences_test_no_changes() {
	const string_list a = { "test1", "test2", "test3" };
	const string_list b = { "test1", "test2", "test3" };
	string_list added, removed;
	__cache::find_differences(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(added.get()), std::back_inserter(removed.get()));

	test(added.size() == 0, "Elements were added when there shouldn't have been any added or removed.").on_fail([&]() {
		std::cerr << "  Added: " << added.string(' ') << '\n';
		});

	test(removed.size() == 0, "Elements were removed when there shouldn't have been any added or removed.").on_fail([&]() {
		std::cerr << "  Removed: " << removed.string(' ') << '\n';
		});
}

void find_differences_test_add() {
	const string_list a = { "test1", "test2", "test3", "test4" };
	const string_list b = { "test1", "test2", "test3" };
	string_list added, removed;
	__cache::find_differences(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(added.get()), std::back_inserter(removed.get()));

	test(added.size() == 1 && added.get().at(0) == "test4", "Element was not added when it should have been.").on_fail([&]() {
		std::cerr << "  Expected result:\t{ test4 }\n  Actual result:\t{ " << added.string(' ') << " }\n";
		});

	test(removed.size() == 0, "Elements were removed when they shouldn't have been.").on_fail([&]() {
		std::cerr << "  Removed: " << removed.string(' ') << '\n';
		});
}

void find_differences_test_remove() {
	const string_list a = { "test1", "test2" };
	const string_list b = { "test1", "test2", "test3" };
	string_list added, removed;
	__cache::find_differences(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(added.get()), std::back_inserter(removed.get()));

	test(added.size() == 0, "Elements were added when there shouldn't have been.").on_fail([&]() {
		std::cerr << "  Added: " << added.string(' ') << '\n';
		});

	test(removed.size() == 1 && removed.get().at(0) == "test3", "Element was not removed when it should have been.").on_fail([&]() {
		std::cerr << "  Expected result:\t{ test3 }\n  Actual result:\t{ " << removed.string(' ') << " }\n";
		});
}

void cache_tests() {
	// Find Differences tests
	find_differences_test_no_changes();
	find_differences_test_add();
	find_differences_test_remove();
}