#include <algorithm>
#include <cmath>
#include <concepts>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace rt {

// template <template<typename T> typename Func>
// concept Callable = requires(T lhs, T rhs) {
//     std::is_same<decltype(Func(lhs, rhs)), T>();
// };

struct interval {
    size_t start_;
    size_t end_;
};

// @param T is an element of a monoid with a binary operation
// @param Op is a functional object that supports an operation
// on a given range (i.e. max, min or sum on a range)
// @param Id is an identity element of a monoid
template <typename T = int, typename Op = std::plus<T>, T Id = 0>
class range_tree {
private:
    std::vector<T> tree;
    size_t vec_size_;
    Op op_;

    void build_tree(size_t start, size_t end);
    void set_recursive(size_t dest, T val, size_t cur, size_t left, size_t right);
    T accumulate_recursive(size_t cur_node, size_t cur_left, size_t cur_right, size_t dest_left,
                           size_t dest_right);

public:
    range_tree(const std::vector<T>& vec);
    const std::vector<T>& as_vector() const;
    void set(size_t dest, T val);
    T accumulate(size_t left, size_t right);
};

template <typename T, typename Op, T Id>
range_tree<T, Op, Id>::range_tree(const std::vector<T>& vec)
    : tree(std::vector<T>()), op_(Op()) {
    if (vec.size() == 0) return;

    size_t n = 0;
    while (std::pow(2, n) < vec.size()) n++;
    tree = std::vector<T>(std::pow(2, n + 1) - 1, Id);
    vec_size_ = std::pow(2, n);

    std::vector<int> temp = vec;
    temp.resize(vec_size_, Id);
    std::reverse(temp.begin(), temp.end());
    std::copy(temp.begin(), temp.end(), tree.begin());

    size_t start_pos = 0;
    build_tree(start_pos, vec_size_);
}

template <typename T, typename Op, T Id>
void range_tree<T, Op, Id>::build_tree(size_t start, size_t end) {
    size_t nodes_num = end - start;
    if (nodes_num == 1) {
        std::reverse(tree.begin(), tree.end());
        return;
    }

    for (size_t i = 0; i < nodes_num / 2; ++i) {
        tree[end + i] = op_(tree[start + 2 * i], tree[start + 2 * i + 1]);
    }

    build_tree(end, end + nodes_num / 2);
}

template <typename T, typename Op, T Id>
const std::vector<T>& range_tree<T, Op, Id>::as_vector() const {
    return tree;
}

template <typename T, typename Op, T Id>
void range_tree<T, Op, Id>::set(size_t dest, T val) {
    if (dest >= vec_size_) throw std::out_of_range("Set index out of range");

    size_t cur_node = 0;
    size_t left = 0, right = vec_size_;  // right exluded
    set_recursive(dest, val, cur_node, left, right);
}

template <typename T, typename Op, T Id>
void range_tree<T, Op, Id>::set_recursive(size_t dest, T val, size_t cur, size_t left,
                                               size_t right) {
    if (left == right - 1) {
        tree[cur] = val;
        return;
    }

    size_t mid = (left + right) / 2;
    size_t l_child = 2 * cur + 1, r_child = 2 * cur + 2;
    if (dest < mid)
        set_recursive(dest, val, l_child, left, mid);
    else
        set_recursive(dest, val, r_child, mid, right);

    tree[cur] = op_(tree[l_child], tree[r_child]);
}

template <typename T, typename Op, T Id>
T range_tree<T, Op, Id>::accumulate(size_t left, size_t right) {
    size_t cur_node = 0, cur_left = 0, cur_right = vec_size_;  // right excluded
    return accumulate_recursive(cur_node, cur_left, cur_right, left, right + 1);
}

template <typename T, typename Op, T Id>
T range_tree<T, Op, Id>::accumulate_recursive(size_t cur_node, size_t cur_left,
                                                   size_t cur_right, size_t dest_left,
                                                   size_t dest_right) {
    if (dest_right <= cur_left or cur_right <= dest_left) return Id;
    if (dest_left <= cur_left and cur_right <= dest_right) return tree[cur_node];
    size_t mid = (cur_left + cur_right) / 2;
    T res_left = accumulate_recursive(2 * cur_node + 1, cur_left, mid, dest_left, dest_right);
    T res_right = accumulate_recursive(2 * cur_node + 2, mid, cur_right, dest_left, dest_right);
    return op_(res_left, res_right);
}

}  // namespace rt
