#include "interpreter.h"
#include <QDebug>
#include <sstream>
#include "tokenizer.h"
#include "parser.h"

namespace EBasic {

void Interpreter::run() {
    state.clear();
    state.start();
    // run the program line by line
    auto cur_line = prog.begin();
    qDebug() << "Starting from" << cur_line->first;
    while(state.isRunning() && cur_line != prog.end()) {
        cur_line->second->exec(*this, state);
        if(state.hasJumped())
            cur_line = prog.find(state.getJumpDest());
        else
            ++cur_line;
        state.proceed(cur_line->first);
    }
    if(state.isRunning())
        qDebug() << "Warning: program reached EOF instead of END statement.";
    state.end();
    output("[Program exited]");
}

void Interpreter::step() {
    if(!state.isRunning()) {
        state.clear();
        state.start();
    }

    unsigned prev_line = state.getLine();
    auto cur_line = prog.find(prev_line);
    if(cur_line == prog.end())
        throw std::runtime_error("Error: target location invalid.");
    cur_line->second->exec(*this, state);
    state.proceed(state.hasJumped() ?
                      state.getJumpDest() :(++cur_line)->first);
}

void Interpreter::interp(const std::string &buf)
{
    // take one line of input and interpret it
    if(buf.size() == 0)
        return;
    Line l = Tokenizer::tokenize(buf);
    if(l.no == 0) {
        // this is a control keyword
        switch(l.tokens[0]->type) {
        case Token::RUN:
            run();
        break;
        case Token::LIST:
            listCommands();
        break;
        case Token::VARS:
            listVariables();
        break;
        case Token::CLEAR:
            state.clear();
        break;
        case Token::HELP:
            printHelp();
        break;
        case Token::QUIT:
            setQuit();
        break;
        default:
            std::runtime_error("Invalid control variable");
        }
    }
    else {
        // a new line
        Statement *stmt = Parser::parseOne(l, buf);
        insert(l.no, stmt);
    }
}

void Interpreter::listCommands()
{
    output("----- commands -----");
    for(auto i = prog.begin(); i != prog.end(); ++i) {
        std::ostringstream buf;
        buf << (i->second->orig);
        output(buf.str());
    }
    output("--- end commands ---");
}

void Interpreter::listVariables()
{
    output("----- variables -----");
    const std::map<std::string, int> &vars = state.getVars();
    for(auto i = vars.begin(); i != vars.end(); ++i) {
        output(i->first + "\t\t" + std::to_string(i->second));
    }
    output("--- end variables ---");
}

void Interpreter::printHelp()
{
    output("------- Help -------");
    output("Basic interpreter");
    output("Author Eugene Chen");
    output("--------------------");

}

void Interpreter::setQuit()
{
    exit(0);
}

}
