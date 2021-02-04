#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <iostream>
#include <list>
#include <string>

// include your other headers here
#include "AbstractWrapper.h"
#include "Parser/SimpleParser/SimpleParser.h"
#include "Parser/shared/FileLexer.h"

#include "PQLParser/QueryObject.h"
#include "PQLParser/Parser.h"

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
};

#endif
