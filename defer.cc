#include "print.hh"
#include <iostream>
#include <functional>
#include <memory>

struct defer_impl_cls {
    template <typename C, typename ... A>
    explicit defer_impl_cls(C&& c, A&&...a) {
        // static_assert(std::is_nothrow_invocable_v<C, A...>); // 要求函数 不抛出异常
        using Tuple = std::tuple<std::decay_t<C>, std::decay_t<A>...>;
        // using Function = decltype(c(a...));
        auto t = std::make_unique<Tuple>(c, std::forward<A>(a)...); // (new Tuple(c, std::forward<A>(a)...));
        // Function func = a
        f = [t=t.release()]() {
            std::unique_ptr<Tuple> e(t);
            [&]<size_t ... v>(std::index_sequence<v...>) {
                std::invoke(get<v>(*e)...);
            } (std::make_index_sequence<sizeof...(A)+1>());
        };
    }
    ~defer_impl_cls() noexcept { f(); }
    defer_impl_cls(const defer_impl_cls&) = delete;
    defer_impl_cls(defer_impl_cls&&) = delete;
    std::function<void()> f;
};
#define CALL(x, ...) [&]<typename...A>(A&&...a) {\
	return __VA_OPT__(__VA_ARGS__ ->) x(std::forward<A>(a)...);\
} 
// 根据行号生成一个名称
#define STRINGIFY(x) DEFER_X##x
#define TOSTRING(x) STRINGIFY(x)
#define UNIQUE_NAME_FOR_LINE TOSTRING(__LINE__)
#define DEFER(...) __VA_OPT__(defer_impl_cls UNIQUE_NAME_FOR_LINE {__VA_ARGS__});
// ------------------ TEST ---------------------
struct A {
    void foo(int t) {
        std::println("A::foo(t: {})", t);
    }
    template <typename T>
    void fun(T&& t) {
        std::println("A::fun(t: {})", t);
    }
    A() {
        DEFER([]() {
            std::println("----------- A::A(end) -----------");
        }) // 最先写的最后调用
        defer_impl_cls d_1(&A::foo, this, 1);
        DEFER([this]() { foo(2); })
        defer_impl_cls d_2(CALL(fun, this), 'C');
        std::println("----------- A::A(beg) -----------");
    }
};
void f(int a) {
    std::println("f(a: {})", a);
}
template <typename T>
void g(T&& t) {
    std::println("g(t: {})", t);
}

int main() try {
    DEFER(f, 1) // 调用函数 f(1) 且*后调用
    DEFER(CALL(g), "234...") // 调用函数 f(2) 且先调用此函数
    DEFER() // 什么都不写 OK
    defer_impl_cls NAME_HHHH(f, 3); // 自己起个名字O
    A a;
    // DEFER(CALL(println), "hello world");
    return 0;
}
catch (std::exception&e) {
    std::println("{}", e.what());
}