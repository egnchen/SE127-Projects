#include "tokenizer.h"
#include <sstream>
#include <cctype>
#include <cassert>
#include <QDebug>

/*
 * tokenizer
 * convert string into stream of tokens
 * line by line
 */

namespace EBasic {

const std::map<std::string, Token>
    Tokenizer::staticTokens = {
    {"REM", Token(Token::Type::REM)},
    {"LET", Token(Token::Type::LET)},
    {"PRINT", Token(Token::Type::PRINT)},
    {"INPUT", Token(Token::Type::INPUT)},
    {"GOTO", Token(Token::Type::GOTO)},
    {"IF", Token(Token::Type::IF)},
    {"THEN", Token(Token::  Type::THEN)},
    {"END", Token(Token::Type::END)},
    {"+", Token(Token::Type::PLUS)},
    {"-", Token(Token::Type::MINUS)},
    {"*", Token(Token::Type::TIMES)},
    {"/", Token(Token::Type::DIVIDE)},
    {"=", Token(Token::Type::EQUAL)},
    {">", Token(Token::Type::GT)},
    {"<", Token(Token::Type::LT)},
    {"(", Token(Token::Type::LBRACE)},
    {")", Token(Token::Type::RBRACE)},
    {")", Token(Token::Type::RBRACE)},
    {"RUN", Token(Token::Type::RUN)},
    {"LIST", Token(Token::Type::LIST)},
    {"VARS", Token(Token::Type::VARS)},
    {"CLEAR", Token(Token::Type::CLEAR)},
    {"HELP", Token(Token::Type::HELP)},
    {"QUIT", Token(Token::Type::QUIT)}
};

Line Tokenizer::tokenize(const std::string &stmt) {
    std::istringstream iss(stmt);
    Line cur_stmt;
    if(stmt.length() == 0)
        return cur_stmt;

    if(isalpha(iss.peek())) {
        // should be a control keyword
        std::string cur;
        iss >> cur;
        auto controlToken = staticTokens.find(cur);
        if(controlToken == staticTokens.end() ||
                !controlToken->second.isControl()) {
            // not a control keyword
            throw std::runtime_error("Failed to parse line, invalid start.");
        }
        cur_stmt.tokens.push_back(&(controlToken->second));
        cur_stmt.no = 0;
        return cur_stmt;
    }

    // should be a statement elsewise
    iss>>cur_stmt.no;

    while(!iss.eof()) {
        std::string cur;
        // one space separated token at a time
        iss>>cur;
        // find it in constant map
        auto singleToken = staticTokens.find(cur);
        if(singleToken != staticTokens.end()) {
            cur_stmt.tokens.push_back(&(singleToken->second));
            if(singleToken->second.type == Token::Type::REM)
                // ignore all following tokens
                break;
        } else {
            // dynamic
            if(isdigit(cur[0])) {
                // can only be number, validate that
                char *end = nullptr;
                unsigned ret = std::strtol(cur.c_str(), &end, 10);
                if(end == cur.c_str()) {
                    // parse error
                    throw std::invalid_argument("Failed to parse integer.");
                } else {
                    cur_stmt.tokens.push_back(new ConstantToken(ret));
                }
            } else if(isalpha(cur[0]) || cur[0] == '_') {
                // can only be identifier
                cur_stmt.tokens.push_back(new IdentifierToken(cur));
            }
        }
    }

    return cur_stmt;
}

}
