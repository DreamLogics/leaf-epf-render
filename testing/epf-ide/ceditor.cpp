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
    QList<QListWidgetItem*> selection = m_pMatchList->selectedItems();
    if (m_pMatchList->isVisible() && selection.size() == 1)
    {
        if (ev->key() == Qt::Key_Return)
        {

            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            cursor.insertText(selection[0]->text());
            cursor.movePosition(QTextCursor::EndOfWord,QTextCursor::MoveAnchor);
            m_pMatchList->hide();
            ev->accept();
            return;
        }
        else if (ev->key() == Qt::Key_Up)
        {
            int row = m_pMatchList->currentRow();
            row--;
            if (row >= 0)
                m_pMatchList->setCurrentRow(row);
            ev->accept();
            return;
        }
        else if (ev->key() == Qt::Key_Down)
        {
            int row = m_pMatchList->currentRow();
            row++;
            if (row < m_pMatchList->count())
                m_pMatchList->setCurrentRow(row);
            ev->accept();
            return;
        }

    }

    QTextEdit::keyPressEvent(ev);
}

void CEditor::keyReleaseEvent(QKeyEvent *ev)
{
    QTextEdit::keyReleaseEvent(ev);
    if (!(ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down))
    {
        scanID();
        scanVars();
        autoComplete();
    }
    else if (ev->key() == Qt::Key_Tab)
    {
        cleanTabs();
    }
    else if (ev->key() == Qt::Key_Return)
    {
        //auto indent
        QRegExp indent("^([ \t]+)");
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
        if (indent.indexIn(cursor.selectedText()) != -1)
        {
            textCursor().insertText(indent.cap(1));
        }

        //auto bracket
        /*QTextCursor cursor = textCursor();
        QTextCursor curcursor = cursor;
        cursor.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,2);
        cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
        if (cursor.selectedText() == "{")
        {
            setTextCursor(curcursor);
            curcursor.insertText("\n    \n}");
        }*/

    }
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
        //first->setSelected(true);

        for (int i=0;i<m_Matches.size();i++)
        {
            if (i==0)
                m_pMatchList->addItem(first);
            else
                m_pMatchList->addItem(m_Matches[i]);
        }

        m_pMatchList->show();
        m_pMatchList->setCurrentItem(first);
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
        int start,end;
        QRegExp startreg("< *sections *>",Qt::CaseInsensitive);
        QRegExp stopreg("< */ *sections *>",Qt::CaseInsensitive);
        start = text.indexOf(startreg);
        end = text.indexOf(stopreg);
        if (start == -1 || end == -1)
            return;
        text = text.mid(start,end-start);
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
        QRegExp varreg("\\$([a-zA_Z0-9_-]+) *= *[^;\n\r]+;");
        int offset = 0;
        int index;
        while ((index = varreg.indexIn(text,offset)) != -1)
        {
            m_Varlist.append(varreg.cap(1));
            offset = index + varreg.cap(0).size();
        }
        //qDebug() << m_Varlist;
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

void CEditor::cleanTabs()
{
    QString text = toPlainText();
    text.replace("\t","    ");
    setPlainText(text);
}
