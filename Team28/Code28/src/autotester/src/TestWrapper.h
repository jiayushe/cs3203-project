#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <iostream>
#include <list>
#include <memory>
#include <string>

// include your other headers here
#include "AbstractWrapper.h"
#include "KnowledgeBase/PKB.h"
#include "Parser/PQLParser/PQLParser.h"
#include "Parser/PQLParser/PQLStringLexer.h"
#include "Parser/PQLParser/QueryObject.h"
#include "Parser/SimpleParser/SimpleFileLexer.h"
#include "Parser/SimpleParser/SimpleParser.h"
#include "Parser/SimpleParser/SimpleStringLexer.h"
#include "QueryEvaluator/BruteForceEvaluator.h"
#include "SimpleExtractor/DesignExtractor.h"

class TestWrapper : public AbstractWrapper {
public:
    // default constructor
    TestWrapper();

    // destructor
    ~TestWrapper();

    // method for parsing the SIMPLE source
    virtual void parse(std::string filename);

    // method for evaluating a query
    virtual void evaluate(std::string query, std::list<std::string>& results);

private:
    std::shared_ptr<KnowledgeBase::PKB> pkb;
};

#endif
