#ifndef TEXTINPUTVIEW_H
#define TEXTINPUTVIEW_H

#include <QWidget>
#include <QList>

namespace Ui {
class TextInputView;
}

class CTextFieldObject;

class TextInputView : public QWidget
{
    Q_OBJECT

public:
    explicit TextInputView(QWidget *parent = 0);
    ~TextInputView();

    static TextInputView* get(QWidget *parent = 0);

public slots:

    void getLine();

signals:

    void setTextLine(QString str);

private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::TextInputView *ui;
    CTextFieldObject* m_pCaller;
};

#endif // TEXTINPUTVIEW_H
