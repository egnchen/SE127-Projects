#include "Board.h"
#include "Cube.h"
#include "EnglishDictionary.h"
#include <QGridLayout>
#include <QDebug>
#include <iostream>
#include <set>
#include <QSignalMapper>
#include <QMouseEvent>

const QString Board::STANDARD_CUBES[16]  = {
        "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
        "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
        "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
        "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const QString Board::BIG_BOGGLE_CUBES[25]  = {
        "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
        "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
        "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
        "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
        "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

const int Board::offset[8][2] = {
        {0,1},{0,-1},{1,0},{-1,0},
        {1,1},{1,-1},{-1,1},{-1,-1}
};

Board::Board(QWidget *parent, int size, const QString *cubeLetters) : QWidget(parent)
{
    this->size = size;
    this->cubes = new Cube*[size * size];
    this->letters = new QString[size * size];
    for (int i = 0; i < size * size; ++i)
        this->letters[i] = cubeLetters[i];

    QGridLayout *layout = new QGridLayout();
    layout->setMargin(20);
    layout->setSpacing(10);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            this->cubes[index(i, j)] = new Cube(this);
            layout->addWidget(this->cubes[index(i, j)], i, j, Qt::AlignmentFlag::AlignCenter);
        }
    }
    setLayout(layout);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            this->cubes[index(i, j)]->setLetter(this->letters[index(i, j)].at(0));
        }
    }

    lastClicked[0] = lastClicked[1] = -1;

    // setup signal mapper
    this->mapper = new QSignalMapper();
    srandom(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int idx = index(i, j);
            this->mapper->setMapping(this->cubes[idx], idx);
            connect(this->cubes[idx], SIGNAL(clicked()),
                    this->mapper, SLOT(map()));
        }
    }
    connect(this->mapper, SIGNAL(mapped(int)),
            this, SLOT(cubeClick(int)));
}

void Board::cubeClick(int index) {

    int x = xidx(index), y = yidx(index);
    if(this->lastClicked[0] >= 0 &&
            !(abs(this->lastClicked[0] - x) <= 1
              && abs(this->lastClicked[1] - y ) <= 1)) {
        emit prompt("Please click on cube by the rule.");
        this->cubes[index]->reset();
        return;
    }
    this->lastClicked[0] = x;
    this->lastClicked[1] = y;

    QChar c = this->cubes[index]->getLetter().toLower();
    if (c != 'q')
        currentWord.append(c);
    else
        currentWord.append("qu");
    if(currentWord.length() < 4) return;
    emit newWord(currentWord);
    emit prompt("");    // to start a new line
}

bool Board::contains(const std::string &word, bool *userVis) const {
    // initialize vis array
    static bool *internalVis = new bool[size * size];
    bool *vis = userVis == nullptr ? internalVis : userVis;
    memset(vis, false, sizeof(bool) * size * size);
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(this->cubes[index(i, j)]->getLetterLowerCase() == word[0]) {
                // search from here
                if(searchFor(word, i, j, 0, vis))
                    return true;
            }
        }
    }
    return false;
}

bool Board::searchFor(const std::string &word, int x, int y, unsigned int pos, bool *vis) const {
    if(pos == word.length())
        return true;
    if(x < 0 || x >= size || y < 0 || y >= size)
        return false;
    if(vis[index(x, y)])
        return false;

    QChar c = this->cubes[index(x, y)]->getLetterLowerCase();
    if(c != word[pos])
        return false;
    if(c == 'q' && pos < word.length() - 1) {
        if ('u' != word[pos + 1])
            return false;
        pos++;
    }

    vis[index(x, y)] = true;
    for(int i = 0; i < 8; i++) {
        if(searchFor(word, x + offset[i][0], y + offset[i][1], pos + 1, vis))
            return true;
    }
    vis[index(x, y)] = false;
    return false;
}

void Board::searchFull(int x, int y, QString &word, bool *vis, QSet<QString> &words) {
    if(x < 0 || x >= size || y < 0 || y >= size)
        return;
    if(vis[index(x, y)])
        return;
    const EnglishDictionary &dic = EnglishDictionary::getInstance();

    QChar c = this->cubes[index(x, y)]->getLetterLowerCase();
    if(c == 'q')
        word.append("qu");
    else
        word.append(c);
    if(word.length() >= 4 && dic.contains(word.toStdString())) {
        words.insert(word);
    } else if(!dic.containsPrefix(word.toStdString())) {
        word.chop(c == 'q' ? 2 : 1);
        return;
    }
    vis[index(x, y)] = true;
    for(int i = 0; i < 8; i++)
        searchFull(x + offset[i][0], y + offset[i][1], word, vis, words);
    vis[index(x, y)] = false;
    word.chop(c == 'q' ? 2 : 1);
}

Board::~Board()
{
    if (cubes) delete[] cubes;
    if (letters) delete[] letters;
    if (mapper) delete mapper;
}

void Board::shake()
{
    // Shuffle Cubes
    for (int i = 0; i < size * size; ++i)
        std::swap(this->letters[random() % (size * size )],
                this->letters[random() % (size * size)]);
    // Shake Cubes
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            this->cubes[index(i, j)]->setLetter(this->letters[index(i, j)].at(
                        random() % (this->letters[index(i, j)].length())));
        }
    }
    // reset game
    currentWord.clear();
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            this->cubes[index(i, j)]->reset();
        }
    }
}

void Board::getCommand(QString command) {
    if(command == ".search") {
        QSet<QString> found;
        found = this->searchAll(found);
        for(QString s: found)
            emit prompt("Found word " + s);
    }
}

void Board::highlight(const bool *vis) {
    if(currentWord.length() != 0) return;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(vis == nullptr || vis[index(i, j)]) cubes[index(i, j)]->flash();
        }
    }
}

QSet<QString> Board::searchAll(const QSet<QString> &found) {
    static bool *vis = new bool[size * size];
    memset(vis, false, sizeof(bool) * size * size);
    QString word;
    word.reserve(16);
    QSet<QString> words;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            searchFull(i, j, word, vis, words);
            word.clear();
        }
    }
    words = words.subtract(found);
    return words;
}

void Board::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::RightButton) {
        for(int i = 0; i < size; i++)
            for(int j = 0; j < size; j++)
                this->cubes[index(i, j)]->reset();
        currentWord.clear();
        lastClicked[0] = lastClicked[1] = -1;
    } else
        QWidget::mousePressEvent(event);
}
