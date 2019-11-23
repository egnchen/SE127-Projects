#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QTextEdit>
#include <QWidget>

class Console : public QTextEdit
{
    Q_OBJECT
public:
    static const QList<QString> prompts;
    static const QList<QString> commands;
    static const QList<QString> descriptions;
    static const QList<QString> instructions;
    explicit Console(QWidget *parent = nullptr);
    void welcome();

signals:
    void newLine(QString newline);
    void newCommand(QString command);

public slots:
    void clear();
    void write(QString msg);
    void getCommand(QString command);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // CONSOLE_H
