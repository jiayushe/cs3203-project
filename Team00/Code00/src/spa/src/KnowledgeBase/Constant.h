#pragma once

namespace KnowledgeBase {

class Constant {
public:
    Constant(int id, int value);

    ~Constant();

    // Gets the id of the constant.
    int get_id();

    // Gets the value of the constant.
    int get_value();

private:
    int id;
    int value;
};

} // namespace KnowledgeBase
