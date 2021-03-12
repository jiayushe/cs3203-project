#include "catch.hpp"

#include "Parser/PQLParser/SuchThatRef.h"
#include <string>

using namespace Parser;

TEST_CASE("PQLParser::SuchThatRef") {
    SECTION("check accessibility of method") {
        EntityRef entity_ref;
        StatementRef statement_ref;

        SuchThatRef ref_ent(entity_ref);
        SuchThatRef ref_stmt(statement_ref);

        REQUIRE(ref_ent.get_type() == SuchThatRefType::ENTITY);
        REQUIRE_NOTHROW(ref_ent.get_entity_ref());
        REQUIRE_THROWS(ref_ent.get_statement_ref());

        REQUIRE(ref_stmt.get_type() == SuchThatRefType::STATEMENT);
        REQUIRE_THROWS(ref_stmt.get_entity_ref());
        REQUIRE_NOTHROW(ref_stmt.get_statement_ref());
    }
}