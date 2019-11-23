#include "parser.h"
#include <QDebug>

/*
 * parser
 * build ast & check grammar errors
 * flow executable into program
 */

namespace EBasic {

unsigned char Parser::getPriority(const Token::Type type)
{
    switch(type) {
    case Token::TIMES: case Token::DIVIDE:
        return 3;
    case Token::PLUS: case Token::MINUS:
        return 2;
    case Token::EQUAL: case Token::GT: case Token::LT:
        return 1;
    case Token::LBRACE:
        // do not ever pop brace
        return 0;
    default:
        throw std::runtime_error("Invalid operator");
    }
}

void Parser::popOne(
        std::stack<Expression *> &exp_stack,
        std::stack<Token::Type> &op_stack)
{
    if(op_stack.empty() || exp_stack.size() < 2)
        throw std::runtime_error("no enough elements to pop");
    Expression *e1, *e2;
    e1 = exp_stack.top();
    exp_stack.pop();
    e2 = exp_stack.top();
    exp_stack.pop();
    exp_stack.push(new CompoundExp(e2, op_stack.top(), e1));
    op_stack.pop();
}

Expression *Parser::consumeExp(const Line &stmt, unsigned int &offset)
{
    // consume an expression
    std::stack<Expression *> exp_stack;
    std::stack<Token::Type> op_stack;

    while(offset < stmt.tokens.size()) {
        const Token *token = stmt.tokens[offset];
        if(token->type == Token::RBRACE) {
            while(!op_stack.empty() && op_stack.top() != Token::LBRACE)
                popOne(exp_stack, op_stack);
            if(op_stack.empty() || op_stack.top() != Token::LBRACE)
                throw std::runtime_error("missing LBRACE");
            op_stack.pop();
        }
        else if(token->type == Token::LBRACE)
            op_stack.push(token->type);
        else if(token->isOperator()) {
            while(!op_stack.empty() &&
                      getPriority(token->type) <= getPriority(op_stack.top()))
                popOne(exp_stack, op_stack);
            op_stack.push(token->type);
        }
        else if(token->type == Token::CONSTANT)
            exp_stack.push(new ConstantExp(static_cast<const ConstantToken *>(token)->value));
        else if(token->type == Token::IDENTIFIER)
            exp_stack.push(new IdentifierExp(static_cast<const IdentifierToken *>(token)->value));
        else {
            break;
        }
        offset++;
    }
    // pop the exp stack until there's no operator left
    while(!op_stack.empty())
        popOne(exp_stack, op_stack);
    if(!(exp_stack.size() == 1 && op_stack.empty()))
        throw std::runtime_error("Failed to clear stacks at end of parsing.");
    return exp_stack.top();
}


const Token *Parser::consume(const Line &stmt, Token::Type type, unsigned int &offset)
{
    if(stmt.tokens.size() <= offset)
        throw std::runtime_error("Expecting token at end of statement.");
    if(stmt.tokens[offset]->type != type)
        throw std::runtime_error("Got wrong token @ offset %d");
    return stmt.tokens[offset++];
}

Expression *Parser::consumeBoolExp(const Line &stmt, unsigned int &offset)
{
    Expression *ret = consumeExp(stmt, offset);
    if(ret->kind != Expression::COMPOUND)
        throw std::runtime_error("Expecting a compound expression in if statement.");
    Token::Type type = static_cast<CompoundExp *>(ret)->getType();
    if(type == Token::GT || type == Token::LT || type == Token::EQUAL)
        return ret;
    else
        throw std::runtime_error("Expecting a compound statement with binary value.");
}

IdentifierExp *Parser::consumeIdentifierExp(const Line &stmt, unsigned int &offset)
{
    auto tok = static_cast<const IdentifierToken *>(consume(stmt, Token::IDENTIFIER, offset));
    return new IdentifierExp(tok->value);
}

Statement *Parser::parseOne(Line stmt, const std::string &orig)
{
    // parse it
    unsigned offset = 0;
    Statement *ret = nullptr;
    try {
        if(!stmt.tokens[offset]->isStmtToken())
            throw std::runtime_error("Start of statement is not a statement token.");
        switch(stmt.tokens[offset]->type) {
        case Token::REM:
            return new RemStmt(orig);
        case Token::LET:
        {
            consume(stmt, Token::LET, offset);
            auto lhs = consumeIdentifierExp(stmt, offset);
            consume(stmt, Token::EQUAL, offset);
            auto rhs = consumeExp(stmt, offset);
            ret = new LetStmt(lhs, rhs, orig);
        }
        break;
        case Token::PRINT:
            consume(stmt, Token::PRINT, offset);
            ret = new PrintStmt(consumeExp(stmt, offset), orig);
        break;
        case Token::INPUT:
            consume(stmt, Token::INPUT, offset);
            ret = new InputStmt(consumeIdentifierExp(stmt, offset), orig);
        break;
        case Token::GOTO:
            consume(stmt, Token::GOTO, offset);
            ret = new GotoStmt(consumeExp(stmt, offset), orig);
        break;
        case Token::IF:
        {
            consume(stmt, Token::IF, offset);
            // have to be a boolean value here
            auto cond = consumeBoolExp(stmt, offset);
            consume(stmt, Token::THEN, offset);
            auto dest = consumeExp(stmt, offset);
            ret = new IfStmt(cond, dest, orig);
        }
        break;
        case Token::END:
            // consume nothing
            consume(stmt, Token::END, offset);
            ret = new EndStmt(orig);
        break;
        default:
            // others
            throw std::runtime_error("Invalid start of statement.");
        }
    }
    catch (std::runtime_error &err) {
        qDebug() << "Runtime error when parsing line" << stmt.no;
        qDebug() << "Cause: " << err.what();
    }
    // parse complete, check if there's any token left
    if(offset < stmt.tokens.size())
        qDebug() << "Warning: Parsing did not reach end of line.";
    return ret;
}


}
