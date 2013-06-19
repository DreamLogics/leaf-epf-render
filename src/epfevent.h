#ifndef EPFEVENT_H
#define EPFEVENT_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>

class EPFEvent
{
public:

    EPFEvent(QString event);

    void setParameters(QStringList parameters);

    QString event() const;
    QString parameter(int index) const;

private:

    QString m_strEvent;
    QStringList m_Parameters;
};

class EPFComponent;

class EPFConnection
{
public:
    EPFConnection(QString eventstr, EPFComponent* target, QString functionstr);

    QString event() const;
    QString function() const;
    EPFComponent* target() const;
    QStringList param(QStringList event_params);

private:

    QString removeSpaces(QString str);

private:
    QString m_strEvent;
    QStringList m_EventParam;
    QStringList m_FuncParam;
    QString m_strFunction;
    EPFComponent* m_pTarget;
};

class EPFComponent
{
public:
    ~EPFComponent() {}

    virtual void onEPFEvent(EPFEvent* ev) = 0;
    void sendEvent(QString event,QStringList parameters=QStringList());
    void addConnection(EPFComponent* listener, QString event, QString function);

private:
    QMap<QString, QList<EPFConnection*> > m_Connections;
};

#endif // EPFEVENT_H
