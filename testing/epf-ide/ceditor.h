#ifndef CEDITOR_H
#define CEDITOR_H

#include <QTextEdit>

class EpfIDE;
class QListWidget;

class CEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit CEditor(QWidget *parent = 0);
    ~CEditor();

    void setIde(EpfIDE* ide);
    void scanID();
    void scanVars();

    void cleanTabs();

signals:

public slots:

protected:

    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void keyReleaseEvent(QKeyEvent *ev);

private:

    void autoComplete();
    QStringList matchIn(QString word, QStringList &list);

private:
    EpfIDE *m_pIDE;

    QStringList m_CSSComplete;
    QStringList m_IDNames;
    QStringList m_JSComplete;
    QStringList m_XMLComplete;
    QStringList m_Varlist;

    QStringList m_Matches;

    QListWidget *m_pMatchList;

};

#endif // CEDITOR_H
