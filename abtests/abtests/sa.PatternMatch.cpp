/**
 * Copyright 2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <catch.hpp>

#include <sa/StringTempl.h>
#include <string>

TEST_CASE("PatternMatch simple")
{
    std::string s1 = "foo";
    std::string s2 = "foo";
    REQUIRE(sa::PatternMatch(s1, s2));
}

TEST_CASE("PatternMatch *")
{
    std::string s1 = "foobarnstuff";
    std::string s2 = "foo*";
    REQUIRE(sa::PatternMatch(s1, s2));
}

TEST_CASE("PatternMatch * 2")
{
    std::string s1 = "foobarbaz";
    std::string s2 = "foo*baz";
    REQUIRE(sa::PatternMatch(s1, s2));
}

TEST_CASE("PatternMatch * 3")
{
    std::string s1 = "foobarbaz";
    std::string s2 = "*bar*";
    std::string s3 = "bar*";
    REQUIRE(sa::PatternMatch(s1, s2));
    REQUIRE(!sa::PatternMatch(s1, s3));
}

TEST_CASE("PatternMatch * 4")
{
    std::string s1 = "foobarbaz";
    std::string s2 = "*foo*";
    REQUIRE(sa::PatternMatch(s1, s2));
}

TEST_CASE("PatternMatch * 5")
{
    std::string s1 = "Insignia";
    std::string s2 = "*ins*";
    REQUIRE(sa::PatternMatch(s1, s2));
    std::string s3 = "*ignia*";
    REQUIRE(sa::PatternMatch(s1, s3));
}

TEST_CASE("PatternMatch wchar_t *")
{
    std::wstring s1 = L"foobarbaz";
    std::wstring s2 = L"*bar*";
    REQUIRE(sa::PatternMatch(s1, s2));
}

TEST_CASE("PatternMatch ?")
{
    std::string s1 = "fouo";
    std::string s2 = "fo?o";
    REQUIRE(sa::PatternMatch(s1, s2));
}

TEST_CASE("PatternMatch CI *")
{
    std::string s1 = "foobarnstuff";
    std::string s2 = "fOo*";
    REQUIRE(sa::PatternMatch(s1, s2));
}

TEST_CASE("PatternMatch CI wchar_t *")
{
    std::wstring s1 = L"foobarbaz";
    std::wstring s2 = L"*bAr*";
    REQUIRE(sa::PatternMatch(s1, s2));
}
