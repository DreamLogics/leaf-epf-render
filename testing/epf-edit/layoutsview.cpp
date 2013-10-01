#include "layoutsview.h"
#include "ui_layoutsview.h"

LayoutsView::LayoutsView(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LayoutsView)
{
    ui->setupUi(this);
}

LayoutsView::~LayoutsView()
{
    delete ui;
}
