#include "epfide.h"
#include "ui_epfide.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include "cprojectviewitem.h"
#include <QIcon>
#include <QProcess>
#include <QScrollBar>
#include <QDebug>

#include "cepfhl.h"

EpfIDE::EpfIDE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EpfIDE)
{
    ui->setupUi(this);

    ui->editview->setIde(this);

    ui->editview->setTabStopWidth(40);
    m_pProjectItem = 0;
    m_pEPFSyntax = new CEPFHL(ui->editview->document());

    connect(ui->editview,SIGNAL(textChanged()),this,SLOT(createOutline()));

    m_pExternal = new QProcess(this);
    connect(m_pExternal,SIGNAL(readyReadStandardOutput()),this,SLOT(stdOut()));
    connect(m_pExternal,SIGNAL(readyReadStandardError()),this,SLOT(stdErr()));
    connect(m_pExternal,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(stdClose()));

    m_bIsRunning = false;
    m_bNoChange = true;
}

EpfIDE::~EpfIDE()
{
    delete ui;
    delete m_pEPFSyntax;
}

void EpfIDE::stdOut()
{
    QByteArray data = m_pExternal->readAllStandardOutput();
    print(QString::fromUtf8(data));
}

void EpfIDE::stdErr()
{
    QByteArray data = m_pExternal->readAllStandardError();
    print(QString::fromUtf8(data),QColor(255,0,0));
}

void EpfIDE::stdClose()
{
    m_pExternal->close();
    m_bIsRunning = false;
}

void EpfIDE::open(QString path)
{
    if (!QFile::exists(path))
        return;

    if (!m_sPath.isNull())
    {
        //close
    }

    m_sPath = path;

    QFileInfo pf(path);
    QFileInfo pd(pf.path());

    ui->projectview->clear();

    m_pProjectItem = new CProjectViewItem("");
    m_pProjectItem->setText(0,pd.fileName());
    m_pProjectItem->setIcon(0,QIcon::fromTheme("package-x-generic"));
    ui->projectview->addTopLevelItem(m_pProjectItem);

    m_pContentItem = new CProjectViewItem(m_sPath);
    m_pContentItem->setText(0,pf.fileName());
    m_pContentItem->setIcon(0,QIcon::fromTheme("text-x-generic"));
    m_pProjectItem->addChild(m_pContentItem);

    m_pScriptsItem = new CProjectViewItem("");
    m_pScriptsItem->setText(0,"Scripts");
    m_pScriptsItem->setIcon(0,QIcon::fromTheme("folder"));
    m_pProjectItem->addChild(m_pScriptsItem);

    m_pTextItem = new CProjectViewItem("");
    m_pTextItem->setText(0,"Text");
    m_pTextItem->setIcon(0,QIcon::fromTheme("folder"));
    m_pProjectItem->addChild(m_pTextItem);

    m_pStylesheetsItem = new CProjectViewItem("");
    m_pStylesheetsItem->setText(0,"CSS");
    m_pStylesheetsItem->setIcon(0,QIcon::fromTheme("folder"));
    m_pProjectItem->addChild(m_pStylesheetsItem);

    m_pResourcesItem = new CProjectViewItem("");
    m_pResourcesItem->setText(0,"Resources");
    m_pResourcesItem->setIcon(0,QIcon::fromTheme("folder"));
    m_pProjectItem->addChild(m_pResourcesItem);

    scanProject();

    m_pProjectItem->setExpanded(true);
}

void EpfIDE::save()
{
    QList<QTreeWidgetItem*> selection = ui->projectview->selectedItems();

    if (selection.size() == 0)
        return;

    QTreeWidgetItem* pitem = selection.at(0);

    CProjectViewItem* item = static_cast<CProjectViewItem*>(pitem);

    QFile f(item->path());
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(ui->editview->toPlainText().toUtf8());
        f.close();
        item->setAltered(false);
        QString name = item->text(0);
        name.chop(1);
        item->setText(0,name);
    }
    else
        print("Can't write to file: "+item->path(),QColor(255,0,0));
}

