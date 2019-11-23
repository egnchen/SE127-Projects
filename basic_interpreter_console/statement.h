#ifndef STATEMENT_H
#define STATEMENT_H

#include "pstate.h"
#include "expression.h"
#include "interpreter.h"

namespace EBasic {

class Interpreter;

class Statement
{
public:
    enum Kind {
        REM, LET, PRINT, INPUT, GOTO, IF, END
    };
    const Kind kind;
    const unsigned no = 0;
    const std::string orig;
    Statement(Kind kind, const std::string &orig): kind(kind), orig(orig) {}
    virtual ~Statement() {}
    virtual void exec(Interpreter &, PState &) = 0;
        //{ throw std::logic_error("Not implemented!"); }
};

class RemStmt: public Statement {
public:
    RemStmt(const std::string orig): Statement(REM, orig) {}
    ~RemStmt() {}
    void exec(Interpreter &, PState &);
};

class LetStmt: public Statement {
    const IdentifierExp *lhs;
    const Expression *rhs;
public:
    LetStmt(const IdentifierExp *lhs, const Expression *rhs, const std::string &orig): Statement(LET, orig), lhs(lhs), rhs(rhs) {}
    ~LetStmt() {}
    void exec(Interpreter &, PState &);
};

class PrintStmt: public Statement {
    const Expression *rhs;
public:
    PrintStmt(const Expression *rhs, const std::string orig): Statement(PRINT, orig), rhs(rhs) {}
    ~PrintStmt() {}
    void exec(Interpreter &, PState &);
};

class InputStmt: public Statement {
    const IdentifierExp *rhs;
public:
    InputStmt(const IdentifierExp *rhs, const std::string orig): Statement(INPUT, orig), rhs(rhs) {}
    ~InputStmt() {}
    void exec(Interpreter &, PState &);
};

class GotoStmt: public Statement {
    const Expression *rhs;
public:
    GotoStmt(const Expression *rhs, const std::string orig): Statement(GOTO, orig), rhs(rhs) {}
    ~GotoStmt() {}
    void exec(Interpreter &, PState &s);
};

class IfStmt: public Statement {
    const Expression *test;
    const Expression *dst;
public:
    IfStmt(const Expression *test, const Expression *dst, const std::string orig): Statement(IF, orig), test(test), dst(dst) {}
    ~IfStmt() {}
    void exec(Interpreter &, PState &s);
};

class EndStmt: public Statement {
public:
    EndStmt(const std::string orig): Statement(END, orig) {}
    ~EndStmt() {}
    void exec(Interpreter &, PState &s);
};

}
#endif // STATEMENT_H
