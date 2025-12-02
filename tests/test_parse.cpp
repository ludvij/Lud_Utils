#include "catch2/catch_test_macros.hpp"
#include "ludutils/lud_parse.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("String Split", "[parse][strings]")
{
    SECTION("Simple split")
    {
        auto parts = Lud::Split("This is a test", " ");
        REQUIRE(parts.size() == 4);
        REQUIRE(parts[0] == "This");
        REQUIRE(parts[1] == "is");
        REQUIRE(parts[2] == "a");
        REQUIRE(parts[3] == "test");
    }

    SECTION("Empty split")
    {
        auto parts = Lud::Split("", " ");
        REQUIRE(parts.size() == 0);
    }

    SECTION("Empty delim")
    {
        auto parts = Lud::Split("This is a test", "");
        REQUIRE(parts.size() == 1);
        REQUIRE(parts[0] == "This is a test");
    }

    SECTION("string consists of delims")
    {
        auto parts = Lud::Split("                 ", " ");
        REQUIRE(parts.size() == 0);
    }

    SECTION("Multi-letter delim")
    {
        auto parts = Lud::Split("This is a test", "is");
        REQUIRE(parts.size() == 3);
        REQUIRE(parts[0] == "Th");
        REQUIRE(parts[1] == " ");
        REQUIRE(parts[2] == " a test");
    }

    SECTION("Split once")
    {
        auto parts = Lud::Split("This is a test", "is", 1);
        REQUIRE(parts.size() == 2);
        REQUIRE(parts[0] == "Th");
        REQUIRE(parts[1] == " is a test");
    }

    SECTION("Split equal amount to required")
    {
        auto parts = Lud::Split("This is a test", "is", 3);
        REQUIRE(parts.size() == 3);
        REQUIRE(parts[0] == "Th");
        REQUIRE(parts[1] == " ");
        REQUIRE(parts[2] == " a test");
    }

    SECTION("Split more than required")
    {
        auto parts = Lud::Split("This is a test", "is", 4);
        REQUIRE(parts.size() == 3);
        REQUIRE(parts[0] == "Th");
        REQUIRE(parts[1] == " ");
        REQUIRE(parts[2] == " a test");
    }

    SECTION("Delim is whole string")
    {
        auto parts = Lud::Split("This is a test", "This is a test");
        REQUIRE(parts.size() == 0);
    }

    SECTION("delim is char")
    {
        auto parts = Lud::Split("This is a test", ' ');
        REQUIRE(parts.size() == 4);
    }
}

TEST_CASE("String Join", "[parse][strings]")
{
    SECTION("Simple join")
    {
        auto full = Lud::Join(std::vector{"This", "is", "a", "test"}, " ");
        REQUIRE(full == "This is a test");
    }

    SECTION("Empty delim")
    {
        auto full = Lud::Join(std::vector{"This", "is", "a", "test"}, "");
        REQUIRE(full == "Thisisatest");
    }

    SECTION("Empty parts")
    {
        auto full = Lud::Join(std::vector<std::string>(), "");
        REQUIRE(full.empty());
    }

    SECTION("Iterator pair")
    {
        std::vector parts{"This", "is", "a", "test"};

        auto full = Lud::Join(parts.begin(), parts.end(), " ");
        REQUIRE(full == "This is a test");

        auto full2 = Lud::Join(parts.begin() + 1, parts.end(), " ");
        REQUIRE(full2 == "is a test");

        auto full3 = Lud::Join(parts.end(), parts.end(), " ");
        REQUIRE(full3 == "");
    }
}

