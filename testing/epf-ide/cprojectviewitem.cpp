#include "cprojectviewitem.h"

CProjectViewItem::CProjectViewItem(QString path, int type) : QTreeWidgetItem(type), m_sPath(path), m_bAltered(false)
{
}

CProjectViewItem::~CProjectViewItem()
{

}

QString CProjectViewItem::path() const
{
    return m_sPath;
}

bool CProjectViewItem::altered() const
{
    return m_bAltered;
}

void CProjectViewItem::setAltered(bool b)
{
    m_bAltered = b;
}
