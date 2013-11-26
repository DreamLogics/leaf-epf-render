#ifndef CLINEEDIT_H
#define CLINEEDIT_H

#include <QLineEdit>

class CLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CLineEdit(QWidget* parent=0);

protected:
    virtual void paintEvent(QPaintEvent *e);

signals:
    void redrawNeeded();
};

#endif // CLINEEDIT_H
