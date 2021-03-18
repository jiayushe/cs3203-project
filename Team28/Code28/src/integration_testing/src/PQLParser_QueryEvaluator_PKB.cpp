#include "catch.hpp"

#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/PQLParser.h"
#include "Parser/SimpleParser/SimpleNode.h"
#include "Parser/shared/Source.h"
#include "QueryEvaluator/Evaluator.h"
#include <algorithm>
#include <list>
#include <memory>
#include <vector>

using namespace Parser;
using namespace QueryEvaluator;
using namespace KnowledgeBase;

// Build a PKB as if the following SIMPLE source is parsed and extracted:
//
//   procedure P1 {
// 1   read V1;
// 2   if (V1 > V3) {
// 3     print V2;
//     } else {
// 4     call P2;
//     }
//   }
//
//   procedure P2 {
// 5   P1 = P1 + 33 * 77;
// 6   call P3;
// 7   V2 = (3 + 7) % 33 - 77;
//   }
//
//   procedure P3 {
// 8   while (V2 > 33) {
// 9     if (3 < 7) {
// 10      print V1;
//       } else {
// 11      print P1;
//       }
//     }
//   }
std::shared_ptr<PKB> build_pkb() {
    auto empty_ast = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);
    auto pkb = std::make_shared<PKB>(empty_ast);

    // Add constants
    pkb->add_constant(3);
    pkb->add_constant(7);
    pkb->add_constant(33);
    pkb->add_constant(77);

    // Add variables
    pkb->add_variable("P1");
    pkb->add_variable("V1");
    pkb->add_variable("V2");
    pkb->add_variable("V3");

    // Commonly used AST nodes
    std::unordered_map<std::string, std::shared_ptr<SimpleNode>> proc_name_nodes{
        {"P1", std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "P1")},
        {"P2", std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "P2")},
        {"P3", std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, "P3")},
    };
    std::unordered_map<std::string, std::shared_ptr<SimpleNode>> var_name_nodes{
        {"P1", std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "P1")},
        {"V1", std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "V1")},
        {"V2", std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "V2")},
        {"V3", std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, "V3")},
    };
    std::unordered_map<int, std::shared_ptr<SimpleNode>> const_value_nodes{
        {3, std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "3")},
        {7, std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "7")},
        {33, std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "33")},
        {77, std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE, "77")},
    };

    // Add statements in P1
    pkb->add_procedure("P1");
    auto read_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::READ, 1);
    pkb->add_statement(StatementType::READ, 1, "P1", read_node_1);
    auto if_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::IF, 2);
    pkb->add_statement(StatementType::IF, 2, "P1", if_node_1);
    auto print_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 3);
    pkb->add_statement(StatementType::PRINT, 3, "P1", print_node_1);
    auto call_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::CALL, 4);
    call_node_1->add_child(proc_name_nodes["P2"]);
    pkb->add_statement(StatementType::CALL, 4, "P1", call_node_1);

    // Add statements in P2
    pkb->add_procedure("P2");
    auto assign_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 5);
    auto arith_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    auto arith_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "*");
    arith_node_2->add_child(const_value_nodes[33]);
    arith_node_2->add_child(const_value_nodes[77]);
    arith_node_1->add_child(var_name_nodes["P1"]);
    arith_node_1->add_child(arith_node_2);
    assign_node_1->add_child(var_name_nodes["P1"]);
    assign_node_1->add_child(arith_node_1);
    pkb->add_statement(StatementType::ASSIGN, 5, "P2", assign_node_1);
    auto call_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::CALL, 6);
    call_node_2->add_child(proc_name_nodes["P3"]);
    pkb->add_statement(StatementType::CALL, 6, "P2", call_node_2);
    auto assign_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, 7);
    auto arith_node_3 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "+");
    auto arith_node_4 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "%");
    auto arith_node_5 = std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, "-");
    arith_node_3->add_child(const_value_nodes[3]);
    arith_node_3->add_child(const_value_nodes[7]);
    arith_node_4->add_child(arith_node_3);
    arith_node_4->add_child(const_value_nodes[33]);
    arith_node_5->add_child(arith_node_4);
    arith_node_5->add_child(const_value_nodes[77]);
    assign_node_2->add_child(var_name_nodes["V2"]);
    assign_node_2->add_child(arith_node_5);
    pkb->add_statement(StatementType::ASSIGN, 7, "P2", assign_node_2);

    // Add statements in P3
    pkb->add_procedure("P3");
    auto while_node_1 = std::make_shared<SimpleNode>(SimpleNodeType::WHILE, 8);
    pkb->add_statement(StatementType::WHILE, 8, "P3", while_node_1);
    auto if_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::IF, 9);
    pkb->add_statement(StatementType::IF, 9, "P3", if_node_2);
    auto print_node_2 = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 10);
    pkb->add_statement(StatementType::PRINT, 10, "P3", print_node_2);
    auto print_node_3 = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, 11);
    pkb->add_statement(StatementType::PRINT, 11, "P3", print_node_3);

    // Add calls relationships
    pkb->add_call_relationship("P1", "P2");
    pkb->add_call_relationship("P2", "P3");

    // Add follows relationships
    pkb->add_follow_relationship(1, 2);
    pkb->add_follow_relationship(5, 6);
    pkb->add_follow_relationship(6, 7);

    // Add parent relationships
    pkb->add_parent_relationship(2, 3);
    pkb->add_parent_relationship(2, 4);
    pkb->add_parent_relationship(8, 9);
    pkb->add_parent_relationship(9, 10);
    pkb->add_parent_relationship(9, 11);

    // Add next relationships
    pkb->add_next_relationship(1, 2);
    pkb->add_next_relationship(2, 3);
    pkb->add_next_relationship(2, 4);
    pkb->add_next_relationship(5, 6);
    pkb->add_next_relationship(6, 7);
    pkb->add_next_relationship(8, 9);
    pkb->add_next_relationship(9, 10);
    pkb->add_next_relationship(9, 11);
    pkb->add_next_relationship(10, 8);
    pkb->add_next_relationship(11, 8);

    // Add modifies relationships
    pkb->add_modify_relationship(1, "V1");
    pkb->add_modify_relationship(5, "P1");
    pkb->add_modify_relationship(7, "V2");

    // Add uses relationships
    pkb->add_use_relationship(2, "V1");
    pkb->add_use_relationship(2, "V3");
    pkb->add_use_relationship(3, "V2");
    pkb->add_use_relationship(5, "P1");
    pkb->add_use_relationship(8, "V2");
    pkb->add_use_relationship(10, "V1");
    pkb->add_use_relationship(11, "P1");

    return pkb;
}