TEST_CASE("Remove Prefix", "[parse][strings]")
{
    SECTION("Simple remove prefix")
    {
        std::string original = "This is a test";
        auto res = Lud::RemovePrefix(original, "This");

        REQUIRE(original == "This is a test");
        REQUIRE(res == " is a test");
    }

    SECTION("Simple remove prefix, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemovePrefix(original, "This");

        REQUIRE(original == " is a test");
    }

    SECTION("Prefix not found")
    {
        std::string original = "This is a test";
        auto res = Lud::RemovePrefix(original, "Not found");

        REQUIRE(original == "This is a test");
        REQUIRE(res == "This is a test");
    }

    SECTION("Prefix not found, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemovePrefix(original, "Not found");

        REQUIRE(original == "This is a test");
    }

    SECTION("Partial found")
    {
        std::string original = "This is a test";
        auto res = Lud::RemovePrefix(original, " This ");

        REQUIRE(original == "This is a test");
        REQUIRE(res == "This is a test");
    }

    SECTION("Partial found, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemovePrefix(original, " This ");

        REQUIRE(original == "This is a test");
    }

    SECTION("Found at the end")
    {
        std::string original = "This is a test";
        auto res = Lud::RemovePrefix(original, "test");

        REQUIRE(original == "This is a test");
        REQUIRE(res == "This is a test");
    }

    SECTION("Found at the end, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemovePrefix(original, "test");

        REQUIRE(original == "This is a test");
    }

    SECTION("Prefix repeated multiple times")
    {
        std::string original = "+++This is a test";
        auto res = Lud::RemovePrefix(original, "+");

        REQUIRE(original == "+++This is a test");
        REQUIRE(res == "++This is a test");
    }

    SECTION("Prefix repeated multiple times, inplace")
    {
        std::string original = "+++This is a test";
        Lud::inplace::RemovePrefix(original, "+");

        REQUIRE(original == "++This is a test");
    }
}

TEST_CASE("Remove Suffix", "[parse][strings]")
{
    SECTION("Simple remove Suffix")
    {
        std::string original = "This is a test";
        auto res = Lud::RemoveSuffix(original, "test");

        REQUIRE(original == "This is a test");
        REQUIRE(res == "This is a ");
    }

    SECTION("Simple remove prefix, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemoveSuffix(original, "test");

        REQUIRE(original == "This is a ");
    }

    SECTION("Suffix not found")
    {
        std::string original = "This is a test";
        auto res = Lud::RemovePrefix(original, "Not found");

        REQUIRE(original == "This is a test");
        REQUIRE(res == "This is a test");
    }

    SECTION("Sufix not found, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemovePrefix(original, "Not found");

        REQUIRE(original == "This is a test");
    }

    SECTION("Partial found")
    {
        std::string original = "This is a test";
        auto res = Lud::RemovePrefix(original, " test ");

        REQUIRE(original == "This is a test");
        REQUIRE(res == "This is a test");
    }

    SECTION("Partial found, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemovePrefix(original, " test ");

        REQUIRE(original == "This is a test");
    }

    SECTION("Found at the start")
    {
        std::string original = "This is a test";
        auto res = Lud::RemovePrefix(original, "this");

        REQUIRE(original == "This is a test");
        REQUIRE(res == "This is a test");
    }

    SECTION("Found at the end, inplace")
    {
        std::string original = "This is a test";
        Lud::inplace::RemovePrefix(original, "this");

        REQUIRE(original == "This is a test");
    }

    SECTION("Prefix repeated multiple times")
    {
        std::string original = "This is a test+++";
        auto res = Lud::RemoveSuffix(original, "+");

        REQUIRE(original == "This is a test+++");
        REQUIRE(res == "This is a test++");
    }

    SECTION("Prefix repeated multiple times, inplace")
    {
        std::string original = "This is a test+++";
        Lud::inplace::RemoveSuffix(original, "+");

        REQUIRE(original == "This is a test++");
    }
}

TEST_CASE("To upper", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string original = "this is a test";
        auto res = Lud::ToUpper(original);
        REQUIRE(original == "this is a test");
        REQUIRE(res == "THIS IS A TEST");
    }

    SECTION("Inplace")
    {
        std::string original = "this is a test";
        Lud::inplace::ToUpper(original);
        REQUIRE(original == "THIS IS A TEST");
    }
}

TEST_CASE("To lower", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string original = "THIS IS A TEST";
        auto res = Lud::ToLower(original);
        REQUIRE(original == "THIS IS A TEST");
        REQUIRE(res == "this is a test");
    }

    SECTION("Inplace")
    {
        std::string original = "THIS IS A TEST";
        Lud::inplace::ToLower(original);
        REQUIRE(original == "this is a test");
    }
}

TEST_CASE("To title", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string original = "this is a test";
        auto res = Lud::ToTitle(original);
        REQUIRE(original == "this is a test");
        REQUIRE(res == "This Is A Test");
    }

    SECTION("Inplace")
    {
        std::string original = "this is a test";
        Lud::inplace::ToTitle(original);
        REQUIRE(original == "This Is A Test");
    }
}

