#include "WordListWidget.h"
#include "WordTable.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QDebug>
#include "EnglishDictionary.h"

WordListWidget::WordListWidget(QWidget *parent, QString label) : QWidget(parent)
{

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *headLayout = new QHBoxLayout();
    nameLabel = new QLabel(this);
    scoreLabel = new QLabel(this);
    QFrame *hline = new QFrame();
    wordTable = new WordTable();

    reset();

    QFont font = nameLabel->font();
    font.setPointSize(20);
    nameLabel->setFont(font);
    scoreLabel->setFont(font);
    nameLabel->setText(label);
    scoreLabel->setNum(this->score);

    headLayout->addWidget(nameLabel, 0, Qt::AlignmentFlag::AlignLeft);
    headLayout->addWidget(scoreLabel, 0, Qt::AlignmentFlag::AlignRight);
    layout->addLayout(headLayout);

    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);
    layout->addWidget(hline);

    layout->addWidget(wordTable);

    setLayout(layout);
}

bool WordListWidget::contains(const QString word) const {
    return this->words.contains(word);
}

void WordListWidget::addScore(int score)
{
    this->score += score;
    scoreLabel->setNum(this->score);
}

void WordListWidget::addWord(QString word)
{
    this->words.insert(word);
    this->wordTable->addWord(word);
    this->addScore(word.length() - 3);
    update();
}

void WordListWidget::reset()
{
    this->score = 0;
    this->scoreLabel->setNum(0);
    this->words.clear();
    this->wordTable->clear();
    update();
}
