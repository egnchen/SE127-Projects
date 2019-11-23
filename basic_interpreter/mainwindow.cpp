#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // configure menu & actions
    QMenu *fileMenu = new QMenu(this);
    QAction *openFileAction = new QAction(this);
    openFileAction->setText(tr("&Open"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));
    QAction *saveFileAction = new QAction(this);
    saveFileAction->setText(tr("&Save"));
    connect(saveFileAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->setTitle(tr("&File"));
    menuBar()->addMenu(fileMenu);

    // configure syntax highlighter
    highlighter = new BasicSyntaxHighlighter(
                (ui->centralwidget->findChildren<QTextEdit *>("mainTextEdit"))[0]->document());

    // setup current filename
    changeFilename("");
}

void MainWindow::changeFilename(QString filename)
{
    this->currentFilename = filename;
    if(this->currentFilename.length() > 0)
        this->setWindowTitle(filename + " - Basic interpreter");
    else
        this->setWindowTitle("New file - Basic interpreter");
}

void MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open script"));
    if(filename.length() == 0)
        return;
    // open file now
    QFile file(filename);
    changeFilename(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        (ui->centralwidget->findChildren<QTextEdit *>("mainTextEdit"))[0]->setText(
                stream.readAll());
    } else {
        QMessageBox::warning(this, "Failed to open file", "Cannot open" + file.fileName());
    }
}

void MainWindow::saveFile() {
    QString filename;
    if(this->currentFilename.length() > 0) {
        // save as current filename
        filename = currentFilename;
    } else {
        filename = QFileDialog::getSaveFileName(this, "Save to...");
    }

    if(filename.length() == 0)
        return;

    QFile file(filename);
    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << (ui->centralwidget->findChildren<QTextEdit *>("mainTextEdit"))[0]->toPlainText();
        file.close();
    }

    changeFilename(filename);
}

MainWindow::~MainWindow()
{
    delete ui;
}

