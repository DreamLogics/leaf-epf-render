#include "epfreader.h"
#include "ui_epfreader.h"
#include <cepfview.h>
#include <cepfdocumentreader.h>
#include <QLocale>
#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include "cdesktopdevice.h"

EPFReader::EPFReader(QString file, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EPFReader)
{
    ui->setupUi(this);

    m_pDocument = 0;

    Device::setCurrentDevice(new CDesktopDevice());
    m_pView = new CEPFView(this);
    ui->centralWidget->layout()->addWidget(m_pView);
    m_pThread =  new QThread(this);
    m_pThread->start();

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    ui->mainToolBar->addAction(ui->action_Open);
    ui->mainToolBar->addWidget(empty);
    ui->mainToolBar->addAction(ui->actionTable_of_Contents);
    ui->mainToolBar->addAction(ui->actionPrevious_Section);
    ui->mainToolBar->addAction(ui->actionNext_Section);

    if (!file.isNull())
        open(file);
}

EPFReader::~EPFReader()
{
    delete ui;
    delete m_pView;
    m_pThread->deleteLater();
}

void EPFReader::open(QString file)
{
    CEPFDocumentReader reader;
    QLocale locale = QLocale::system();
    QStringList languages = locale.uiLanguages();

    reader.addPlatform("desktop");
    for (int i=0;i<languages.size();i++)
        reader.addLanguage(languages[i]);

    QString err;

    m_pDocument = reader.loadFromFile(file,&err,true,m_pThread);
    if (!err.isNull())
    {
        QMessageBox mb(this);
        mb.setText(err);
        mb.exec();
        return;
    }

    ui->actionTable_of_Contents->setEnabled(!m_pDocument->property("toc").isNull());

    m_pView->setDocument(m_pDocument);
}

void EPFReader::on_action_Open_triggered()
{
    QString file = QFileDialog::getOpenFileName(this,"Open EPF file.",QDir::homePath() + "/Documents","EPF files (*.epf)");
    if (file.isNull())
        return;
    open(file);
}

void EPFReader::on_actionNext_Section_triggered()
{
    if (!m_pDocument)
        return;
    m_pView->nextSection();
}

void EPFReader::on_actionPrevious_Section_triggered()
{
    if (!m_pDocument)
        return;
    m_pView->previousSection();
}

void EPFReader::on_actionTable_of_Contents_triggered()
{
    m_pView->tocSection();
}
