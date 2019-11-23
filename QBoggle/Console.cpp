#include "Console.h"
#include <QKeyEvent>
#include <QTextLine>
#include <QTextCursor>

const QList<QString> Console::prompts = {
    "Welcome to the game of Boggle!"
};

const QList<QString> Console::commands = {
    ".shuffle",
    ".search",
    ".finish",
    ".help"
};

const QList<QString> Console::descriptions = {
    "Shuffle the board",
    "Automatically search for results",
    "Finish entering words",
    "Print this help message"
};

const QList<QString> Console::instructions = {
    "Enter word here directly to give new words.",
    "Left click on the board to select character.",
    "Right click on the board to clear selection."
};

Console::Console(QWidget *parent) : QTextEdit(parent)
{
}

void Console::getCommand(QString command) {
    if (command == ".help")
        this->welcome();
}

void Console::clear()
{
    this->clear();
}

void Console::welcome() {
    for (QString p: prompts)
        this->write(p);

    this->write("Useful commands:");
    for (int i = 0; i < commands.length(); i++) {
        this->write(commands[i] + '\t' + descriptions[i]);
    }

    for (QString p: instructions)
        this->write(p);

    this->write("Enjoy!");
    this->write("");    // to start a new line here
}

void Console::write(QString msg)
{
    this->append(msg);
}

void Console::keyPressEvent(QKeyEvent *event)
{
    QTextCursor cursor = this->textCursor();
    if (event->key() == Qt::Key_Backspace ||
            event->key() == Qt::Key_Delete) {
        if(cursor.columnNumber() == 0)
            return;
    }
    if (this->textCursor().hasSelection())
        return;
    if (event->key() == Qt::Key_Return) {
        cursor.movePosition(QTextCursor::End);
        cursor.select(QTextCursor::LineUnderCursor);
        QString lastLine = cursor.selectedText();
        if(lastLine.startsWith('.')) {
            if(commands.contains(lastLine))
                newCommand(lastLine);
            else
                write("Invalid command");
        } else
            newLine(lastLine);
    }
    QTextEdit::keyPressEvent(event);
}
