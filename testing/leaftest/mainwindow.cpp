#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cepfview.h>
#include <cdocument.h>
#include <cepfdocumentreader.h>
#include <QDebug>
#include <QLayout>
#include <QThread>
#include <QString>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QThread* th = new QThread();
    epfview = new CEPFView();
    ui->centralWidget->layout()->addWidget(epfview);

    QTimer::singleShot(10,this,SLOT(doLoad()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doLoad()
{
    CEPFDocumentReader dr;

    dr.setLanguage("en");
    dr.addPlatform("ipad");
    dr.addModule("optima");

    QString error;

    //th->start();
    CDocument* doc = dr.loadFromFile("optimaui.epf",0/*&error*//*,th*/);

    if (!doc)
    {
        qDebug() << error;
        return;
    }

    epfview->setDocument(doc);
}
