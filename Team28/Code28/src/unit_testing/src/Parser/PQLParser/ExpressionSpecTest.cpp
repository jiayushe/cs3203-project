#include "catch.hpp"

#include "Parser/PQLParser/ExpressionSpec.h"
#include <string>

using namespace Parser;

TEST_CASE("PQLParser::ExpressionSpec") {
    SECTION("check accessibility of method") {
        ExpressionSpec expression_spec;
        expression_spec.set_type(ExpressionSpecType::ANY);
        REQUIRE(expression_spec.get_type() == ExpressionSpecType::ANY);
        REQUIRE_THROWS(expression_spec.get_pattern());

        expression_spec.set_type(ExpressionSpecType::PATTERN);
        expression_spec.set_pattern(nullptr);
        REQUIRE_NOTHROW(expression_spec.get_pattern());
    }
}