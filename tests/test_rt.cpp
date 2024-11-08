#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "range_tree.h"

template <typename T>
struct minimal {
    T operator()(const T& lhs, const T& rhs) {
        return std::min(lhs, rhs);
    }
};

TEST(TestRangeTree, TestEmpty) {
    std::vector<int> vec {};
    rt::range_tree<int, std::plus<int>> tree(vec);
    std::vector<int> expected = {};
    EXPECT_EQ(expected, tree.as_vector());
}

TEST(TestRangeTree, TestOne) {
    std::vector<int> vec{1};
    rt::range_tree<int, std::plus<int>> tree(vec);
    std::vector<int> expected = {1};
    EXPECT_EQ(expected, tree.as_vector());
}

TEST(TestRangeTree, TestNotPowerOf2) {
    std::vector<int> vec{1, 2, 3, 4, 5, 6};
    rt::range_tree<int, std::plus<int>> tree(vec);
    std::vector<int> expected = {21, 10, 11, 3, 7, 11, 0, 1, 2, 3, 4, 5, 6, 0, 0};
    EXPECT_EQ(expected, tree.as_vector());
}

TEST(TestRangeTree, TestPowerOf2) {
    std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8};
    rt::range_tree<int, std::plus<int>> tree(vec);
    std::vector<int> expected = { 36, 10, 26, 3, 7, 11, 15, 1, 2, 3, 4, 5, 6, 7, 8 };
    EXPECT_EQ(expected, tree.as_vector());
}

TEST(TestRangeTree, TestSet) {
    std::vector<int> vec{1, 2, 3, 4, 5, 6};
    rt::range_tree<int, std::plus<int>> tree(vec);
    tree.set(0, 7);
    std::vector<int> expected = {27, 16, 11, 9, 7, 11, 0, 7, 2, 3, 4, 5, 6, 0, 0};
    EXPECT_EQ(expected, tree.as_vector());
}

TEST(TestRangeTree, TestMinimal) {
    std::vector<int> vec{-1, 10, 3, 9, 0, 99, -5};
    constexpr int inf = std::numeric_limits<int>::max();
    rt::range_tree<int, minimal<int>, inf> tree(vec);
    std::vector<int> expected = {-5, -1, -5, -1, 3, 0, -5, -1, 10, 3, 9, 0, 99, -5, 2147483647};
    EXPECT_EQ(expected, tree.as_vector());
}