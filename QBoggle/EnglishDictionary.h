#ifndef ENGLISHDICTIONARY_H
#define ENGLISHDICTIONARY_H

#include "lexicon.h"
#include <QFile>

class EnglishDictionary : public Lexicon
{
public:
    static EnglishDictionary &getInstance() {
        if(dic == nullptr) {
            QFile qFile(":/res/EnglishWords.txt");
            if (!qFile.open(QIODevice::ReadOnly)) {
                throw new std::runtime_error("Resource file not found!");
            }
            dic = new EnglishDictionary(qFile);
        }
        return *dic;
    }
private:
    EnglishDictionary(QFile &file): Lexicon(file) {}
    static EnglishDictionary *dic;
};

#endif // ENGLISHDICTIONARY_H
