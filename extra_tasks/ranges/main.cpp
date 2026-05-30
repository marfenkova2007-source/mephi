#include "task.h"

#define DEF(x) (defined(_GLIBCXX_##x) || defined(_LIBCPP_##x))

#if DEF(VECTOR) || DEF(DEQUE) || DEF(LIST) || DEF(FORWARD_LIST) || DEF(RANGES)
#error "inclusion of vector, deque, list or ranges is prohibited"
#else
#include <deque>
#include <forward_list>
#include <list>
#include <ranges>
#include <vector>
#endif

#undef DEF

#include <cassert>
#include <cstdint>
#include <functional>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#define ASSERT_THROW(expr, exception) \
do {                                  \
    try {                             \
        expr;                         \
        assert(!"no exception");      \
    } catch (const exception&) {      \
    } catch (...) {                   \
        assert(!"other exception");   \
    }                                 \
} while (false)

template <typename View, typename Sequence>
void CheckImpl(const View& view, const Sequence& expected) {
    auto view_it = view.begin();
    auto expected_it = expected.begin();

    const auto view_end = view.end();
    const auto expected_end = expected.end();
    while (view_it != view_end && expected_it != expected_end) {
        assert(*view_it++ == *expected_it++);
    }

    assert(view_it == view_end);
    assert(expected_it == expected_end);
}

template <typename View, typename T>
void Check(const View& view, const std::initializer_list<T>& expected) {
    CheckImpl(view, expected);
}

