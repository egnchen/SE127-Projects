#ifndef HELPER_H
#define HELPER_H

#endif // HELPER_H
#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QList>

namespace snake {
class Helper {
public:
    static QString getRecordFilePath() {
        return QDir::home().filePath(".snake_record.json");
    }

    static void addRecord(QString name, int score) {
        QList<entry> entries = getSortedRecords();
        entries.append(entry(name, score));
        std::sort(entries.begin(), entries.end(), std::greater<entry>());
        if(entries.length() > 10)
            entries.erase(entries.begin() + 10, entries.end());
        QJsonArray arr;
        for(entry e: entries) {
            QJsonObject obj;
            obj.insert("name", e.name);
            obj.insert("score", e.score);
            arr.append(obj);
        }
        QFile recordFile(getRecordFilePath());
        recordFile.open(QIODevice::WriteOnly);
        recordFile.write(QJsonDocument(arr).toJson());
        recordFile.close();
    }

    struct entry {
        QString name;
        int score;
        entry(const QString name, int score): name(name), score(score) {}
        entry(const QJsonObject &&obj): name(obj.value("name").toString()), score(obj.value("score").toInt()) {}
        bool operator < (const entry &rhs) const { return score < rhs.score; }
        bool operator > (const entry &rhs) const { return score > rhs.score; }
    };

    static QList<entry> getSortedRecords() {
        QFile recordFile(getRecordFilePath());
        recordFile.open(QIODevice::ReadOnly);
        QList<entry> ret;
        QJsonArray arr = QJsonDocument::fromJson(recordFile.readAll()).array();
        for (QJsonValueRef obj : arr) {
            if(obj.isObject())
                ret.append(entry(obj.toObject()));
        }
        recordFile.close();
        std::sort(ret.begin(), ret.end(), std::greater<entry>());
        return ret;
    }
};

}
