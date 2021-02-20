#include "Constant.h"

using namespace KnowledgeBase;

Constant::Constant() : value(-1) {}

Constant::Constant(int value) : value(value) {}

Constant::~Constant() {}

int Constant::get_value() { return value; }
