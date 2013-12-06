#include "epfevent.h"
#include <QDebug>

EPFEvent::EPFEvent(QString event) : m_strEvent(event)
{

}

QString EPFEvent::event() const
{
    return m_strEvent;
}

QString EPFEvent::parameter(int index) const
{
    if (index < 0 || index >= m_Parameters.size())
        return "";
    return m_Parameters[index];
}

void EPFEvent::setParameters(QStringList parameters)
{
    m_Parameters = parameters;
}

void EPFComponent::addConnection(EPFComponent *listener, QString event, QString function)
{
    QMap<QString, QList<EPFConnection*> >::iterator it;
    QList<EPFConnection*> list;
    EPFConnection* connection = new EPFConnection(event,listener,function);;

    //qDebug()() << "add connection" << connection->event() << function;
    it = m_Connections.find(connection->event());

    if (it == m_Connections.end())
    {
        //insert
        list.append(connection);
        m_Connections.insert(connection->event(),list);
    }
    else
    {
        list = it.value();
        list.append(connection);
        m_Connections[connection->event()] = list;
    }
}

void EPFComponent::sendEvent(QString event, QStringList parameters)
{
    QMap<QString, QList<EPFConnection*> >::iterator it;
    it = m_Connections.find(event);

    //qDebug()() << event;

    if (it == m_Connections.end())
        return;

    //qDebug()() << "has connections";

    QList<EPFConnection*> list = it.value();
    EPFConnection* connection;
    QStringList param;


    EPFEvent* ev;

    for (int i=0;i<list.size();i++)
    {
        connection = list[i];
        //qDebug()() << "connection" << connection->event() << connection->function();
        param = connection->param(parameters);
        ev = new EPFEvent(connection->function());
        ev->setParameters(param);
        connection->target()->onEPFEvent(ev);
        delete ev;
    }
}


EPFConnection::EPFConnection(QString eventstr, EPFComponent *target, QString functionstr)
{
    QRegExp funcreg("([a-zA-Z]+)\\(([^\\)]+)\\)");

    eventstr = removeSpaces(eventstr);
    functionstr = removeSpaces(functionstr);

    if (funcreg.indexIn(eventstr) != -1)
    {
        m_strEvent = funcreg.cap(1);
        m_EventParam = funcreg.cap(2).split(",");
    }
    else if (eventstr.endsWith("()"))
        m_strEvent = eventstr.left(eventstr.size()-2);
    else
        m_strEvent = eventstr;

    if (funcreg.indexIn(functionstr) != -1)
    {
        m_strFunction = funcreg.cap(1);
        m_FuncParam = funcreg.cap(2).split(",");
    }
    else if (functionstr.endsWith("()"))
        m_strFunction = functionstr.left(functionstr.size()-2);
    else
        m_strFunction = functionstr;

    m_pTarget = target;
}

QString EPFConnection::removeSpaces(QString str)
{
    int i;
    bool inesc=false;
    QString nstr;
    for (i=0;i<str.size();i++)
    {
        if (inesc)
        {
            if (str[i] == '"' || str[i] == '\'')
                inesc=false;
            else
                nstr += str[i];

        }
        else
        {
            if (str[i] == '"' || str[i] == '\'')
                inesc=true;
            else if (str[i] != ' ')
                nstr+=str[i];
        }
    }
    return nstr;
}

QString EPFConnection::event() const
{
    return m_strEvent;
}

QString EPFConnection::function() const
{
    return m_strFunction;
}

QStringList EPFConnection::param(QStringList event_params)
{
    QStringList params = event_params;

    if (params.size() < m_FuncParam.size())
    {
        for (int i = params.size();i<m_FuncParam.size();i++)
            params << m_FuncParam[i];
    }

    for (int i=0;i<params.size();i++)
    {
        if (i < m_EventParam.size())
        {
            if (m_EventParam[i] != m_FuncParam[i])
                params[i] = m_FuncParam[i];
        }
        else
            params[i] = m_FuncParam[i];
    }

    //qDebug()() << params << m_EventParam << m_FuncParam;

    return params;
}

EPFComponent* EPFConnection::target() const
{
    return m_pTarget;
}
