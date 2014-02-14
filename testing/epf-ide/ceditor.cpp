#include "ceditor.h"
#include <QKeyEvent>
#include "epfide.h"
#include <QPainter>
#include "cepfhl.h"
#include <QListWidget>
#include <QDebug>

CEditor::CEditor(QWidget *parent) :
    QTextEdit(parent)
{
    m_pMatchList = new QListWidget(this);
    m_pMatchList->hide();
}

CEditor::~CEditor()
{
    delete m_pMatchList;
}

void CEditor::setIde(EpfIDE *ide)
{
    m_pIDE = ide;
}

void CEditor::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Return)
    {

    }

    QTextEdit::keyPressEvent(ev);
}

void CEditor::keyReleaseEvent(QKeyEvent *ev)
{
    scanID();
    scanVars();
    autoComplete();
    QTextEdit::keyReleaseEvent(ev);
}

void CEditor::autoComplete()
{
    QTextCursor cursor = textCursor();
    QTextCursor cursorold = cursor;
    cursor.movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
    QString word = cursor.selectedText();
    QChar prechar;
    if (cursor.selectionStart() > 0)
        prechar = toPlainText()[cursor.selectionStart()-1];
    setTextCursor(cursorold);

    //qDebug() << word;
    if (m_pIDE->m_pEPFSyntax->mode() == CEPFHL::modeCSS)
    {
        if (prechar == '#')
        {
            if (word == "")
                m_Matches = m_IDNames;
            else
                m_Matches = matchIn(word,m_IDNames);
        }
        else if (prechar == '$')
        {
            if (word == "")
                m_Matches = m_Varlist;
            else
                m_Matches = matchIn(word,m_Varlist);
        }
        else
            m_Matches = matchIn(word,m_CSSComplete);
    }

    if (m_Matches.size() > 0)
    {
        QPoint globalPos = cursorRect().bottomRight();

        m_pMatchList->clear();

        QListWidgetItem *first =  new QListWidgetItem(m_Matches[0]);
        first->setSelected(true);

        for (int i=0;i<m_Matches.size();i++)
        {
            if (i==0)
                m_pMatchList->addItem(first);
            else
                m_pMatchList->addItem(m_Matches[i]);
        }

        m_pMatchList->show();
        int height = m_Matches.size()*16;
        if (height > 400)
            height = 400;
        m_pMatchList->setGeometry(globalPos.x(),globalPos.y(),200,height);


    }
    else
        m_pMatchList->hide();
}

QStringList CEditor::matchIn(QString word, QStringList &list)
{
    QStringList matches;
    for (int i=0;i<list.size();i++)
    {
        if (list[i].startsWith(word))
            matches.append(list[i]);
    }
    return matches;
}

void CEditor::scanID()
{
    if (m_pIDE->m_pEPFSyntax->mode() == CEPFHL::modeXML)
    {
        QString text = toPlainText();
        m_IDNames.clear();
        QRegExp varreg("id *= *\"([^\"]+)\"");
        int offset = 0;
        int index;
        while ((index = varreg.indexIn(text,offset)) != -1)
        {
            QString id = varreg.cap(1);
            id.replace("[ \t\n\r]+","");
            m_IDNames.append(id);
            offset = index + varreg.cap(0).size();
        }
    }
}

void CEditor::scanVars()
{
    QString text = toPlainText();
    m_Varlist.clear();
    if (m_pIDE->m_pEPFSyntax->mode() == CEPFHL::modeCSS)
    {
        QRegExp varreg("$([a-zA_Z0-9_-]+)[^;\n\r]+");
        int offset = 0;
        int index;
        while ((index = varreg.indexIn(text,offset)) != -1)
        {
            m_Varlist.append(varreg.cap(1));
            offset = index + varreg.cap(0).size();
        }
    }
    else if (m_pIDE->m_pEPFSyntax->mode() == CEPFHL::modeJS)
    {
        QRegExp varreg("var[ ]+([a-zA_Z0-9_-]+)[^;\n\r]+");
        int offset = 0;
        int index;
        while ((index = varreg.indexIn(text,offset)) != -1)
        {
            m_Varlist.append(varreg.cap(1));
            offset = index + varreg.cap(0).size();
        }
    }

}