TEST_CASE("Capitalize", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string original = "this is a test";
        auto res = Lud::Capitalize(original);
        REQUIRE(original == "this is a test");
        REQUIRE(res == "This is a test");
    }

    SECTION("Inplace")
    {
        std::string original = "this is a test";
        Lud::inplace::Capitalize(original);
        REQUIRE(original == "This is a test");
    }
}

TEST_CASE("LStrip", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string tabs = "\tthis is a test  ";
        std::string spaces = "  this is a test  ";
        std::string newline = "\nthis is a test  ";
        std::string linefeed = "\rthis is a test  ";
        std::string combination = "\n\t \rthis is a test  ";

        auto res_tabs = Lud::LStrip(tabs);
        auto res_spaces = Lud::LStrip(spaces);
        auto res_newline = Lud::LStrip(newline);
        auto res_linefeed = Lud::LStrip(linefeed);
        auto res_combination = Lud::LStrip(combination);

        REQUIRE(tabs == "	this is a test  ");

        REQUIRE(res_tabs == "this is a test  ");
        REQUIRE(res_spaces == "this is a test  ");
        REQUIRE(res_newline == "this is a test  ");
        REQUIRE(res_linefeed == "this is a test  ");
        REQUIRE(res_combination == "this is a test  ");
    }

    SECTION("Inplace")
    {
        std::string tabs = "\tthis is a test  ";
        std::string spaces = "  this is a test  ";
        std::string newline = "\nthis is a test  ";
        std::string linefeed = "\rthis is a test  ";
        std::string combination = "\n\t \rthis is a test  ";

        Lud::inplace::LStrip(tabs);
        Lud::inplace::LStrip(spaces);
        Lud::inplace::LStrip(newline);
        Lud::inplace::LStrip(linefeed);
        Lud::inplace::LStrip(combination);

        REQUIRE(tabs == "this is a test  ");
        REQUIRE(spaces == "this is a test  ");
        REQUIRE(newline == "this is a test  ");
        REQUIRE(linefeed == "this is a test  ");
        REQUIRE(combination == "this is a test  ");
    }

    SECTION("blank string")
    {
        std::string original = "      ";
        auto res = Lud::LStrip(original);

        REQUIRE(res == "");
    }

    SECTION("blank string, inplace")
    {
        std::string original = "      ";
        Lud::inplace::LStrip(original);

        REQUIRE(original == "");
    }
}

TEST_CASE("RStrip", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string tabs = "  this is a test\t";
        std::string spaces = "  this is a test  ";
        std::string newline = "  this is a test\n";
        std::string linefeed = "  this is a test\r";
        std::string combination = "  this is a test\t \n\r";

        auto res_tabs = Lud::RStrip(tabs);
        auto res_spaces = Lud::RStrip(spaces);
        auto res_newline = Lud::RStrip(newline);
        auto res_linefeed = Lud::RStrip(linefeed);
        auto res_combination = Lud::RStrip(combination);

        REQUIRE(tabs == "  this is a test\t");

        REQUIRE(res_tabs == "  this is a test");
        REQUIRE(res_spaces == "  this is a test");
        REQUIRE(res_newline == "  this is a test");
        REQUIRE(res_linefeed == "  this is a test");
        REQUIRE(res_combination == "  this is a test");
    }

    SECTION("Inplace")
    {
        std::string tabs = "  this is a test\t";
        std::string spaces = "  this is a test  ";
        std::string newline = "  this is a test\n";
        std::string linefeed = "  this is a test\r";
        std::string combination = "  this is a test\t \n\r";

        Lud::inplace::RStrip(tabs);
        Lud::inplace::RStrip(spaces);
        Lud::inplace::RStrip(newline);
        Lud::inplace::RStrip(linefeed);
        Lud::inplace::RStrip(combination);

        REQUIRE(tabs == "  this is a test");
        REQUIRE(spaces == "  this is a test");
        REQUIRE(newline == "  this is a test");
        REQUIRE(linefeed == "  this is a test");
        REQUIRE(combination == "  this is a test");
    }

    SECTION("blank string")
    {
        std::string original = "      ";
        auto res = Lud::RStrip(original);

        REQUIRE(res == "");
    }

    SECTION("blank string, inplace")
    {
        std::string original = "      ";
        Lud::inplace::RStrip(original);

        REQUIRE(original == "");
    }
}

