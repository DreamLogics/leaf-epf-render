#ifndef NEWDOC_H
#define NEWDOC_H

#include <QWidget>

namespace Ui {
class NewDoc;
}

class NewDoc : public QWidget
{
    Q_OBJECT
    
public:
    explicit NewDoc(QWidget *parent = 0);
    ~NewDoc();
    
private:
    Ui::NewDoc *ui;
};

#endif // NEWDOC_H
