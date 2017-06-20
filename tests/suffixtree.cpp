#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "suffixtree.h"

TEST_CASE("is_substring works") {
    SuffixTree<> st;
    std::string s("banana$");
    st.append(s.begin(), s.end());

    {
        std::string ss("ana$");
        REQUIRE(st.is_substring(ss.begin(), ss.end()));
    }

    {
        std::string ss("aa");
        REQUIRE(!st.is_substring(ss.begin(), ss.end()));
    }

    {
        std::string ss("");
        REQUIRE(st.is_substring(ss.begin(), ss.end()));
    }

    {
        std::string ss("a");
        REQUIRE(st.is_substring(ss.begin(), ss.end()));
    }

    {
        std::string ss("ba");
        REQUIRE(st.is_substring(ss.begin(), ss.end()));
    }
}

TEST_CASE("is_suffix works") {
    SuffixTree<> st;
    std::string s("banana$");
    st.append(s.begin(), s.end());

    {
        std::string ss("ana$");
        REQUIRE(st.is_suffix(ss.begin(), ss.end()));
    }

    {
        std::string ss("ana");
        REQUIRE(!st.is_suffix(ss.begin(), ss.end()));
    }

    {
        std::string ss("");
        REQUIRE(st.is_suffix(ss.begin(), ss.end()));
    }
}

TEST_CASE("substring_count works") {
    SuffixTree<> st;
    std::string s("banana$");
    st.append(s.begin(), s.end());

    {
        std::string ss("a");
        REQUIRE(st.substring_count(ss.begin(), ss.end()) == 3);
    }

    {
        std::string ss("na");
        REQUIRE(st.substring_count(ss.begin(), ss.end()) == 2);
    }

    {
        std::string ss("an");
        REQUIRE(st.substring_count(ss.begin(), ss.end()) == 2);
    }

    {
        std::string ss("x");
        REQUIRE(st.substring_count(ss.begin(), ss.end()) == 0);
    }

    {
        std::string ss("");
        REQUIRE(st.substring_count(ss.begin(), ss.end()) == 7);
    }
}

TEST_CASE("multiple strings work") {
    SuffixTree<> st;
    std::string s("abab$0");
    st.add_string(s.begin(), s.end());
    s = "baba$1";
    st.add_string(s.begin(), s.end());

    {
        std::string ss("ab$0");
        REQUIRE(st.is_suffix(ss.begin(), ss.end()));
    }

    {
        std::string ss("ba$1");
        REQUIRE(st.is_suffix(ss.begin(), ss.end()));
    }

    {
        std::string ss("ba$0");
        REQUIRE(!st.is_suffix(ss.begin(), ss.end()));
    }
}
