#include "BoggleWindow.h"
#include "lexicon.h"
#include "EnglishDictionary.h"

#include <QFile>
#include <QHBoxLayout>
#include <QTextEdit>
#include <iostream>
#include <QWidget>

BoggleWindow::BoggleWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("QBoggle!");
    setFixedSize(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);

    me = new WordListWidget(this, "Me");
    computer = new WordListWidget(this, "Computer");
    board = new Board(this);
    console = new Console(this);

    me->setGeometry(20, 20, 230, 300);
    board->setGeometry(230, 0, 300, 300);
    computer->setGeometry(800 - 50 - 200, 20, 230, 300);
    console->setGeometry(30, 320, 740, 260);

    console->welcome();
    /* connect signals and slots */
    connect(this->console, SIGNAL(newCommand(QString)),
            this->board, SLOT(getCommand(QString)));
    connect(this->console, SIGNAL(newCommand(QString)),
            this, SLOT(getCommand(QString)));
    connect(this->console, SIGNAL(newCommand(QString)),
            this->console, SLOT(getCommand(QString)));
    connect(this->console, SIGNAL(newLine(QString)),
            this, SLOT(newWord(QString)));
    connect(this->board, SIGNAL(newWord(QString)),
            this, SLOT(newWord(QString)));
    // prompting in console
    connect(this->me, SIGNAL(prompt(QString)),
            this->console, SLOT(write(QString)));
    connect(this, SIGNAL(prompt(QString)),
            this->console, SLOT(write(QString)));
    connect(this->board, SIGNAL(prompt(QString)),
            this->console, SLOT(write(QString)));
}

void BoggleWindow::newWord(QString word) {
    if(word.length() < 4) {
        emit prompt("Word too short!");
        return;
    }
    if(me->contains(word)) {
        emit prompt(QString("[%1]\tWord already in your list.").arg(word));
        return;
    }
    int bSize = this->board->getBoardSize();
    bool *vis = new bool[bSize * bSize];
    if(!EnglishDictionary::getInstance().contains(word.toStdString()))
        emit prompt(QString("[%1]\tCannot find the word in the dictionary.").arg(word));
    else if(!EnglishDictionary::getInstance().containsPrefix(word.toStdString()))
        emit prompt(QString("[%1]\tDon't have any valid word with this prefix.").arg(word));
    else if(!board->contains(word, vis))
        emit prompt(QString("[%1]\tCannot find the word on the board.").arg(word));
    else {
        emit prompt(QString("[%1]\tValid word.").arg(word));
        // add highlight
        me->addWord(word);
        board->highlight(vis);
    }
    delete[] vis;
}

void BoggleWindow::getCommand(QString command) {
    if(command == ".finish") {
        // finish entering new words, computer's turn
        QSet<QString> words = board->searchAll(me->getWords());
        for(QString w: words)
            computer->addWord(w);
        emit prompt(QString("The computer found %1 words!").arg(words.size()));
        emit prompt("Use .shuffle to start another round.");
    }
    else if(command == ".shuffle") {
        this->me->reset();
        this->computer->reset();
        this->board->shake();
        emit prompt("<p style='color: red'>Game restarted</p>");
    }
}

BoggleWindow::~BoggleWindow()
{
}
