#include "Constant.h"

using namespace KnowledgeBase;

Constant::Constant(int value) : value(value) {}

Constant::~Constant() {}

int Constant::get_value() { return value; }
