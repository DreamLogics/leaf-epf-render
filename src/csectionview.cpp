#include "csectionview.h"
#include "csection.h"

CSectionView::CSectionView(CSection* section, QObject *parent) :
    QGraphicsView(parent)
{
    setScene(section);
}
