#include "TestWrapper.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;

AbstractWrapper* WrapperFactory::createWrapper() {
    if (wrapper == 0)
        wrapper = new TestWrapper;
    return wrapper;
}

// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
    try {
        Parser::SimpleFileLexer lexer(filename);
        Parser::SimpleParser parser(lexer);
        auto root_node = parser.parse_program();

        pkb = std::make_shared<KnowledgeBase::PKB>(root_node);
        SimpleExtractor::DesignExtractor::extract_follow_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_parent_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_modify_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_use_relationship(pkb);
    } catch (std::runtime_error& error) {
        std::cout << "Error: " << error.what() << std::endl;
        throw error;
    }
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
    try {
        Parser::PQLStringLexer lexer(query);
        Parser::PQLParser parser(lexer);
        auto query_object = parser.parse_query();

        QueryEvaluator::BruteForceEvaluator::evaluate(pkb, query_object, results);
    } catch (std::runtime_error& error) {
        std::cout << "Error: " << error.what() << std::endl;
        results.clear();
    }
}
