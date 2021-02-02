#include "Constant.h"

using namespace KnowledgeBase;

Constant::Constant(int id, int value) : id(id), value(value) {}

Constant::~Constant() {}

int Constant::get_id() { return id; }

int Constant::get_value() { return value; }
