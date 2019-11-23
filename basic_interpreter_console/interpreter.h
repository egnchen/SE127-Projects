#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>
#include <string>
#include <queue>
#include <iostream>
#include "parser.h"
#include "statement.h"
#include "pstate.h"

namespace EBasic {

// this class handles
// control & io routines

class Statement;
class Parser;

class Interpreter
{
private:
    std::map<unsigned, Statement *> prog;
    PState &state;
public:
    Interpreter(PState &state): state(state) {}
    virtual ~Interpreter() {}
    void insert(unsigned no, Statement *s) { prog[no] = s; }
    void step();
    void run();
    void interp(const std::string &);

    virtual void listVariables();
    virtual void listCommands();
    virtual void printHelp();
    virtual void setQuit();

    virtual std::string input() = 0;
    virtual void output(std::string buf, char end='\n') = 0;
};

class Interpreter_stdio: public Interpreter
{
public:
    Interpreter_stdio(PState &state): Interpreter(state) {}
    ~Interpreter_stdio() {}
    std::string input() {
        std::string buf;
        std::getline(std::cin, buf);
        return buf;
    }

    void output(std::string buf, char end='\n') {
        std::cout << buf << end;
    }
};

}
#endif // PROGRAM_H
