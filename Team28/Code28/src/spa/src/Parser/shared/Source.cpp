#include "Source.h"
#include <string>
#include <sstream>
#include <fstream>

using namespace Parser;

Source::Source(const std::string& value, bool is_filename) : pos(0) {
    if (is_filename) {
        std::ifstream inFile;
        inFile.open(value); //open the input file

        std::stringstream strStream;
        strStream << inFile.rdbuf(); //read the file
        std::string query_str = strStream.str(); //str holds the content of the file
        this->value = query_str + "$";
    } else {
        this->value = value + "$";
    }
}


std::string Source::pop_string(int n) {
    std::string str = peek_string(n);
    pos += n;
    return str;
}

std::string Source::peek_string(int n) {
    if (!has_more(n)) {
        throw std::runtime_error("Not enough characters left");
    }
    std::string str;
    int tmp_pos = pos;
    while (n > 0) {
        str += value[tmp_pos];
        tmp_pos += 1;
        n--;
    }
    return str;
}

bool Source::has_more() { return pos < value.size() - 1; }

bool Source::has_more(int n) { return pos + n - 2 < value.size(); }

int Source::pop_char() {
    int result = peek_char();
    pos += 1;
    return result;
}

int Source::peek_char() {
    if (!has_more()) {
        throw std::runtime_error("String is empty");
    }
    return value[pos];
}

int Source::current_pos() {
    return pos;
}

void Source::reset_pos(int new_pos) {
    if (new_pos < 0 || new_pos >= value.size()) {
        throw std::runtime_error("Invalid position supplied");
    }
    pos = new_pos;
}
