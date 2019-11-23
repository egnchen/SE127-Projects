#ifndef WORDLISTWIDGET_H
#define WORDLISTWIDGET_H

#include "WordTable.h"
#include "Board.h"
#include <QWidget>

class WordListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WordListWidget(QWidget *parent = nullptr, QString label = "");\
    bool contains(const QString word) const;
    const QSet<QString> &getWords() const { return words; }
signals:
    void prompt(QString promptString);

public slots:
    void addWord(QString word);
    void addScore(int s);
    void reset();

private:
    QString label;
    QSet<QString> words;
    int score;
    WordTable *wordTable;
    QLabel *nameLabel;
    QLabel *scoreLabel;
};

#endif // WORDLISTWIDGET_H
