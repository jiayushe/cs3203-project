#include "catch.hpp"

#include "KnowledgeBase/PKB.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "Parser/SimpleParser/SimpleParser.h"
#include "Parser/shared/Source.h"
#include "SimpleExtractor/DesignExtractor.h"
#include <memory>
#include <string>

using namespace KnowledgeBase;
using namespace Parser;

std::shared_ptr<SimpleNode> build_ast_for_next_and_affects() {
    std::string source;

    source += "procedure First {";
    source += "    a = 0;";
    source += "    b = 5;";
    source += "    while (c < 10) {";
    source += "        if (d != 2) then {";
    source += "            d = a + 1;";
    source += "        } else {";
    source += "            e = a + d;";
    source += "            h = b;";
    source += "        }";
    source += "        e = b + a;";
    source += "        z = 1;";
    source += "        call Second;";
    source += "        f = z * 10;";
    source += "        c = c + 1;";
    source += "        while (d < 10) {";
    source += "            d = a * 2;";
    source += "            k = h + 2;";
    source += "        }";
    source += "    }";
    source += "    e = e + 1;";
    source += "    if (i == 2) then {";
    source += "        read e;";
    source += "        if (j != 1) then {";
    source += "            g = a / 3 + e;";
    source += "        } else {";
    source += "            m = k * 10;";
    source += "        }";
    source += "    } else {";
    source += "        a = 5;";
    source += "        while (j < 3) {";
    source += "            g = a + 1;";
    source += "            j = j - 1;";
    source += "        }";
    source += "    }";
    source += "    n = m - 1;";
    source += "    print a;";
    source += "    a = a * b;";
    source += "}";

    source += "procedure Second {";
    source += "    if (c != 1) then {";
    source += "        z = 5 + b;";
    source += "    } else {";
    source += "        c = c + 1;";
    source += "    }";
    source += "    v = z;";
    source += "    print v;";
    source += "}";

    auto base_source = std::make_shared<Parser::Source>(source);
    Parser::SimpleParser parser(base_source);
    auto ast = parser.parse_program();
    return ast;
}

TEST_CASE("Test for next and affects relationship with multi-nested source program") {
    auto ast = build_ast_for_next_and_affects();
    auto pkb = std::make_shared<KnowledgeBase::PKB>(ast);
    SimpleExtractor::DesignExtractor::extract_cfg(pkb);
    SimpleExtractor::DesignExtractor::extract_next_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_follow_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_parent_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_call_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_modify_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_use_relationship(pkb);
    SimpleExtractor::DesignExtractor::extract_affect_relationship(pkb);
}
