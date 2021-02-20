#include "catch.hpp"

#include "Parser/PQLParser/Ref.h"
#include <string>

using namespace Parser;

TEST_CASE("PQLParser::Ref") {
    SECTION("check accessibility of method") {
        EntityRef entity_ref;
        StatementRef statement_ref;

        Ref ref_ent(entity_ref);
        Ref ref_stmt(statement_ref);

        REQUIRE(ref_ent.get_type() == RefType::ENTITY);
        REQUIRE_NOTHROW(ref_ent.get_entity_ref());
        REQUIRE_THROWS(ref_ent.get_statement_ref());

        REQUIRE(ref_stmt.get_type() == RefType::STATEMENT);
        REQUIRE_THROWS(ref_stmt.get_entity_ref());
        REQUIRE_NOTHROW(ref_stmt.get_statement_ref());
    }
}