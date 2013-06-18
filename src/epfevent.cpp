#include "epfevent.h"

EPFEvent::EPFEvent(QString event, QStringList parameters) : m_strEvent(event), m_Parameters(parameters)
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
