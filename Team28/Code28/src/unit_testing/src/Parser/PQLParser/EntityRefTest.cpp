#include "catch.hpp"

#include "Parser/PQLParser/EntityRef.h"
#include <string>

using namespace Parser;

TEST_CASE("PQLParser::EntityRef") {
    SECTION("check accessibility of method") {
        EntityRef entity_ref;
        entity_ref.set_type(EntityRefType::ANY);
        REQUIRE_THROWS(entity_ref.get_synonym());
        REQUIRE_THROWS(entity_ref.get_name());

        entity_ref.set_type(EntityRefType::SYNONYM);
        REQUIRE_NOTHROW(entity_ref.get_synonym());
        REQUIRE_THROWS(entity_ref.get_name());

        entity_ref.set_type(EntityRefType::NAME);
        REQUIRE_THROWS(entity_ref.get_synonym());
        REQUIRE_NOTHROW(entity_ref.get_name());
    }
}