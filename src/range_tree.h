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

// @param T is an element of a semi-group with a binary operation Op
// @param Func is functional object that supports an operation
// on a given range (i.e. max, min or sum on a range)
template <typename T = int, typename Op = std::plus<T>, T Neutral = 0>
class range_tree {
private:
    std::vector<T> tree;
    std::vector<T> vec_;

    void build_tree(size_t start, size_t end);
    void set_recursive(size_t dest, T val, size_t cur, size_t left, size_t right);

public:
    range_tree(const std::vector<T>& vec);
    const std::vector<T>& as_vector() const;
    void set(size_t dest, T val);
    T sum();
};

template <typename T, typename Op, T Neutral>
range_tree<T, Op, Neutral>::range_tree(const std::vector<T>& vec)
    : vec_(vec), tree(std::vector<T>()) {
    if (vec.size() == 0) return;

    size_t n = 0;
    while (std::pow(2, n) < vec.size()) n++;
    tree = std::vector<T>(std::pow(2, n + 1) - 1, Neutral);

    std::vector<int> temp = vec;
    temp.resize(std::pow(2, n), Neutral);
    std::reverse(temp.begin(), temp.end());
    std::copy(temp.begin(), temp.end(), tree.begin());

    size_t start_pos = 0;
    build_tree(start_pos, std::pow(2, n));
}

template <typename T, typename Op, T Neutral>
void range_tree<T, Op, Neutral>::build_tree(size_t start, size_t end) {
    size_t nodes_num = end - start;
    if (nodes_num == 1) {
        std::reverse(tree.begin(), tree.end());
        return;
    }

    for (size_t i = 0; i < nodes_num / 2; ++i) {
        tree[end + i] = Op()(tree[start + 2 * i], tree[start + 2 * i + 1]);
    }

    build_tree(end, end + nodes_num / 2);
}

template <typename T, typename Op, T Neutral>
const std::vector<T>& range_tree<T, Op, Neutral>::as_vector() const {
    return tree;
}

template <typename T, typename Op, T Neutral>
void range_tree<T, Op, Neutral>::set(size_t dest, T val) {
    size_t vec_size = (tree.size() + 1) / 2;
    if (dest >= vec_size) throw std::out_of_range("Set index out of range");

    size_t cur_node = 0;
    size_t left = 0, right = vec_size;  // right exluded
    set_recursive(dest, val, cur_node, left, right);
}

template <typename T, typename Op, T Neutral>
void range_tree<T, Op, Neutral>::set_recursive(size_t dest, T val, size_t cur, size_t left,
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

    tree[cur] = Op()(tree[l_child], tree[r_child]);
}

}  // namespace rt
