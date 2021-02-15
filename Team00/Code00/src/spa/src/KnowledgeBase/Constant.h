#pragma once

namespace KnowledgeBase {

class Constant {
public:
    Constant();

    Constant(int value);

    ~Constant();

    // Gets the value of the constant.
    int get_value();

private:
    int value;
};

} // namespace KnowledgeBase
