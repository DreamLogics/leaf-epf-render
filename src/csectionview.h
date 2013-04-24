#ifndef CSECTIONVIEW_H
#define CSECTIONVIEW_H

#include <QGraphicsView>

class CSection;

class CSectionView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CSectionView(CSection* section,QObject *parent = 0);
    
signals:
    
public slots:
    

private:

    //CSection* m_pSection;
};

#endif // CSECTIONVIEW_H
