#include "clineedit.h"

CLineEdit::CLineEdit(QWidget* parent) : QLineEdit(parent)
{

}

void CLineEdit::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);
    emit redrawNeeded();
}
