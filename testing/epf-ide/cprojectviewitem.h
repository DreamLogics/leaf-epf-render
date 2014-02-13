#ifndef CPROJECTVIEWITEM_H
#define CPROJECTVIEWITEM_H

#include <QTreeWidgetItem>

class CProjectViewItem : public QTreeWidgetItem
{
public:
    CProjectViewItem(QString path, int type = Type);
    virtual ~CProjectViewItem();

    QString path() const;

private:
    QString m_sPath;
};

#endif // CPROJECTVIEWITEM_H
