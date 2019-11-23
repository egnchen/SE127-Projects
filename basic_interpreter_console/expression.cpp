#include "expression.h"

namespace EBasic {

int CompoundExp::eval(PState &s) const {
    switch(type) {
    case Token::PLUS:   return lhs->eval(s) + rhs->eval(s);
    case Token::MINUS:  return lhs->eval(s) - rhs->eval(s);
    case Token::TIMES:  return lhs->eval(s) * rhs->eval(s);
    case Token::DIVIDE: return lhs->eval(s) / rhs->eval(s);
    case Token::EQUAL:  return lhs->eval(s) == rhs->eval(s);
    case Token::GT:     return lhs->eval(s) > rhs->eval(s);
    case Token::LT:     return lhs->eval(s) < rhs->eval(s);
    default: throw std::logic_error("Operation not implemented.");
    }
}

}
