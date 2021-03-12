#include "catch.hpp"

#include "Parser/PQLParser/DesignEntity.h"
#include "Parser/PQLParser/QueryObject.h"
#include <string>

using namespace Parser;

TEST_CASE("PQLParser::QueryObject") {
    SECTION("duplicated synonym") {
        QueryObject query_object;
        DesignEntity designEntity; // assume this is wont throw error

        REQUIRE(query_object.get_declarations().size() == 0);
        REQUIRE_NOTHROW(query_object.add_declaration("a", designEntity));
        REQUIRE_NOTHROW(query_object.add_declaration("b", designEntity));
        REQUIRE_THROWS(query_object.add_declaration("b", designEntity));
        REQUIRE(query_object.get_declarations().size() == 2);
    }
}