#ifndef EPFREADER_H
#define EPFREADER_H

#include <QMainWindow>

class CEPFView;
class CDocument;

namespace Ui {
class EPFReader;
}

class EPFReader : public QMainWindow
{
    Q_OBJECT

public:
    explicit EPFReader(QString file, QWidget *parent = 0);
    ~EPFReader();

    void open(QString file);

private slots:
    void on_action_Open_triggered();

    void on_actionNext_Section_triggered();

    void on_actionPrevious_Section_triggered();

    void on_actionTable_of_Contents_triggered();

private:
    Ui::EPFReader *ui;
    CEPFView* m_pView;
    QThread *m_pThread;
    CDocument *m_pDocument;
};

#endif // EPFREADER_H