TEST_CASE("Strip", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string tabs = "\tthis is a test\t";
        std::string spaces = "  this is a test  ";
        std::string newline = "\nthis is a test\n";
        std::string linefeed = "\tthis is a test\r";
        std::string combination = "\r\nthis is a test\t  ";

        auto res_tabs = Lud::Strip(tabs);
        auto res_spaces = Lud::Strip(spaces);
        auto res_newline = Lud::Strip(newline);
        auto res_linefeed = Lud::Strip(linefeed);
        auto res_combination = Lud::Strip(combination);

        REQUIRE(tabs == "\tthis is a test\t");

        REQUIRE(res_tabs == "this is a test");
        REQUIRE(res_spaces == "this is a test");
        REQUIRE(res_newline == "this is a test");
        REQUIRE(res_linefeed == "this is a test");
        REQUIRE(res_combination == "this is a test");
    }

    SECTION("Inplace")
    {
        std::string tabs = "\tthis is a test\t";
        std::string spaces = "  this is a test  ";
        std::string newline = "\nthis is a test\n";
        std::string linefeed = "\tthis is a test\r";
        std::string combination = "\r\nthis is a test\t  ";

        Lud::inplace::Strip(tabs);
        Lud::inplace::Strip(spaces);
        Lud::inplace::Strip(newline);
        Lud::inplace::Strip(linefeed);
        Lud::inplace::Strip(combination);

        REQUIRE(tabs == "this is a test");
        REQUIRE(spaces == "this is a test");
        REQUIRE(newline == "this is a test");
        REQUIRE(linefeed == "this is a test");
        REQUIRE(combination == "this is a test");
    }

    SECTION("blank string")
    {
        std::string original = "      ";
        auto res = Lud::Strip(original);

        REQUIRE(res == "");
    }

    SECTION("blank string, inplace")
    {
        std::string original = "      ";
        Lud::inplace::Strip(original);

        REQUIRE(original == "");
    }
}

TEST_CASE("Reverse", "[parse][strings]")
{
    SECTION("Simple")
    {
        std::string original = "tset a si sihT";
        auto res = Lud::Reverse(original);

        REQUIRE(original == "tset a si sihT");
        REQUIRE(res == "This is a test");
    }

    SECTION("inplace")
    {
        std::string original = "tset a si sihT";
        Lud::inplace::Reverse(original);

        REQUIRE(original == "This is a test");
    }
}

TEST_CASE("Is Num integer", "[parse][numbers][integer]")
{
    SECTION("Simple")
    {
        REQUIRE(Lud::is_num<int>("+23").value() == 23);
        REQUIRE(Lud::is_num<short>("0").value() == 0);
        REQUIRE(Lud::is_num<long>("-1").value() == -1);
    }

    SECTION("Signed")
    {
        REQUIRE(Lud::is_num<int>("-1").value() == -1);
        REQUIRE(!Lud::is_num<unsigned int>("-1"));
    }

    SECTION("Out of range")
    {
        REQUIRE(Lud::is_num<uint8_t>("255").value() == 255);
        REQUIRE(!Lud::is_num<uint8_t>("256"));
    }

    SECTION("Bad parse")
    {
        REQUIRE(!Lud::is_num<int>(" not a number"));
        REQUIRE(!Lud::is_num<int>("23 not a number"));
    }

    SECTION("Different base")
    {
        REQUIRE(Lud::is_num<uint8_t>("0xFF", 16).value() == 255);
        REQUIRE(Lud::is_num<uint8_t>("0XFF", 16).value() == 255);
        REQUIRE(Lud::is_num<uint8_t>("  FF", 16).value() == 255);

        REQUIRE(Lud::is_num<uint8_t>("0b11111111", 2).value() == 255);
        REQUIRE(Lud::is_num<uint8_t>("0B11111111", 2).value() == 255);
        REQUIRE(Lud::is_num<uint8_t>("  11111111", 2).value() == 255);

        REQUIRE(Lud::is_num<uint8_t>("0o377", 8).value() == 255);
        REQUIRE(Lud::is_num<uint8_t>("0O377", 8).value() == 255);
        REQUIRE(Lud::is_num<uint8_t>(" 0377", 8).value() == 255);
        REQUIRE(Lud::is_num<uint8_t>("  377", 8).value() == 255);
    }
}
TEST_CASE("IS Num Real", "[parse][numbers][real]")
{
    SECTION("Simple")
    {
        REQUIRE(Lud::is_num<float>("23.23").value() == 23.23f);
        REQUIRE(Lud::is_num<double>("0.15").value() == 0.15);
        REQUIRE(Lud::is_num<long double>("-1.23").value() == -1.23L);
    }

    SECTION("Bad parse")
    {
        REQUIRE(!Lud::is_num<float>(" not a number"));
        REQUIRE(!Lud::is_num<float>("23.23 not a number"));
    }

    SECTION("Different fmt")
    {
        REQUIRE(Lud::is_num<float>("0xFF", std::chars_format::hex).value() == 255.f);
        REQUIRE(Lud::is_num<float>("0XFF", std::chars_format::hex).value() == 255.f);
        REQUIRE(Lud::is_num<float>("  FF", std::chars_format::hex).value() == 255.f);

        REQUIRE(Lud::is_num<float>("1e4", std::chars_format::scientific).value() == 1e4f);
        REQUIRE(!Lud::is_num<float>("e4", std::chars_format::scientific));
        REQUIRE(!Lud::is_num<float>("1", std::chars_format::scientific));
    }
}