void EpfIDE::saveAll()
{

}

void EpfIDE::scanProject()
{
    if (m_sPath.isNull() || !m_pProjectItem)
        return;

    QList<QTreeWidgetItem*> todelete;

    todelete << m_pResourcesItem->takeChildren();
    todelete << m_pStylesheetsItem->takeChildren();
    todelete << m_pScriptsItem->takeChildren();
    todelete << m_pTextItem->takeChildren();

    for (int i=0;i<todelete.size();i++)
        delete todelete[i];

    QFileInfo pf(m_sPath);
    QDir contentdir = pf.dir();

    if (!contentdir.cd("content"))
    {
        contentdir.mkdir("content");
        if (!contentdir.cd("content"))
            return;
    }

    QStringList mediafiles = QStringList() << "webm" << ".ogv";
    QStringList imgfiles = QStringList() << ".png" << ".svg" << ".jpg";

    CProjectViewItem* item;

    QFileInfoList info = contentdir.entryInfoList();

    for (int i=0;i<info.size();i++)
    {
        if (info.at(i).isFile())
        {
            item = new CProjectViewItem(info.at(i).absoluteFilePath());
            item->setText(0,info.at(i).fileName());
            QString ext = info.at(i).fileName().right(4);
            if (info.at(i).fileName().right(2) == "js")
            {
                item->setIcon(0,QIcon::fromTheme("text-x-script"));
                m_pScriptsItem->addChild(item);
            }
            else if (mediafiles.contains(ext))
            {
                item->setIcon(0,QIcon::fromTheme("video-x-generic"));
                m_pResourcesItem->addChild(item);
            }
            else if (imgfiles.contains(ext))
            {
                item->setIcon(0,QIcon::fromTheme("image-x-generic"));
                m_pResourcesItem->addChild(item);
            }
            else if (ext == "html")
            {
                item->setIcon(0,QIcon::fromTheme("text-html"));
                m_pTextItem->addChild(item);
            }
            else if (ext == ".css")
            {
                item->setIcon(0,QIcon::fromTheme("text-x-generic"));
                m_pStylesheetsItem->addChild(item);
            }
            else
            {
                item->setIcon(0,QIcon::fromTheme("text-x-generic"));
                m_pResourcesItem->addChild(item);
            }
        }
    }
}

void EpfIDE::editFile(QString path)
{
    bool bLoad = false;
    if (path.endsWith(".js"))
    {
        m_pEPFSyntax->setMode(CEPFHL::modeJS);
        bLoad = true;
    }
    else if (path.endsWith(".xml"))
    {
        m_pEPFSyntax->setMode(CEPFHL::modeXML);
        bLoad = true;
    }
    else if (path.endsWith(".css"))
    {
        m_pEPFSyntax->setMode(CEPFHL::modeCSS);
        bLoad = true;
    }
    else if (path.endsWith(".html"))
    {
        m_pEPFSyntax->setMode(CEPFHL::modeHTML);
        bLoad = true;
    }

    if (bLoad)
    {
        if (!m_sOpenFile.isNull())
        {
            if (m_sOpenFile.contains(m_sOpenFile))
                m_FileBuffer[m_sOpenFile] = ui->editview->toPlainText();
            else
                m_FileBuffer.insert(m_sOpenFile,ui->editview->toPlainText());
        }

        QFileInfo fi(path);
        m_bNoChange = true;

        ui->action_Save->setText("&Save \""+fi.fileName()+"\"");

        if (m_FileBuffer.contains(path))
        {
            ui->editview->setPlainText(m_FileBuffer[path]);
        }
        else
        {
            QFile f(path);
            if (f.open(QIODevice::ReadOnly))
            {
                ui->editview->setPlainText(QString::fromUtf8(f.readAll()));
                f.close();
            }
        }

        ui->editview->scanID();
        ui->editview->scanVars();

        m_sOpenFile = path;
    }
}

