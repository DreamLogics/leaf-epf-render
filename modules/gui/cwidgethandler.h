#ifndef CWIDGETHANDLER_H
#define CWIDGETHANDLER_H

#include <QWidget>
#include <QMap>
#include <QMutex>
#include <QRect>

class CEPFView;
class QPainter;

class CWidgetHandler : public QWidget
{
    Q_OBJECT
public:
    CWidgetHandler(QWidget *parent = 0);

    static CWidgetHandler* get(CEPFView* p);
    void destroy();

    virtual bool eventFilter(QObject *p, QEvent *ev);

    void renderWidget(QObject* obj, QPainter* p);

protected:

    virtual bool event(QEvent *ev);

private:

    //bool isActiveObject(QObject* obj);

signals:

public slots:

    void createTextField();
    void updateWidgetGeometry(QRect r);
    void showWidget();
    void hideWidget();
    void destroyWidget();

private:
    QMap<QObject*,QWidget*> m_widgets;
    //QMutex m_mWidgetMutex;
};

#endif // CWIDGETHANDLER_H
