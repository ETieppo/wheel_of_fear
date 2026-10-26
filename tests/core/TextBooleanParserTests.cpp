#include "core/text/TextBooleanParser.hpp"
#include <doctest/doctest.h>

namespace wheel_of_fear {
  TEST_CASE("truthy words parse as true regardless of case") {
    CHECK(TextBooleanParser::parse("1") == true);
    CHECK(TextBooleanParser::parse("true") == true);
    CHECK(TextBooleanParser::parse("TRUE") == true);
    CHECK(TextBooleanParser::parse("Yes") == true);
    CHECK(TextBooleanParser::parse("on") == true);
  }

  TEST_CASE("falsy words parse as false regardless of case") {
    CHECK(TextBooleanParser::parse("0") == false);
    CHECK(TextBooleanParser::parse("false") == false);
    CHECK(TextBooleanParser::parse("No") == false);
    CHECK(TextBooleanParser::parse("OFF") == false);
  }

  TEST_CASE("unknown words are rejected") {
    CHECK_FALSE(TextBooleanParser::parse("").has_value());
    CHECK_FALSE(TextBooleanParser::parse("2").has_value());
    CHECK_FALSE(TextBooleanParser::parse("maybe").has_value());
  }
} // namespace wheel_of_fear
