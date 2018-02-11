#include <iostream>

#include "task_includes.hpp"

struct Hyde {
    // private:
    void operator()() {}
};
struct HydeE {
    // private:
    void operator()() {throw(2);}
};

int main()
{
    using namespace TaskManager;

    Task func{[] { return false; }};

    Task tmp{[] { std::cout << "lambda expression" << std::endl; }};

    // clang-format off
    auto&& tmp_if = If([] { return 2 > 3; })(
                        [] { std::cout << "impossible" << std::endl; }
                    )->ElseIf([] { return false; })(

                    );
    // clang-format on

    auto count = 0;

    // clang-format off
    TaskSet set{
        Hyde{},
        Do(
            [] { std::cout << "once called" << std::endl; }
        )->Until([] { return true; }),
        tmp_if->copy()->ElseIf([] { return 123 > 234; })(
            [] { std::cout << 1 * 2 * 3 * 4 * 5 << std::endl; }
        )->Else(
            Delay{1},
            [] { std::cout << "else expression" << std::endl; }
        ),
        [] {},
        tmp,
        Wait([count]() mutable { std::cout << ++count << std::endl; return count > 3; }),
        While([] { return true; })(
            [count]() mutable { ++count; std::cout << "endless!" << count << std::endl; }
        ),
        tmp_if
    };
    // clang-format on

    TaskSet set2 = set;

    set.evaluate();
    set.evaluate();
    set.evaluate();
    set.evaluate();
    set.evaluate();

    set2.evaluate();
    set2.evaluate();
    set2.evaluate();
    set2.evaluate();
    set2.evaluate();

    set2.interrupt_func = HydeE{};
    set2.force_quit();

    std::cout << 1 << std::endl;

    return 0;
}
