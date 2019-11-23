#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"
#include "expression.h"
#include "statement.h"
#include <functional>
#include <stack>

namespace EBasic {

class Statement;
struct Line;

class Parser
{
private:
    static unsigned char getPriority(const Token::Type type);
    static void popOne(
            std::stack<Expression *> &,
            std::stack<Token::Type> &);
public:
    static const Token *consume(const Line &stmt, Token::Type type, unsigned &offset);
    static Expression *consumeExp(const Line &stmt, unsigned &offset);
    static Expression *consumeBoolExp(const Line &stmt, unsigned &offset);
    static IdentifierExp *consumeIdentifierExp(const Line &stmt, unsigned &offset);
    static Statement *parseOne(Line stmt, const std::string &orig);
};

}

#endif // PARSER_H
