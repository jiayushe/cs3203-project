#include "catch.hpp"

#include "Parser/PQLParser/StatementRef.h"
#include <string>

using namespace Parser;

TEST_CASE("PQLParser::StatementRef") {
    SECTION("check accessibility of method") {
        StatementRef statement_ref;
        statement_ref.set_type(StatementRefType::ANY);
        REQUIRE_THROWS(statement_ref.get_synonym());
        REQUIRE_THROWS(statement_ref.get_statement_id());

        statement_ref.set_type(StatementRefType::SYNONYM);
        REQUIRE_NOTHROW(statement_ref.get_synonym());
        REQUIRE_THROWS(statement_ref.get_statement_id());

        statement_ref.set_type(StatementRefType::STATEMENT_ID);
        REQUIRE_THROWS(statement_ref.get_synonym());
        REQUIRE_NOTHROW(statement_ref.get_statement_id());
    }
}