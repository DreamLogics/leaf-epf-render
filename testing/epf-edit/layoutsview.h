#ifndef LAYOUTSVIEW_H
#define LAYOUTSVIEW_H

#include <QDockWidget>

namespace Ui {
class LayoutsView;
}

class LayoutsView : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit LayoutsView(QWidget *parent = 0);
    ~LayoutsView();
    
private:
    Ui::LayoutsView *ui;
};

#endif // LAYOUTSVIEW_H
