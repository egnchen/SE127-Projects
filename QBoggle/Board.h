#ifndef BOARD_H
#define BOARD_H

#include "Cube.h"
#include <QWidget>
#include <QSignalMapper>

class Board : public QWidget
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = nullptr, int size = 5, const QString *cubeLetters = BIG_BOGGLE_CUBES);
    virtual ~Board();
    void shake();
    bool contains(const QString &word, bool *userVis = nullptr) const { return contains(word.toStdString(), userVis); }
    bool contains(const std::string &word, bool *userVis = nullptr) const;
    void highlight(const bool *vis = nullptr);

    QSet<QString> searchAll(const QSet<QString> &found);
    int getBoardSize() const { return this->size; }

signals:
    void prompt(QString prompt);
    void newWord(QString word);

public slots:
    void getCommand(QString command);
    void cubeClick(int index);

private:
    int size;
    Cube **cubes;
    QString *letters;
    QSignalMapper *mapper;
    QString currentWord;
    inline int index(int i, int j) const { return i * size + j; }
    inline int xidx(int idx) const { return idx / size; }
    inline int yidx(int idx) const { return idx % size; }
    bool searchFor(const std::string &, int x, int y, unsigned int pos, bool *vis) const;
    void searchFull(int x, int y, QString &word, bool *vis, QSet<QString> &words);


    static const QString STANDARD_CUBES[16];
    static const QString BIG_BOGGLE_CUBES[25];
    static const int offset[8][2];
    int lastClicked[2];

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // BOARD_H
