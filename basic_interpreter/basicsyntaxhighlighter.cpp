#include "basicsyntaxhighlighter.h"
#include <QDebug>

BasicSyntaxHighlighter::BasicSyntaxHighlighter(QTextDocument *parent)
    :QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bPRINT\\b"),
        QStringLiteral("\\bINPUT\\b"),
        QStringLiteral("\\bGOTO\\b"),
        QStringLiteral("\\bIF\\b"),
        QStringLiteral("\\bTHEN\\b"),
        QStringLiteral("\\bEND\\b")
    };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // comments, grey, italic
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);

    commentRule.pattern = QRegularExpression(QStringLiteral("\\bREM\\b.*$"));
    commentRule.format = commentFormat;

    // variables, green, regular fontweight
    QTextCharFormat variableFormat;
    variableFormat.setForeground(Qt::green);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z_][a-zA-Z_0-9]*\\b"));
    rule.format = variableFormat;
    highlightingRules.append(rule);

    // set line number format
    lineNumberFormat.setForeground(Qt::cyan);
    lineNumberFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("^\\b[0-9]+\\b"));
    rule.format = lineNumberFormat;
    highlightingRules.append(rule);

    // numbers, yellow, regular fontweight
    QTextCharFormat numberFormat;
    numberFormat.setForeground(Qt::yellow);
    rule.pattern = QRegularExpression(QStringLiteral("\\b\\d+(\\.\\d+)?\\b"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // operators, red, bold
    QTextCharFormat opFormat;
    opFormat.setForeground(Qt::red);
    opFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("(?<=\\s)[+\\-\\*/<>=](?=\\s)"));
    rule.format = opFormat;
    highlightingRules.append(rule);
}

void BasicSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (auto r = highlightingRules.rbegin(); r != highlightingRules.rend(); ++r) {
        QRegularExpressionMatchIterator matchIterator = r->pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), r->format);
        }
    }
}
