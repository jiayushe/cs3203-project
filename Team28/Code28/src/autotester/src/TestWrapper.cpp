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
        auto source = std::make_shared<Parser::Source>(filename, true);
        Parser::SimpleParser parser(source);
        auto root_node = parser.parse_program();

        pkb = std::make_shared<KnowledgeBase::PKB>(root_node);
        SimpleExtractor::DesignExtractor::extract_cfg(pkb);
        SimpleExtractor::DesignExtractor::extract_next_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_follow_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_parent_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_call_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_modify_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_use_relationship(pkb);
        SimpleExtractor::DesignExtractor::extract_affect_relationship(pkb);
        // SimpleExtractor::DesignExtractor::extract_next_bip_relationship(pkb);
        // SimpleExtractor::DesignExtractor::extract_affect_bip_relationship(pkb);
    } catch (std::runtime_error& error) {
        std::cout << "Error: " << error.what() << std::endl;
        exit(1);
    }
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
    Parser::QueryObject query_object;
    try {
        auto source = std::make_shared<Parser::Source>(query);
        Parser::PQLParser parser(source);
        parser.parse_query(query_object);

        QueryEvaluator::Evaluator::evaluate(pkb, query_object, results);
    } catch (std::runtime_error& error) {
        std::cout << "Error: " << error.what() << std::endl;

        switch (query_object.get_result().get_type()) {
        case Parser::ResultType::BOOLEAN:
            results.emplace_back("FALSE");
            break;
        case Parser::ResultType::TUPLE:
        case Parser::ResultType::INVALID:
            results.clear();
            break;
        default:
            throw std::runtime_error("Unhandled result type");
        }
    }
}
