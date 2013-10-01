#ifndef EPFEDIT_H
#define EPFEDIT_H

#include <QMainWindow>

namespace Ui {
class epfedit;
}

class epfedit : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit epfedit(QWidget *parent = 0);
    ~epfedit();
    
private:
    Ui::epfedit *ui;
};

#endif // EPFEDIT_H
