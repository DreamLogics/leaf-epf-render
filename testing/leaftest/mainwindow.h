#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CEPFView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void doLoad();
    
private:
    Ui::MainWindow *ui;
    CEPFView* epfview;
};

#endif // MAINWINDOW_H
