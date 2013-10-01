#ifndef SECTIONVIEW_H
#define SECTIONVIEW_H

#include <QDockWidget>

namespace Ui {
class SectionView;
}

class SectionView : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit SectionView(QWidget *parent = 0);
    ~SectionView();
    
private:
    Ui::SectionView *ui;
};

#endif // SECTIONVIEW_H
