#ifndef CPROJECTVIEWITEM_H
#define CPROJECTVIEWITEM_H

#include <QTreeWidgetItem>

class CProjectViewItem : public QTreeWidgetItem
{
public:
    CProjectViewItem(QString path, int type = Type);
    virtual ~CProjectViewItem();

    QString path() const;
    bool altered() const;
    void setAltered(bool b);

private:
    QString m_sPath;
    bool m_bAltered;
};

#endif // CPROJECTVIEWITEM_H
