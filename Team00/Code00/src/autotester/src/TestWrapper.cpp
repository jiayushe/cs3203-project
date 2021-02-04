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
TestWrapper::TestWrapper() {
    // create any objects here as instance variables of this class
    // as well as any initialization required for your spa program
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
    try {
        Parser::FileLexer lexer(filename);
        Parser::SimpleParser parser(lexer);
        auto root_node = parser.parse_program();

        // TODO: PKB logic here...
        // FIXME: For now, simply print out the AST parsed
        std::cout << root_node->to_string();

        delete root_node;
    } catch (char const* message) {
        std::cout << "Error: " << message << std::endl;
        throw message;
    } catch (const std::string& message) {
        std::cout << "Error: " << message << std::endl;
        throw message;
    }
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
    // call your evaluator to evaluate the query here
    // ...code to evaluate query...

    // store the answers to the query in the results list (it is initially empty)
    // each result must be a string.
}
