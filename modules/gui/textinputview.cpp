#include "textinputview.h"
#include "ui_textinputview.h"
#include "ctextfieldobject.h"

TextInputView::TextInputView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextInputView)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Popup);
    hide();
}

TextInputView::~TextInputView()
{
    delete ui;
}

void TextInputView::on_lineEdit_returnPressed()
{
    emit setTextLine(ui->lineEdit->text());
    hide();
    m_pCaller = 0;
}

TextInputView* TextInputView::get(QWidget *parent)
{
    static TextInputView* p = 0;
    if (!p)
        p = new TextInputView(parent);
    return p;
}

void TextInputView::getLine()
{
    if (m_pCaller)
        return;
    CTextFieldObject* obj = dynamic_cast<CTextFieldObject*>(sender());
    if (obj)
    {
        m_pCaller = obj;
        disconnect(this,SIGNAL(setTextLine(QString)));
        connect(this,SIGNAL(setTextLine(QString)),obj,SLOT(setTextLine(QString)));
        show();
    }
}