template <typename View, typename Sequence>
void Check(const View& view, const Sequence& expected) {
    CheckImpl(view, expected);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SimpleTest() {
    {
        Check(ranges::Empty<int>(), std::vector<int>{});
    }

    {
        std::vector<int> vec = {1, -1, 1, 2, -2, -3, 0, 3, -5, 5, 4};

        auto positive = [](int x) { return x > 0; };
        Check(ranges::View(vec) | ranges::Filter(positive), {1, 1, 2, 3, 5, 4});

        int value = 3;
        auto lt3 = [value](int x) { return x < value; };
        Check(ranges::View(vec) | ranges::Filter(lt3) | ranges::Filter(positive), {1, 1, 2});

        Check(ranges::View(vec) | ranges::First(5), {1, -1, 1, 2, -2});
        Check(ranges::View(vec) | ranges::First(5) | ranges::Filter(positive), {1, 1, 2});

        std::set<int> set = ranges::View(vec) | ranges::Filter(positive) | ranges::To<std::set<int>>();
        Check(ranges::View(set), {1, 2, 3, 4, 5});
    }

    {
        std::map<std::string, std::vector<int>> map = {
            {"abc", {1, 2, 3}},
            {"xyz", {1, -1, 0, 100, 1}},
            {"def", {-6, -5, -4, -3}},
            {"a", {}},
            {"bbb", {5}},
        };

        auto big = [](const std::pair<const std::string, std::vector<int>>& x) {
            return x.second.size() > 3;
        };
        auto key = [](const std::pair<const std::string, std::vector<int>>& x) {
            return x.first;
        };
        Check(ranges::View(map) | ranges::Filter(big) | ranges::First(2) | ranges::Map(key), {"def", "xyz"});
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FirstTest() {
    std::vector<std::string> vec = {"ab", "bc", "de", "ef", "gh"};
    const ranges::View view(vec);

    Check(view | ranges::First(1), {"ab"});
    Check(view | ranges::First(0), std::vector<std::string>{});
    Check(view | ranges::First(10), vec);
    Check(view | ranges::First(1'000'000'000'000'000LL), vec);

    auto first4 = view | ranges::First(4);
    Check(first4 | ranges::First(0) | ranges::First(100), std::vector<std::string>{});
    Check(first4 | ranges::First(2) | ranges::First(1), {"ab"});

    Check(vec, {"ab", "bc", "de", "ef", "gh"});
    Check(view, vec);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FilterTest() {
    std::vector<std::string> vec = {"abc", "b", "defghi", "ef", "xyz"};
    const ranges::View view(vec);

    auto lt = [](int lim, std::string_view s) {
        return static_cast<int>(s.size()) < lim;
    };
    Check(view | ranges::Filter(std::bind_front(lt, 3)), {"b", "ef"});
    Check(view | ranges::Filter(std::bind_front(lt, 10)), vec);
    Check(view | ranges::Filter(std::bind_front(lt, 1)), std::vector<std::string>{});

    auto eq = [](std::string_view sample, std::string_view s) {
        return sample == s;
    };
    auto lt5 = view | ranges::Filter(std::bind_front(lt, 5));
    Check(lt5 | ranges::Filter(std::bind_front(eq, "ef")), {"ef"});
    Check(
        lt5 | ranges::Filter(std::bind_front(lt, 1)) | ranges::Filter(std::bind_front(eq, "x")),
        std::vector<std::string>{}
    );
    Check(ranges::Empty<std::string>() | ranges::Filter(std::bind_front(eq, "ef")), std::vector<std::string>{});
    Check(lt5 | ranges::Filter(std::bind_front(eq, "x")) | ranges::First(5), std::vector<std::string>{});
    Check(lt5 | ranges::First(3) | ranges::Filter(std::bind_front(lt, 3)), {"b", "ef"});

    Check(vec, {"abc", "b", "defghi", "ef", "xyz"});
    Check(view, vec);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MapTest() {
    auto to_uint = [](int x) -> uint32_t {
        if (x < 0) {
            throw std::invalid_argument(std::to_string(x));
        }
        return x;
    };

    auto to_str = [](int x) {
        return '\'' + std::to_string(x) + '\'';
    };

    {
        std::vector<int> vec = {1, -1, 0, -2, 2};

        Check(ranges::View(vec) | ranges::Map(to_str) | ranges::First(3), {"'1'", "'-1'", "'0'"});

        Check(
            ranges::View(vec) | ranges::SafeMap(to_uint) | ranges::First(4),
            {std::optional<uint32_t>(1), std::optional<uint32_t>(), std::optional<uint32_t>(0), std::optional<uint32_t>()}
        );
    }

    {
        std::vector<std::pair<std::string, int>> vec = {
            {"abc", 3},
            {"d", 1},
            {"-oprst", -5},
            {"10102", 0},
            {"qwerty", 5},
            {"-5", -2},
        };

        auto get_first = [](const std::pair<std::string, int>& x) { return x.first; };
        auto get_second = [](const std::pair<std::string, int>& x) { return x.second; };

        auto second = ranges::View(vec) | ranges::Map(get_second);
        Check(second, {3, 1, -5, 0, 5, -2});
        Check(second | ranges::Map(to_str) | ranges::First(3), {"'3'", "'1'", "'-5'"});

        Check(ranges::Empty<std::pair<std::string, int>>() | ranges::Map(get_second), std::vector<int>{});

        auto uint = second | ranges::Map(to_uint) | ranges::First(3);
        Check(uint | ranges::First(2), {3u, 1u});
        ASSERT_THROW(Check(uint | ranges::First(3), {3u, 1u, 0u}), std::invalid_argument);
        Check(
            second | ranges::SafeMap(to_uint) | ranges::First(3),
            {std::optional<uint32_t>(3), std::optional<uint32_t>(1), std::optional<uint32_t>()}
        );

        Check(ranges::View(vec)
            | ranges::Map(get_first)
            | ranges::SafeMap([](const std::string& s) { return std::stoi(s); })
            | ranges::Unwrap()
            | ranges::To<std::vector<int>>(),
            {10102, -5}
        );

        assert(vec.size() == 6);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct NonCopyable {
    int Value;

    NonCopyable(int value)
        : Value(value)
    { }

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

bool operator==(NonCopyable lhs, NonCopyable rhs) {
    return lhs.Value == rhs.Value;
}

void NonCopyableTest() {
    std::vector<NonCopyable> non_copy;
    non_copy.reserve(10);
    for (int i = 0; i < 10; ++i) {
        non_copy.emplace_back(i);
    }

    Check(
        ranges::View(static_cast<const std::vector<NonCopyable>&>(non_copy))
        | ranges::Filter([](const NonCopyable& x) { return x.Value & 1; })
        | ranges::First(10)
        | ranges::Map([](const NonCopyable& x) { return x.Value; }),
        {1, 3, 5, 7, 9}
    );

    assert(non_copy.size() == 10);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

class CountingCopies {
public:
    explicit CountingCopies(size_t* counter = nullptr)
        : Counter_(counter)
    {
        Inc();
    }

    CountingCopies(const CountingCopies& other)
        : CountingCopies(other.Counter_)
    { }

    CountingCopies& operator=(const CountingCopies& other) {
        Counter_ = other.Counter_;
        Inc();
        return *this;
    }

private:
    void Inc() {
        if (Counter_ != nullptr) {
            ++*Counter_;
        }
    }

private:
    size_t* Counter_;
};

struct SqrSize : public CountingCopies {
    using CountingCopies::CountingCopies;

    size_t operator()(std::string_view s) const {
        return s.size() * s.size();
    }
};

struct Even : public CountingCopies {
    using CountingCopies::CountingCopies;

    bool operator()(size_t x) const {
        return (x & 1) == 0;
    }
};

void CopyFunctorTest() {
    {
        std::vector<std::string> vec = {"abc", "b", "defghi", "ef", "xyz", "", "1234"};

        auto counters = [](
            const std::vector<std::string>& vec,
            const std::vector<size_t>& sqrs,
            const std::vector<size_t>& evens
        ) {
            size_t sqr_counter = 0;

            auto sqr_sizes = ranges::View(vec) | ranges::Map(SqrSize(&sqr_counter));

            const size_t init_sqr_counter = sqr_counter;
            Check(sqr_sizes, sqrs);
            Check(sqr_sizes, sqrs);
            assert(init_sqr_counter == sqr_counter);

            size_t even_counter = 0;
            Check(sqr_sizes | ranges::Filter(Even(&even_counter)) | ranges::First(50), evens);

            return std::make_pair(sqr_counter, even_counter);
        };

        auto [sqr1, even1] = counters(
            {"abc", "b", "defghi", "ef", "xyz", "", "1234"},
            {9u, 1u, 36u, 4u, 9u, 0u, 16u},
            {36u, 4u, 0u, 16u}
        );

        auto [sqr2, even2] = counters(
            {"abc", "b", "defghi", "ef", "xyz", "", "1234", "eeee", "fffff", "ggg", "", "0123", "h"},
            {9u, 1u, 36u, 4u, 9u, 0u, 16u, 16u, 25u, 9u, 0u, 16u, 1u},
            {36u, 4u, 0u, 16u, 16u, 0u, 16u}
        );

        assert(sqr1 == sqr2);
        assert(even1 == even2);
    }

    {
        std::vector<std::string> vec = {"abc", "b", "defghi", "ef", "xyz", "", "1234"};

        auto make_view = [](const std::vector<std::string>& vec) {
            SqrSize mapper;
            Even filter;
            return ranges::View(vec) | ranges::Map(mapper) | ranges::First(5) | ranges::Filter(filter);
        };

        auto view = make_view(vec);
        Check(view, {36u, 4u});
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Container {
    std::vector<int> Vector;
    mutable int64_t IterationCount = 0;

    class Iterator : public std::vector<int>::const_iterator {
    public:
        using Base = std::vector<int>::const_iterator;

    public:
        Iterator(Base b, int64_t* counter, bool copy = false)
            : Base(std::move(b))
            , Counter_(counter)
        {
            if (!copy) {
                Inc();
            }
        }

        const int& operator*() const {
            return Base::operator*();
        }

        Iterator& operator++() {
            Base::operator++();
            Inc();
            return *this;
        }

        Iterator operator++(int) {
            Inc();
            return Iterator(Base::operator++(0), Counter_, true);
        }

        bool operator==(const Iterator& other) const {
            return static_cast<const Base&>(*this) == static_cast<const Base&>(other);
        }

    private:
        void Inc() {
            if (Counter_ != nullptr) {
                ++*Counter_;
            }
        }

    private:
        int64_t* Counter_ = nullptr;
    };

    auto begin() const {
        return Iterator(Vector.begin(), &IterationCount);
    }

    auto end() const {
        return Iterator(Vector.end(), nullptr);
    }
};

void StressTest() {
    constexpr int kLen = 1'000'000;
    constexpr int kLimit = 500;

    Container cont;
    cont.Vector.resize(kLen);
    std::iota(cont.Vector.begin(), cont.Vector.end(), 0);

    auto view = ranges::View(cont)
        | ranges::First(kLen - 1)
        | ranges::Filter([](int x) { return x < kLen - 1; })
        | ranges::Map([](int x) { return std::min(kLen - 1, x); })
        | ranges::First(kLen - 10)
        | ranges::Filter([](int x) { return x < kLen - 10; })
        | ranges::Map([](int x) { return std::min(kLen - 10, x); })
        | ranges::First(kLen - 100)
        | ranges::Filter([](int x) { return x < kLen - 100; })
        | ranges::Map([](int x) { return std::min(kLen - 100, x); })
        | ranges::First(kLen - 1000)
        | ranges::Filter([](int x) { return x < kLen - 1000; })
        | ranges::Map([](int x) { return std::min(kLen - 1000, x); })
        | ranges::First(kLen - 10000)
        | ranges::Filter([](int x) { return x < kLen - 10000; })
        | ranges::Map([](int x) { return std::min(kLen - 10000, x); })
        | ranges::First(kLimit);

    std::vector<int> expected(kLimit);
    std::iota(expected.begin(), expected.end(), 0);

    for (int i = kLimit; i > 0; --i) {
        cont.IterationCount = 0;

        Check(view | ranges::First(i), expected);
        assert(cont.IterationCount == i);

        expected.pop_back();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    FirstTest();
    FilterTest();
    MapTest();
    SimpleTest();
    NonCopyableTest();
    CopyFunctorTest();
    StressTest();

    return 0;
}
