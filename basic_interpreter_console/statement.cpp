#include "statement.h"

#include <iostream>

namespace EBasic {

void RemStmt::exec(Interpreter &, PState &) {
    // do nothing
}

void LetStmt::exec(Interpreter &, PState &s) {
    // assign the value
    s.setOne(lhs->getName(), rhs->eval(s));
}

void PrintStmt::exec(Interpreter &inter, PState &s) {
    // print out the value
    inter.output(std::to_string(rhs->eval(s)));
}

void InputStmt::exec(Interpreter &inter, PState &s) {
    // input from console
    inter.output(rhs->getName() + ">>", '>');
    std::string buf = inter.input();

    char *p;
    int v = std::strtol(buf.c_str(), &p, 10);
    if(p - buf.c_str() == 0)
        throw std::runtime_error("Invalid input");
    s.setOne(rhs->getName(), v);
}

void GotoStmt::exec(Interpreter &, PState &s) {
    s.jump(static_cast<unsigned>(rhs->eval(s)));
}

void IfStmt::exec(Interpreter &, PState &s) {
    if(test->eval(s))
        s.jump(static_cast<unsigned>(dst->eval(s)));
}

void EndStmt::exec(Interpreter &, PState &s) {
    // TODO end the program
    s.end();
}

}
