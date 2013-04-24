#ifndef CEPFVIEW_H
#define CEPFVIEW_H

#include <QObject>
#include <QString>
#include <QList>

class CDocument;
class CSectionView;

class CEPFView : public QObject
{
public:
    CEPFView();

    void setDocument(CDocument* doc);

public slots:

    void setSection(QString id);
    void setSection(int index);
    void nextSection();
    void previousSection();
    void tocSection();

private:

    int indexForSection(QString id);

private:

    QList<QString> m_SectionIndex;
    CDocument* m_pDocument;

    CSectionView* m_SectionViews;

    int m_iCurrentSection;
};

#endif // CEPFVIEW_H
