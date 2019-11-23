#ifndef BASICSYNTAXHIGHLIGHTER_H
#define BASICSYNTAXHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class BasicSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    BasicSyntaxHighlighter(QTextDocument *parent=nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    HighlightingRule commentRule;
    QTextCharFormat lineNumberFormat;
};

#endif // BASICSYNTAXHIGHLIGHTER_H