void EpfIDE::createOutline()
{

}

void EpfIDE::print(QString str, QColor color)
{
    bool bScroll = false;
    if (ui->console->verticalScrollBar()->value() == ui->console->verticalScrollBar()->maximum())
        bScroll = true;

    QTextBlockFormat format;
    format.setForeground(color);
    ui->console->append(str+"\n");
    ui->console->moveCursor(QTextCursor::End);
    ui->console->textCursor().setBlockFormat(format);

    if (bScroll)
        ui->console->verticalScrollBar()->setValue(ui->console->verticalScrollBar()->maximum());
}

void EpfIDE::build()
{
    QFileInfo f(m_sPath);
    QFileInfo fd(f.path());
    QDir d = f.dir();
    d.cdUp();
    m_pExternal->setProgram("epf-create");
    m_pExternal->setWorkingDirectory(d.absolutePath());
    m_pExternal->setArguments(QStringList() << fd.fileName());
    m_pExternal->open();
}

void EpfIDE::run()
{
    if (m_pExternal->isOpen())
    {
        if (m_bIsRunning)
        {
            m_pExternal->kill();
            m_pExternal->close();
        }
        else
            return;
    }
    QFileInfo f(m_sPath);
    QFileInfo fd(f.path());
    QDir d = f.dir();
    d.cdUp();
    m_pExternal->setProgram("epf-view");
    m_pExternal->setWorkingDirectory(d.absolutePath());
    m_pExternal->setArguments(QStringList() << fd.fileName() + ".epf");
    m_pExternal->open();
    m_bIsRunning = true;
}

void EpfIDE::on_action_New_triggered()
{

}

void EpfIDE::on_action_Open_triggered()
{
#ifdef Q_OS_MACX
    open(QFileDialog::getOpenFileName(this,"Open content.xml EPF file.",QDir::homePath(),"EPF File (*.xml)"));
#else
    open(QFileDialog::getOpenFileName(this,"Open content.xml EPF file.",QDir::homePath(),"EPF File (content.xml)"));
#endif
}

void EpfIDE::on_action_Save_triggered()
{
    save();
}

void EpfIDE::on_action_Build_triggered()
{
    build();
}

void EpfIDE::on_action_Run_triggered()
{
    run();
}

void EpfIDE::on_projectview_itemChanged(QTreeWidgetItem *pitem, int column)
{

}

void EpfIDE::on_projectview_customContextMenuRequested(const QPoint &pos)
{
    if (m_sPath.isNull())
        return;

    QPoint globalPos = ui->projectview->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Refresh");
    menu.addSeparator();
    menu.addAction("Add new...");
    menu.addAction("Add Existing Files...");

    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->text() == "Refresh")
            scanProject();
    }
}

void EpfIDE::on_projectview_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> selection = ui->projectview->selectedItems();

    if (selection.size() == 0)
        return;

    QTreeWidgetItem* pitem = selection.at(0);

    CProjectViewItem* item = static_cast<CProjectViewItem*>(pitem);
    editFile(item->path());
}

void EpfIDE::on_editsearch_textChanged(const QString &arg1)
{
    //mark
}

void EpfIDE::on_outlinesearch_textChanged(const QString &arg1)
{

}

void EpfIDE::on_editsearch_returnPressed()
{
    ui->editview->find(ui->editsearch->text(),0);
}

void EpfIDE::on_editview_textChanged()
{
    if (m_bNoChange)
    {
        m_bNoChange = false;
        return;
    }
    QList<QTreeWidgetItem*> selection = ui->projectview->selectedItems();

    if (selection.size() == 0)
        return;

    QTreeWidgetItem* pitem = selection.at(0);

    CProjectViewItem* item = static_cast<CProjectViewItem*>(pitem);

    ui->action_Save->setEnabled(true);

    if (!item->altered())
    {
        item->setAltered(true);
        item->setText(0,item->text(0)+"*");
    }
}
