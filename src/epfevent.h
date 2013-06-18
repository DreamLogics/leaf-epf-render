#ifndef EPFEVENT_H
#define EPFEVENT_H

#include <QString>
#include <QStringList>

class EPFEvent
{
public:

    EPFEvent(QString event, QStringList parameters);

    QString event() const;
    QString parameter(int index) const;

private:

    QString m_strEvent;
    QStringList m_Parameters;
};

class EPFComponent
{
public:
    ~EPFComponent() {}

    virtual void onEPFEvent(EPFEvent* ev) = 0;
};

#endif // EPFEVENT_H
