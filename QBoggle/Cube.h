#ifndef CUBE_H
#define CUBE_H

#include <QLabel>
#include <QWidget>
#include <QPropertyAnimation>

class Cube : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double hlProgress READ getHlProgress WRITE setHlProgress)
public:
    explicit Cube(QWidget *parent = nullptr);
    void setLetter(QChar l);
    QChar getLetter() const;
    QChar getLetterLowerCase() const;
    void flash(int durationMillis = 1000);
    void highlight(int durationMillis = 1000);
    double getHlProgress() const { return hlProgress; }
    void setHlProgress(double value);
private:
    QChar letter = '\0';
    QLabel *label;
    QPropertyAnimation animation;
    double hlProgress;
    bool isClicked;
signals:
    void clicked();
public slots:
    void reset();
protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CUBE_H
