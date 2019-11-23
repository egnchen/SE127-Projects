//#include <QCoreApplication>
#include "interpreter.h"
#include "parser.h"
#include "tokenizer.h"
#include <QDebug>

#include <iostream>

using namespace EBasic;

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
//    return a.exec();
    PState state;
    Interpreter_stdio interp(state);

    // REPL
    std::string buf;
    while(!std::cin.eof()) {
        std::getline(std::cin, buf);
        interp.interp(buf);
    }
}