std::list<std::string> evaluate(const std::string& query) {
    auto pkb = build_pkb();
    auto source = std::make_shared<Source>(query);
    PQLParser parser(source);
    auto query_object = parser.parse_query();
    std::list<std::string> output;
    Evaluator::evaluate(pkb, query_object, output);
    return output;
}

TEST_CASE("PQLParser_QueryEvaluator_PKB") {

    auto fixture = GENERATE(table<std::string, std::list<std::string>>({
        // ######################################
        // Queries without any clause
        // ######################################

        {"           Select BOOLEAN", {"TRUE"}},
        {"stmt stmt; Select BOOLEAN", {"TRUE"}},

        {"procedure procedure; Select procedure", {"P1", "P2", "P3"}},
        {"procedure Select;    Select Select.procName", {"P1", "P2", "P3"}},

        {"variable such; Select such", {"P1", "V1", "V2", "V3"}},
        {"variable that; Select that.varName", {"P1", "V1", "V2", "V3"}},

        {"constant pattern; Select pattern", {"3", "7", "33", "77"}},
        {"constant c; Select c.value", {"3", "7", "33", "77"}},

        {"stmt s; Select s", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"}},
        {"stmt s; Select s.stmt#", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"}},

        {"prog_line p; Select p", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"}},

        {"read read; Select read", {"1"}},
        {"read read; Select read.stmt#", {"1"}},
        {"read read; Select read.varName", {"V1"}},

        {"print print; Select print", {"3", "10", "11"}},
        {"print print; Select print.stmt#", {"3", "10", "11"}},
        {"print print; Select print.varName", {"P1", "V1", "V2"}},

        {"call with; Select with", {"4", "6"}},
        {"call with; Select with.stmt#", {"4", "6"}},
        {"call with; Select with.procName", {"P2", "P3"}},

        {"assign Follows; Select Follows", {"5", "7"}},
        {"assign Next;    Select Next.stmt#", {"5", "7"}},

        {"if boolean; Select boolean", {"2", "9"}},
        {"if Calls;   Select Calls.stmt#", {"2", "9"}},

        {"while Modifies; Select Modifies", {"8"}},
        {"while Uses;     Select Uses.stmt#", {"8"}},

        {"procedure p;              Select <p>", {"P1", "P2", "P3"}},
        {"procedure p; constant c;  Select <p, c>",
         {"P1 3", "P1 7", "P1 33", "P1 77", "P2 3", "P2 7", "P2 33", "P2 77", "P3 3", "P3 7",
          "P3 33", "P3 77"}},
        {"assign a; read r; call c; Select <a.stmt#, r.varName, c.procName>",
         {"5 V1 P2", "5 V1 P3", "7 V1 P2", "7 V1 P3"}},
        {"call c;                   Select <c, c, c.procName>", {"4 4 P2", "6 6 P3"}},

        // ######################################
        // Queries with a single such-that clause
        // ######################################

        // ModifiesS
        {"stmt s;              Select BOOLEAN   such that Modifies(s, _)", {"TRUE"}},
        {"while w;             Select BOOLEAN   such that Modifies(w, _)", {"FALSE"}},
        {"if i; variable v;    Select <i, v>    such that Modifies(i, v)", {"2 P1", "2 V2"}},
        {"call c; variable v;  Select BOOLEAN   such that Modifies(c, v)", {"TRUE"}},
        {"while w; variable v; Select v         such that Modifies(w, v)", {}},
        {"prog_line p;         Select p         such that Modifies(p, \"V2\")", {"2", "4", "7"}},
        {"assign a;            Select BOOLEAN   such that Modifies(a, \"V1\")", {"FALSE"}},
        {"                     Select BOOLEAN   such that Modifies(1, _)", {"TRUE"}},
        {"procedure p;         Select p         such that Modifies(6, _)", {}},
        {"variable v;          Select v.varName such that Modifies(4, v)", {"P1", "V2"}},
        {"variable v;          Select v         such that Modifies(5, v)", {"P1"}},
        {"procedure p;         Select p         such that Modifies(1, \"V1\")", {"P1", "P2", "P3"}},
        {"                     Select BOOLEAN   such that Modifies(10, \"V1\")", {"FALSE"}},

        // ModifiesP
        {"procedure p1, p2;        Select <p1, p2>  such that Modifies(p1, _)",
         {"P1 P1", "P1 P2", "P1 P3", "P2 P1", "P2 P2", "P2 P3"}},
        {"procedure p; variable v; Select BOOLEAN   such that Modifies(p, v)", {"TRUE"}},
        {"procedure p; variable v; Select <p, v>    such that Modifies(p, v)",
         {"P1 P1", "P1 V1", "P1 V2", "P2 P1", "P2 V2"}},
        {"procedure p;             Select p         such that Modifies(p, \"P1\")", {"P1", "P2"}},
        {"procedure p;             Select p         such that Modifies(p, \"V1\")", {"P1"}},
        {"                         Select BOOLEAN   such that Modifies(\"P2\", _)", {"TRUE"}},
        {"                         Select BOOLEAN   such that Modifies(\"P3\", _)", {"FALSE"}},
        {"variable v;              Select v         such that Modifies(\"P1\", v)",
         {"P1", "V1", "V2"}},
        {"variable v;              Select v         such that Modifies(\"P2\", v)", {"P1", "V2"}},
        {"read r;                  Select r.varName such that Modifies(\"P1\", \"P1\")", {"V1"}},
        {"procedure p;             Select p         such that Modifies(\"P3\", \"V1\")", {}},

        // UsesS
        {"stmt s;                  Select BOOLEAN   such that Uses(s, _)", {"TRUE"}},
        {"assign a;                Select a         such that Uses(a, _)", {"5"}},
        {"prog_line p; variable v; Select <p, v>    such that Uses(p, v)",
         {"2 P1", "2 V1", "2 V2", "2 V3", "3 V2", "4 P1", "4 V1", "4 V2", "5 P1", "6 P1", "6 V1",
          "6 V2", "8 P1", "8 V1", "8 V2", "9 P1", "9 V1", "10 V1", "11 P1"}},
        {"call c; variable v;      Select BOOLEAN   such that Uses(c, v)", {"TRUE"}},
        {"if i;                    Select i.stmt#   such that Uses(i, \"V1\")", {"2", "9"}},
        {"print p;                 Select p.stmt#   such that Uses(p, \"V1\")", {"10"}},
        {"procedure p;             Select p         such that Uses(6, _)", {"P1", "P2", "P3"}},
        {"procedure p;             Select p         such that Uses(1, _)", {}},
        {"variable v;              Select v.varName such that Uses(8, v)", {"P1", "V1", "V2"}},
        {"variable v;              Select v.varName such that Uses(9, v)", {"P1", "V1"}},
        {"variable v;              Select v         such that Uses(7, v)", {}},
        {"                         Select BOOLEAN   such that Uses(2, \"P1\")", {"TRUE"}},
        {"procedure p;             Select p         such that Uses(1, \"V1\")", {}},

        // UsesP
        {"procedure p;             Select BOOLEAN   such that Uses(p, _)", {"TRUE"}},
        {"procedure p; variable v; Select <p, v>    such that Uses(p, v)",
         {"P1 P1", "P1 V1", "P1 V2", "P1 V3", "P2 P1", "P2 V1", "P2 V2", "P3 P1", "P3 V1",
          "P3 V2"}},
        {"procedure p;             Select BOOLEAN   such that Uses(p, \"P1\")", {"TRUE"}},
        {"procedure p;             Select p         such that Uses(\"P3\", _)", {"P1", "P2", "P3"}},
        {"variable v;              Select v         such that Uses(\"P2\", v)", {"P1", "V1", "V2"}},
        {"                         Select BOOLEAN   such that Uses(\"P1\", \"P1\")", {"TRUE"}},

        // Follows
        {"procedure p;      Select p         such that Follows(_, _)", {"P1", "P2", "P3"}},
        {"if i;             Select i         such that Follows(_, i)", {"2"}},
        {"print p;          Select BOOLEAN   such that Follows(_, p)", {"FALSE"}},
        {"while w;          Select BOOLEAN   such that Follows(_, 6)", {"TRUE"}},
        {"procedure p;      Select p         such that Follows(_, 5)", {}},
        {"stmt s;           Select BOOLEAN   such that Follows(s, _)", {"TRUE"}},
        {"print p;          Select p         such that Follows(p, _)", {}},
        {"prog_line p1, p2; Select <p1, p2>  such that Follows(p1, p2)", {"1 2", "5 6", "6 7"}},
        {"stmt s;           Select BOOLEAN   such that Follows(s, s)", {"FALSE"}},
        {"stmt s;           Select s         such that Follows(s, 6)", {"5"}},
        {"if i;             Select i         such that Follows(i, 6)", {}},
        {"procedure p;      Select BOOLEAN   such that Follows(1, _)", {"TRUE"}},
        {"procedure p;      Select <p>       such that Follows(2, _)", {}},
        {"assign a; call c; Select <a, c>    such that Follows(6, a)", {"7 4", "7 6"}},
        {"call c;           Select c         such that Follows(6, c)", {}},
        {"read r;           Select r.varName such that Follows(6, 7)", {"V1"}},
        {"read r;           Select r.varName such that Follows(6, 8)", {}},

        // FollowsT
        {"procedure p;           Select BOOLEAN      such that Follows*(_, _)", {"TRUE"}},
        {"assign a;              Select <a.stmt#>    such that Follows*(_, a)", {"7"}},
        {"read r;                Select r            such that Follows*(_, r)", {}},
        {"procedure p;           Select p            such that Follows*(_, 2)", {"P1", "P2", "P3"}},
        {"stmt s;                Select s            such that Follows*(_, 1)", {}},
        {"prog_line p;           Select p            such that Follows*(p, _)", {"1", "5", "6"}},
        {"print p;               Select BOOLEAN      such that Follows*(p, _)", {"FALSE"}},
        {"assign a; prog_line p; Select <a.stmt#, p> such that Follows*(a, p)", {"5 6", "5 7"}},
        {"while w; assign a;     Select BOOLEAN      such that Follows*(a, w)", {"FALSE"}},
        {"stmt s;                Select s            such that Follows*(s, 7)", {"5", "6"}},
        {"stmt s;                Select s            such that Follows*(s, 8)", {}},
        {"                       Select BOOLEAN      such that Follows*(6, _)", {"TRUE"}},
        {"                       Select BOOLEAN      such that Follows*(7, _)", {"FALSE"}},
        {"assign a;              Select BOOLEAN      such that Follows*(5, a)", {"TRUE"}},
        {"while w;               Select w.stmt#      such that Follows*(5, w)", {}},
        {"procedure p;           Select p            such that Follows*(5, 7)", {"P1", "P2", "P3"}},
        {"procedure p;           Select p            such that Follows*(7, 5)", {}},

        // Parent
        {"                  Select BOOLEAN  such that Parent(_, _)", {"TRUE"}},
        {"prog_line p;      Select p        such that Parent(_, p)", {"3", "4", "9", "10", "11"}},
        {"while w;          Select w        such that Parent(_, w)", {}},
        {"procedure p;      Select p        such that Parent(_, 11)", {"P1", "P2", "P3"}},
        {"procedure p;      Select p        such that Parent(_, 5)", {}},
        {"stmt s;           Select s        such that Parent(s, _)", {"2", "8", "9"}},
        {"prog_line p1, p2; Select <p1, p2> such that Parent(p1, p2)",
         {"2 3", "2 4", "8 9", "9 10", "9 11"}},
        {"stmt s;           Select BOOLEAN  such that Parent(s, s)", {"FALSE"}},
        {"if i;             Select i.stmt#  such that Parent(i, 10)", {"9"}},
        {"while w;          Select BOOLEAN  such that Parent(w, 10)", {"FALSE"}},
        {"procedure p;      Select p        such that Parent(2, _)", {"P1", "P2", "P3"}},
        {"                  Select BOOLEAN  such that Parent(1, _)", {"FALSE"}},
        {"print p;          Select p        such that Parent(2, p)", {"3"}},
        {"call c;           Select c        such that Parent(2, c)", {"4"}},
        {"                  Select BOOLEAN  such that Parent(9, 10)", {"TRUE"}},
        {"                  Select BOOLEAN  such that Parent(8, 10)", {"FALSE"}},

        // ParentT
        {"procedure p;      Select p        such that Parent*(_, _)", {"P1", "P2", "P3"}},
        {"stmt s;           Select s.stmt#  such that Parent*(_, s)", {"3", "4", "9", "10", "11"}},
        {"                  Select BOOLEAN  such that Parent*(_, 4)", {"TRUE"}},
        {"                  Select BOOLEAN  such that Parent*(_, 8)", {"FALSE"}},
        {"prog_line p;      Select p        such that Parent*(p, _)", {"2", "8", "9"}},
        {"stmt s1, s2;      Select <s1, s2> such that Parent*(s1, s2)",
         {"2 3", "2 4", "8 9", "8 10", "8 11", "9 10", "9 11"}},
        {"prog_line p;      Select p        such that Parent*(p, p)", {}},
        {"stmt s;           Select s        such that Parent*(s, 11)", {"8", "9"}},
        {"while w;          Select BOOLEAN  such that Parent*(w, 4)", {"FALSE"}},
        {"                  Select BOOLEAN  such that Parent*(9, _)", {"TRUE"}},
        {"                  Select BOOLEAN  such that Parent*(6, _)", {"FALSE"}},
        {"print p;          Select p        such that Parent*(8, p)", {"10", "11"}},
        {"prog_line p;      Select p        such that Parent*(1, p)", {}},
        {"                  Select BOOLEAN  such that Parent*(2, 4)", {"TRUE"}},
        {"                  Select BOOLEAN  such that Parent*(1, 2)", {"FALSE"}},

        // Calls
        {"procedure p;          Select p          such that Calls(_, _)", {"P1", "P2", "P3"}},
        {"procedure p;          Select p          such that Calls(_, p)", {"P2", "P3"}},
        {"procedure p;          Select BOOLEAN    such that Calls(_, \"P2\")", {"TRUE"}},
        {"procedure p;          Select BOOLEAN    such that Calls(_, \"P1\")", {"FALSE"}},
        {"procedure p;          Select p.procName such that Calls(p, _)", {"P1", "P2"}},
        {"procedure p;          Select BOOLEAN    such that Calls(p, _)", {"TRUE"}},
        {"procedure p1, p2;     Select <p1, p2>   such that Calls(p1, p2)", {"P1 P2", "P2 P3"}},
        {"procedure p1, p2, p3; Select <p1, p3>   such that Calls(p1, p2)",
         {"P1 P1", "P1 P2", "P1 P3", "P2 P1", "P2 P2", "P2 P3"}},
        {"procedure p;          Select BOOLEAN    such that Calls(p, \"P3\")", {"TRUE"}},
        {"procedure p;          Select p          such that Calls(p, \"P1\")", {}},
        {"                      Select BOOLEAN    such that Calls(\"P1\", _)", {"TRUE"}},
        {"                      Select BOOLEAN    such that Calls(\"P3\", _)", {"FALSE"}},
        {"procedure p;          Select p          such that Calls(\"P2\", p)", {"P3"}},
        {"procedure p;          Select p          such that Calls(\"P3\", p)", {}},
        {"read r;               Select r          such that Calls(\"P1\", \"P2\")", {"1"}},
        {"read r;               Select r          such that Calls(\"P1\", \"P3\")", {}},

        // CallsT
        {"                      Select BOOLEAN    such that Calls*(_, _)", {"TRUE"}},
        {"procedure p;          Select p.procName such that Calls*(_, p)", {"P2", "P3"}},
        {"procedure p;          Select p          such that Calls*(_, \"P3\")", {"P1", "P2", "P3"}},
        {"procedure p;          Select p          such that Calls*(_, \"P1\")", {}},
        {"procedure p;          Select p          such that Calls*(p, _)", {"P1", "P2"}},
        {"procedure p, q;       Select <p, q>     such that Calls*(p, q)",
         {"P1 P2", "P1 P3", "P2 P3"}},
        {"procedure p;          Select <p>        such that Calls*(p, \"P3\")", {"P1", "P2"}},
        {"procedure p;          Select BOOLEAN    such that Calls*(p, \"P1\")", {"FALSE"}},
        {"procedure p;          Select p          such that Calls*(\"P1\", _)", {"P1", "P2", "P3"}},
        {"                      Select BOOLEAN    such that Calls*(\"P3\", _)", {"FALSE"}},
        {"procedure p;          Select BOOLEAN    such that Calls*(\"P1\", p)", {"TRUE"}},
        {"procedure p;          Select BOOLEAN    such that Calls*(\"P3\", p)", {"FALSE"}},
        {"print p;              Select p          such that Calls*(\"P1\", \"P3\")",
         {"3", "10", "11"}},
        {"print p;              Select p          such that Calls*(\"P3\", \"P1\")", {}},

        // Next
        {"                     Select BOOLEAN      such that Next(_, _)", {"TRUE"}},
        {"prog_line p;         Select p            such that Next(_, p)",
         {"2", "3", "4", "6", "7", "8", "9", "10", "11"}},
        {"if i;                Select i            such that Next(_, i)", {"2", "9"}},
        {"read r;              Select r            such that Next(_, r)", {}},
        {"                     Select BOOLEAN      such that Next(_, 8)", {"TRUE"}},
        {"                     Select BOOLEAN      such that Next(_, 5)", {"FALSE"}},
        {"stmt s;              Select s            such that Next(s, _)",
         {"1", "2", "5", "6", "8", "9", "10", "11"}},
        {"print p;             Select p            such that Next(p, _)", {"10", "11"}},
        {"prog_line p; stmt s; Select <p, s.stmt#> such that Next(p, s)",
         {"1 2", "2 3", "2 4", "5 6", "6 7", "8 9", "9 10", "9 11", "10 8", "11 8"}},
        {"stmt s;              Select s            such that Next(s, s)", {}},
        {"prog_line p;         Select p            such that Next(p, 8)", {"10", "11"}},
        {"prog_line p;         Select BOOLEAN      such that Next(p, 5)", {"FALSE"}},
        {"                     Select BOOLEAN      such that Next(11, _)", {"TRUE"}},
        {"procedure p;         Select p            such that Next(3, _)", {}},
        {"while w;             Select w            such that Next(10, w)", {"8"}},
        {"while w;             Select w            such that Next(2, w)", {}},
        {"                     Select BOOLEAN      such that Next(2, 4)", {"TRUE"}},
        {"                     Select BOOLEAN      such that Next(1, 4)", {"FALSE"}},
        {"                     Select BOOLEAN      such that Next(9, 8)", {"FALSE"}},

        // NextT
        {"procedure p; Select p         such that Next*(_, _)", {"P1", "P2", "P3"}},
        {"stmt s;      Select s         such that Next*(_, s)",
         {"2", "3", "4", "6", "7", "8", "9", "10", "11"}},
        {"             Select BOOLEAN   such that Next*(_, 8)", {"TRUE"}},
        {"procedure p; Select p         such that Next*(_, 1)", {}},
        {"prog_line p; Select p         such that Next*(p, _)",
         {"1", "2", "5", "6", "8", "9", "10", "11"}},
        {"prog_line p1, p2;    Select <p1, p2>  such that Next*(p1, p2)",
         {"1 2",  "1 3",  "1 4",   "2 3",   "2 4",  "5 6",  "5 7",   "6 7",
          "8 8",  "8 9",  "8 10",  "8 11",  "9 8",  "9 9",  "9 10",  "9 11",
          "10 8", "10 9", "10 10", "10 11", "11 8", "11 9", "11 10", "11 11"}},
        {"stmt s;      Select s         such that Next*(s, s)", {"8", "9", "10", "11"}},
        {"             Select BOOLEAN   such that Next*(5, _)", {"TRUE"}},
        {"             Select BOOLEAN   such that Next*(4, _)", {"FALSE"}},
        {"print p;     Select p.varName such that Next*(10, p)", {"V1", "P1"}},
        {"read r;      Select r.varName such that Next*(10, r)", {}},
        {"procedure p; Select p         such that Next*(10, 9)", {"P1", "P2", "P3"}},
        {"procedure p; Select p         such that Next*(3, 2)", {}},

        // ######################################
        // Queries with a single pattern clause
        // ######################################

        // assign
        {"assign a;             Select a.stmt# pattern a(_, _)", {"5", "7"}},
        {"assign a;             Select a       pattern a(_, \"(3 + 7) % 33 - 77\")", {"7"}},
        {"assign a;             Select BOOLEAN pattern a(_, \"P1 + 33 * 77\")", {"TRUE"}},
        {"assign a;             Select BOOLEAN pattern a(_, \"3 + 7\")", {"FALSE"}},
        {"assign a;             Select a       pattern a(_, _\"33\"_)", {"5", "7"}},
        {"assign a;             Select a       pattern a(_, _\"33 * 77\"_)", {"5"}},
        {"assign a;             Select a       pattern a(_, _\"P1 + 33\"_)", {}},
        {"assign a; variable v; Select <a, v>  pattern a(v, _)", {"5 P1", "7 V2"}},
        {"assign a; variable v; Select v       pattern a(v, \"(3 + 7) % 33 - 77\")", {"V2"}},
        {"assign a; variable v; Select v       pattern a(v, \"missing\")", {}},
        {"assign a; variable v; Select v       pattern a(v, _\"P1\"_)", {"P1"}},
        {"assign a; variable v; Select v       pattern a(v, _\"unknown\"_)", {}},
        {"assign a;             Select BOOLEAN pattern a(\"P1\", _)", {"TRUE"}},
        {"assign a;             Select BOOLEAN pattern a(\"V1\", _)", {"FALSE"}},
        {"assign a;             Select a       pattern a(\"V2\", _\"P1\"_)", {}},

        // while
        {"while w;             Select w       pattern w(_, _)", {"8"}},
        {"while w;             Select BOOLEAN pattern w(_, _)", {"TRUE"}},
        {"while w; variable v; Select <w, v>  pattern w(v, _)", {"8 V2"}},
        {"while w; variable v; Select BOOLEAN pattern w(v, _)", {"TRUE"}},
        {"while w;             Select w       pattern w(\"V2\", _)", {"8"}},
        {"while w;             Select w       pattern w(\"V1\", _)", {}},

        // if
        {"if i;             Select i       pattern i(_, _, _)", {"2"}},
        {"if i;             Select BOOLEAN pattern i(_, _, _)", {"TRUE"}},
        {"if i; variable v; Select <i, v>  pattern i(v, _, _)", {"2 V1", "2 V3"}},
        {"if i; variable v; Select BOOLEAN pattern i(v, _, _)", {"TRUE"}},
        {"if i;             Select i       pattern i(\"V1\", _, _)", {"2"}},
        {"if i;             Select i       pattern i(\"P1\", _, _)", {}},

        // ######################################
        // Queries with a single with clause
        // ######################################

        // integer = integer
        {"prog_line p1, p2;        Select <p1, p2>     with p1 = p2",
         {"1 1", "2 2", "3 3", "4 4", "5 5", "6 6", "7 7", "8 8", "9 9", "10 10", "11 11"}},
        {"prog_line p;             Select p            with p = p",
         {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"}},
        {"prog_line n; print p;    Select <n, p.varName> with n = p.stmt#",
         {"3 V2", "10 V1", "11 P1"}},
        {"prog_line p; assign a;   Select p            with p = a.stmt#", {"5", "7"}},
        {"prog_line p; constant c; Select c            with p = c.value", {"3", "7"}},
        {"prog_line p;             Select BOOLEAN      with p = 11", {"TRUE"}},
        {"prog_line p;             Select BOOLEAN      with p = 12", {"FALSE"}},
        {"stmt s; while w;         Select <s, w.stmt#> with s.stmt# = w.stmt#", {"8 8"}},
        {"if i; while w;           Select <i, w>       with i.stmt# = w.stmt#", {}},
        {"print p; constant c;     Select p.varName    with p.stmt# = c.value", {"V2"}},
        {"read r; constant c;      Select BOOLEAN      with r.stmt# = c.value", {"FALSE"}},
        {"read r;                  Select r.varName    with r.stmt# = 1", {"V1"}},
        {"read r;                  Select r.varName    with r.stmt# = 99", {}},
        {"constant c1, c2;         Select <c1, c2>     with c1.value = c2.value",
         {"3 3", "7 7", "33 33", "77 77"}},
        {"constant c;              Select c            with c.value = c.value",
         {"3", "7", "33", "77"}},
        {"constant c;              Select BOOLEAN      with c.value = 33", {"TRUE"}},
        {"constant c;              Select BOOLEAN      with c.value = 50", {"FALSE"}},
        {"procedure p;             Select p            with 7 = 7", {"P1", "P2", "P3"}},
        {"procedure p;             Select p            with 7 = 77", {}},

        // string = string
        {"variable v; read r;    Select r.stmt#   with v.varName = r.varName", {"1"}},
        {"print p; read r;       Select <p, r>    with p.varName = r.varName", {"10 1"}},
        {"print p; procedure pp; Select p.varName with p.varName = pp.procName", {"P1"}},
        {"read r; call c;        Select r         with r.varName = c.procName", {}},
        {"print p;               Select p         with p.varName = \"V2\"", {"3"}},
        {"read r;                Select BOOLEAN   with r.varName = \"V2\"", {"FALSE"}},
        {"procedure p; call c;   Select p         with p.procName = c.procName", {"P2", "P3"}},
        {"call c;                Select c         with c.procName = c.procName", {"4", "6"}},
        {"call c;                Select c.stmt#   with c.procName = \"P3\"", {"6"}},
        {"call c;                Select c.stmt#   with c.procName = \"P1\"", {}},
        {"procedure p;           Select p         with \"hello\" = \"hello\"", {"P1", "P2", "P3"}},
        {"procedure p;           Select p         with \"hello\" = \"bye\"", {}},

        // ######################################
        // Queries with multiple clauses
        // ######################################

        // single group
        {"stmt s1, s2, s3;"
         "Select <s1, s2, s3> such that Follows*(s1, s2) such that Follows(s2, s3)",
         {"5 6 7"}},
        {"stmt s1, s2, s3;"
         "Select <s1, s2, s3> such that Parent(s1, s2) and Parent*(s2, s3)",
         {"8 9 10", "8 9 11"}},
        {"stmt s1, s2; prog_line p;"
         "Select BOOLEAN such that Parent(s1, s2) with p = 11 such that Parent*(s2, p)",
         {"TRUE"}},
        {"stmt s1, s2; prog_line p;"
         "Select BOOLEAN such that Parent(s1, s2) with p = 8 such that Parent*(s2, p)",
         {"FALSE"}},
        {"while w; print p; variable v; prog_line n;"
         "Select v.varName with w.stmt# = n and n = 8 pattern w(v, _) such that Parent*(w, p) "
         "with p.varName = \"V1\"",
         {"V2"}},
        {"while w; print p; variable v; prog_line n;"
         "Select v.varName with w.stmt# = n and n = 9 pattern w(v, _) such that Parent*(w, p) "
         "with p.varName = \"V1\"",
         {}},
        {"while w; print p; variable v; prog_line n;"
         "Select v.varName with w.stmt# = n and n = 8 pattern w(v, _) such that Parent*(w, p) "
         "with p.varName = \"V3\"",
         {}},

        // multiple groups
        {"stmt s1, s2, s3; assign a; variable v;"
         "Select <s1, s2, s3> "
         "such that Follows*(s1, s2) and Follows(s2, s3) "
         "pattern a(v, _) with v.varName = \"V2\"",
         {"5 6 7"}},
        {"stmt s1, s2, s3; assign a; variable v;"
         "Select <s1, s2, s3> "
         "such that Follows*(s1, s2) and Follows(s2, s3) "
         "pattern a(v, _) with v.varName = \"V1\"",
         {}},
        {"assign a; variable v1, v2; if i; prog_line n; procedure p;"
         "Select a "
         "such that Uses(a, v1) and Modifies(a, v1) pattern a(_, _\"33 * 77\"_) "
         "such that Parent*(i, n) and Uses(n, v2) with v2.varName = p.procName",
         {"5"}},
        {"assign a; variable v1, v2; if i; prog_line n; procedure p;"
         "Select a "
         "such that Uses(a, v1) and Modifies(a, v1) pattern a(_, _\"33 * 77\"_) "
         "such that Parent*(i, n) and Uses(n, v2) with v2.varName = \"unknown\"",
         {}},
        {"stmt s1, s2, s3, s4, s5; prog_line p1, p2, p3, p4, p5;"
         "Select BOOLEAN "
         "such that Follows(s1, p1) and Parent(s2, p2) and Parent*(s3, p3) "
         "and Next(s4, p4) and Next*(s5, p5)",
         {"TRUE"}},
        {"stmt s1, s2, s3, s4, s5; prog_line p1, p2, p3, p4, p5;"
         "Select BOOLEAN "
         "such that Follows(s1, p1) and Parent(s2, p2) and Parent*(s3, p3) "
         "and Next(s4, p4) and Next*(s5, p5) with s4.stmt# = p4",
         {"FALSE"}},
    }));

    auto query = std::get<0>(fixture);
    auto expected = std::get<1>(fixture);
    auto output = evaluate(query);

    std::vector<std::string> v_output(output.begin(), output.end());
    std::vector<std::string> v_expected(expected.begin(), expected.end());
    std::sort(v_output.begin(), v_output.end());
    std::sort(v_expected.begin(), v_expected.end());

    REQUIRE(v_output == v_expected);
}
