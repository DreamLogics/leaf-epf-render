#include "epfedit.h"
#include "ui_epfedit.h"

epfedit::epfedit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::epfedit)
{
    ui->setupUi(this);
}

epfedit::~epfedit()
{
    delete ui;
}
