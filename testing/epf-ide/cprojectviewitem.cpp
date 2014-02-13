#include "cprojectviewitem.h"

CProjectViewItem::CProjectViewItem(QString path, int type) : QTreeWidgetItem(type), m_sPath(path)
{
}

CProjectViewItem::~CProjectViewItem()
{

}

QString CProjectViewItem::path() const
{
    return m_sPath;
}
