#include <cstring>  // strcmp
#include <iostream> // std::cout
#include <string>   // std::string
#include <vector>   // std::vector

#include "expected.hpp"

#define RUN_TEST(test) test ? printf("OK\n") : printf("FAILED:  " #test "\n");

auto TestCtors() -> void;
auto TestHasValue() -> void;
auto TestOperatorBool() -> void;
auto TestValue() -> void;
auto TestError() -> void;
auto TestSwap() -> void;

int main() {
    TestCtors();
    TestHasValue();
    TestOperatorBool();
    TestValue();
    TestError();
    TestSwap();
    return 0;
}

auto TestCtors() -> void {
    expected<int, std::string> e1;
    std::string str("exception");
    expected<int, std::string> e2(str);
    expected<int, std::string> e3(10);
    expected<std::vector<int>, std::exception> e4(5, 1);
    expected<char, double> e5('a');
    const char c = 'a';
    expected<char, double> e6(c);
    expected<int, std::exception> e7(std::runtime_error("bad"));
    expected<int, std::exception> e8(e7);
    expected<int, std::exception> e9(std::move(e7));
    expected<int, std::exception> e10;
    e10 = e9;
    e10 = std::move(e8);
    expected<double, double> e11(2.0);
    expected<double, double> e12(unexpected(2.0));
}

auto TestHasValue() -> void {
    expected<int, std::string> e1;
    RUN_TEST(e1.has_value());
    std::string str("exception");
    expected<int, std::string> e2(str);
    RUN_TEST(!e2.has_value());
    expected<int, std::string> e3(10);
    RUN_TEST(e3.has_value());
    expected<std::vector<int>, std::exception> e4(5, 1);
    RUN_TEST(e4.has_value());
    expected<char, double> e5('a');
    RUN_TEST(e5.has_value());
    const char c = 'a';
    expected<char, double> e6(c);
    RUN_TEST(e6.has_value());
    expected<int, std::exception> e7(std::runtime_error("bad"));
    RUN_TEST(!e7.has_value());
    expected<int, std::exception> e8(e7);
    RUN_TEST(!e8.has_value());
    expected<int, std::exception> e9(std::move(e7));
    RUN_TEST(!e9.has_value());
    expected<int, std::exception> e10;
    RUN_TEST(e10.has_value());
    e10 = e9;
    RUN_TEST(!e10.has_value());
    e10 = std::move(e8);
    RUN_TEST(!e10.has_value());
    expected<double, double> e11(2.0);
    RUN_TEST(e11.has_value());
    expected<double, double> e12(unexpected(2.0));
    RUN_TEST(!e12.has_value());
}

auto TestOperatorBool() -> void {
    expected<int, std::string> e1;
    RUN_TEST(bool(e1));
    std::string str("exception");
    expected<int, std::string> e2(str);
    RUN_TEST(!bool(e2));
    expected<int, std::string> e3(10);
    RUN_TEST(bool(e3));
    expected<std::vector<int>, std::exception> e4(5, 1);
    RUN_TEST(bool(e4));
    expected<char, double> e5('a');
    RUN_TEST(bool(e5));
    const char c = 'a';
    expected<char, double> e6(c);
    RUN_TEST(bool(e6));
    expected<int, std::exception> e7(std::runtime_error("bad"));
    RUN_TEST(!bool(e7));
    expected<int, std::exception> e8(e7);
    RUN_TEST(!bool(e8));
    expected<int, std::exception> e9(std::move(e7));
    RUN_TEST(!bool(e9));
    expected<int, std::exception> e10;
    RUN_TEST(bool(e10));
    e10 = e9;
    RUN_TEST(!bool(e10));
    e10 = std::move(e8);
    RUN_TEST(!bool(e10));
    expected<double, double> e11(2.0);
    RUN_TEST(bool(e11));
    expected<double, double> e12(unexpected(2.0));
    RUN_TEST(!bool(e12));

    // bool b1 = e11; // should not compile
    bool b2(e11);
}

auto TestValue() -> void {
    expected<int, std::exception> e1(std::runtime_error("bad"));
    try {
        e1.value();
    } catch (std::exception) { std::cout << "OK\n"; }
    try {
        expected<int, double>(2.0).value();
    } catch (double) { std::cout << "OK\n"; }

    expected<int, std::string> e2(10);
    RUN_TEST(e2.value() == 10);
    e2.value() = 5;
    RUN_TEST(e2.value() == 5);

    const expected<std::string, int> e3("ok");
    RUN_TEST(e3.value() == "ok");
    // e3.value() = "fail"; // should not compile

    RUN_TEST((expected<int, std::string>(10).value()) == 10);
    // expected<int, std::string>(10).value() = 5; // should not compile

    const expected<int, std::string> e4(10);
    RUN_TEST(std::move(e4).value() == 10);
    const expected<int, std::string> e5(e4);
    // std::move(e5).value() = 5; // should not compile
}

auto TestError() -> void {
    expected<int, std::runtime_error> e1(std::runtime_error("bad"));
    RUN_TEST(0 == strcmp(e1.error().what(), "bad"));
    e1.error() = std::runtime_error("very bad");
    RUN_TEST(0 == strcmp(e1.error().what(), "very bad"));

    const expected<int, std::exception> e2 = e1;
    RUN_TEST(0 == strcmp(e2.error().what(), "std::exception"));
    // e2.error() = std::runtime_error("very very bad"); // should not compile

    std::string s("?");
    RUN_TEST((expected<int, std::string>(unexpected<std::string>("no")).error()) == "no");
    expected<int, std::string>(unexpected<std::string>("no")).error() = s; // should not compile

    const expected<int, std::string> e3(unexpected<std::string>("big no"));
    RUN_TEST(std::move(e3).error() == "big no");
    const expected<int, std::string> e4(e3);
    // std::move(e4).error() = s; // should not compile
}

auto TestSwap() -> void {
    expected<int, std::exception> e1(unexpected(std::runtime_error("bye")));
    RUN_TEST(!e1.has_value());
    expected<int, std::exception> e2(2);
    RUN_TEST(e2.has_value());
    e1.swap(e2);
    RUN_TEST(e1.has_value());
    RUN_TEST(!e2.has_value());
}