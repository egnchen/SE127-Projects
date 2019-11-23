#ifndef TOKENS_H
#define TOKENS_H

#include <string>
#include <vector>
#include <map>

namespace EBasic {

class Token {
public:
    enum Type {
        NONE = 0,
        // comment token
        REM = 1,
        // statement tokens
        LET = 16, PRINT, INPUT, GOTO, IF, THEN, END,
        // control tokens
        // operators
        PLUS = 32, MINUS, TIMES, DIVIDE, EXPONENT,
        EQUAL, LBRACE, RBRACE, LT, GT,
        // tokens with actual values
        IDENTIFIER = 64, CONSTANT,
        // control keywords
        RUN = 128, LIST, VARS, CLEAR, HELP, QUIT
    };

    Type type;
    Token(Type type): type(type) {}
    bool isStmtToken() const { return type & 16 || type == 1; }
    bool isOperator() const { return type & 32; }
    bool isControl() const { return type & 128; }
};

class IdentifierToken: public Token {
public:
    std::string value;
    IdentifierToken(std::string v): Token(IDENTIFIER), value(v) {}
};

class ConstantToken: public Token {
public:
    int value;
    ConstantToken(int v): Token(CONSTANT), value(v) {}
};

struct Line {
    unsigned no;
    std::vector<const Token *> tokens;
    bool operator<(const Line &rhs) const { return this->no < rhs.no; }
};

}

#endif // TOKENS_H
