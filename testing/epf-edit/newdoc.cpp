#include "newdoc.h"
#include "ui_newdoc.h"

NewDoc::NewDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewDoc)
{
    ui->setupUi(this);
}

NewDoc::~NewDoc()
{
    delete ui;
}
