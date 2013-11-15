#ifndef CWIDGETHANDLER_H
#define CWIDGETHANDLER_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QRect>

class CEPFView;
class QPainter;

class CWidgetHandler : public QObject
{
    Q_OBJECT
public:
    CWidgetHandler(CEPFView* p, QObject *parent = 0);

    static CWidgetHandler* get(CEPFView* p);
    void destroy();

    virtual bool eventFilter(QObject *p, QEvent *ev);

    void renderWidget(QObject* obj, QPainter* p);

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
    CEPFView* m_pView;
};

#endif // CWIDGETHANDLER_H
