#include "FileLexer.h"

using namespace Parser;

FileLexer::FileLexer(const std::string& filename) : file(std::ifstream(filename)) {}

bool FileLexer::has_more() { return file.peek() != EOF; }

int FileLexer::pop_char() { return file.get(); }

int FileLexer::peek_char() { return file.peek(); }
