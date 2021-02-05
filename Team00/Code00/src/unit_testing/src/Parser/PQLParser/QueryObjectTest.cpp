#include "catch.hpp"

#include "Parser/PQLParser/DesignEntity.h"
#include "Parser/PQLParser/QueryObject.h"
#include <string>

using namespace Parser;

TEST_CASE("PQLParser::QueryObject") {
    SECTION("check accessibility of method") {
        QueryObject query_object;
        REQUIRE_THROWS(query_object.get_pattern());
        REQUIRE_THROWS(query_object.get_such_that());

        query_object.set_has_pattern(true);
        REQUIRE_NOTHROW(query_object.get_pattern());

        query_object.set_has_such_that(true);
        REQUIRE_NOTHROW(query_object.get_such_that());
    }

    SECTION("duplicated synonym") {
        QueryObject query_object;
        DesignEntity designEntity;  // assume this is wont throw error

        REQUIRE_NOTHROW(query_object.add_declaration("a", designEntity));
        REQUIRE_NOTHROW(query_object.add_declaration("b", designEntity));
        REQUIRE_THROWS(query_object.add_declaration("b", designEntity));
    }
}