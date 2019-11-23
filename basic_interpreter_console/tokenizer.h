#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <queue>
#include <map>
#include <vector>
#include "tokens.h"

namespace EBasic {

class Tokenizer {
public:

private:
    static const std::map<std::string, Token> staticTokens;

public:
    Tokenizer() {}
    static Line tokenize(const std::string &);
};

}

#endif // TOKENIZER_H
