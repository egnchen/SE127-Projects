#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "pstate.h"
#include "tokens.h"
#include <string>

namespace EBasic {


class Expression
{
public:
    Expression();
    enum Kind {
        CONSTANT, IDENTIFIER, COMPOUND
    };
    const Kind kind;
    Expression(Kind k):kind(k) {}
    virtual ~Expression() {}
    virtual int eval(PState &) const { throw std::logic_error("Not implemented."); }
    virtual const std::string &getName() const { throw std::logic_error("Not implemented."); }
};

class ConstantExp: public Expression {
private:
    const int v;
public:
    ConstantExp(int v):
        Expression(CONSTANT), v(v) {}
    ~ConstantExp() {}
    int eval(PState &) const { return v; }
};

class IdentifierExp: public Expression {
private:
    const std::string idt;
public:
    IdentifierExp(std::string idt):
        Expression(IDENTIFIER), idt(idt) {}
    ~IdentifierExp() {}
    int eval(PState &s) const { return s.getOne(idt); }
    const std::string &getName() const { return idt; }
};

class CompoundExp: public Expression {
private:
    Expression *lhs;
    Token::Type type;
    Expression *rhs;
public:
    CompoundExp(Expression *lhs, Token::Type type, Expression *rhs):
        Expression(COMPOUND), lhs(lhs), type(type), rhs(rhs) {}
    int eval (PState &s) const;
    Token::Type getType() const { return type; }
};
}

#endif // EXPRESSION_H
