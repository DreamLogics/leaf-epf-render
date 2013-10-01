#include "sectionview.h"
#include "ui_sectionview.h"

SectionView::SectionView(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SectionView)
{
    ui->setupUi(this);
}

SectionView::~SectionView()
{
    delete ui;
}
