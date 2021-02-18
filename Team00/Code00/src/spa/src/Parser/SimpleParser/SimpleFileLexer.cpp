#include "SimpleFileLexer.h"
#include <string>

using namespace Parser;

SimpleFileLexer::SimpleFileLexer(const std::string& filename) : file(std::ifstream(filename)) {}

bool SimpleFileLexer::has_more() { return file.peek() != EOF; }

int SimpleFileLexer::pop_char() { return file.get(); }

int SimpleFileLexer::peek_char() { return file.peek(); }