TEST_CASE("Is fraction", "[parse][numbers][real]")
{
    SECTION("Simple")
    {
        REQUIRE(Lud::is_fraction<double>("1/2").value() == 1.f / 2.f);
        REQUIRE(Lud::is_fraction<double>("2/1").value() == 2.f);
        REQUIRE(Lud::is_fraction<double>("1.0/2.0").value() == 1.f / 2.f);
        REQUIRE(Lud::is_fraction<double>("1.5/4.0").value() == 1.5f / 4.f);
    }

    SECTION("spaced")
    {
        REQUIRE(Lud::is_fraction<double>("1 / 2").value() == 1.f / 2.f);
        REQUIRE(Lud::is_fraction<double>("1/ 2").value() == 1.f / 2.f);
        REQUIRE(Lud::is_fraction<double>("1 /2").value() == 1.f / 2.f);
        REQUIRE(Lud::is_fraction<double>("1   /   2").value() == 1.f / 2.f);
    }

    SECTION("bad parse")
    {
        REQUIRE(!Lud::is_fraction<double>("1/"));
        REQUIRE(!Lud::is_fraction<double>("/1"));
        REQUIRE(!Lud::is_fraction<double>("a/b"));
        REQUIRE(!Lud::is_fraction<double>("1/b"));
        REQUIRE(!Lud::is_fraction<double>("a/1"));
        REQUIRE(!Lud::is_fraction<double>("1"));
        REQUIRE(!Lud::is_fraction<double>("2"));
    }

    SECTION("limits")
    {
        REQUIRE(Lud::is_fraction<double>("0 / 2").value() == 0.0f);
        REQUIRE(std::isnan(Lud::is_fraction<double>("1 / 0").value()));
    }
}

TEST_CASE("Is Percentage", "[parse][numbers][real]")
{
    SECTION("Simple")
    {
        REQUIRE(Lud::is_percentage<double>("23%").value() == .23);
        REQUIRE(Lud::is_percentage<double>("0%").value() == .0);
        REQUIRE(Lud::is_percentage<double>("100%").value() == 1.0);
        REQUIRE(Lud::is_percentage<double>("0.23%").value() == 0.0023);
        REQUIRE(Lud::is_percentage<double>("1000%").value() == 10);
    }

    SECTION("Bad parse")
    {
        REQUIRE(!Lud::is_percentage<double>("23"));
        REQUIRE(!Lud::is_percentage<double>("23%%"));
        REQUIRE(!Lud::is_percentage<double>("%23"));
        REQUIRE(!Lud::is_percentage<double>("%23%"));
        REQUIRE(!Lud::is_percentage<double>("a%"));
        REQUIRE(!Lud::is_percentage<double>("a"));
    }
}
