#include "Cube.h"

#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QDebug>
#include <QMouseEvent>

Cube::Cube(QWidget *parent) : QWidget(parent), animation(this, "hlProgress")
{
    label = new QLabel();
    QHBoxLayout *layout = new QHBoxLayout();
    label->setText("");
    label->setAlignment(Qt::AlignCenter);
    QFont font = label->font();
    font.setPointSize(20);
    label->setFont(font);
    layout->addWidget(label);
    isClicked = false;
    this->setFixedSize(75, 75);
    this->setStyleSheet("background-color: white; border-radius: 15px; border: 2px solid");
    setLayout(layout);
}

void Cube::setLetter(QChar l)
{
    letter = l;
    if(letter.toLower() == 'q')
        label->setText("Qu");
    else
        label->setText(letter);
}

QChar Cube::getLetter() const {
    return letter;
}

void Cube::setHlProgress(double value) {
    hlProgress = value;
    static QString tpl("background-color: %1; border-radius: %2; border: %3 solid");
    static QColor startColor("#00bcff"), endColor("white");
    QColor color;
    color.setRedF(startColor.redF() * (1 - hlProgress) + endColor.redF() * hlProgress);
    color.setGreenF(startColor.greenF() * (1 - hlProgress) + endColor.greenF() * hlProgress);
    color.setBlueF(startColor.blueF() * (1 - hlProgress) + endColor.blueF() * hlProgress);
    double borderRadius = 15 * hlProgress;
    double borderWidth = 2 * hlProgress;
    setStyleSheet(tpl.arg(color.name()).arg(borderRadius).arg(borderWidth));
}

void Cube::flash(int durationMillis) {
    animation.setDuration(durationMillis);
    animation.setStartValue(0);
    animation.setEndValue(1);
    animation.start();
}

void Cube::highlight(int durationMillis) {
    animation.setDuration(durationMillis);
    animation.setStartValue(1);
    animation.setEndValue(0);
    animation.start();
}

QChar Cube::getLetterLowerCase() const {
    return letter.toLower();
}

void Cube::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::LeftButton) {
        if(isClicked)
            return;
        isClicked = true;
        highlight(500);
        update();
        emit clicked();
    } else
        QWidget::mousePressEvent(event);
}

void Cube::reset() {
    animation.stop();
    this->isClicked = false;
    this->flash(500);
}
