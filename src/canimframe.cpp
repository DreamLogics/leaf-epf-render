#include "canimframe.h"
//#include "css/css_style.h"

CAnimFrame::CAnimFrame(CSection* section) : m_pSection(section)//CSectionRender *sr) : m_pSR(sr)
{

}

void CAnimFrame::setPropertiesForObject(CBaseObject *object, QMap<QString, QString> map)
{
    if (m_Objects.contains(object))
        m_Objects[object] = map;
    else
        m_Objects.insert(object,map);
}

void CAnimFrame::apply()
{
    QString css;
    QMap<QString,QString> props;
    QList<QString> proplist;
    QList<CBaseObject*> objects = m_Objects.keys();
    CBaseObject* obj;
    for (int i=0;i<objects.size();i++)
    {
        obj = objects[i];
        props = m_Objects[obj];
        proplist = props.keys();
        css = "";
        for (int n=0;n<proplist.size();n++)
        {
            css += proplist[n] + ":" + props[proplist[n]] + ";";
        }
        obj->setCSSOverride(css);
    }

    //update view
    //m_pSR->updateView();
    //m_pSection->renderer()->updateView();
    m_pSection->document()->renderview()->update();
}

bool CAnimFrame::hasObjectProps(CBaseObject *obj)
{
    return m_Objects.contains(obj);
}

CSection* CAnimFrame::section()
{
    return m_pSection;
}

QMap<CBaseObject*,QMap<QString,QString> > CAnimFrame::objectPropMap()
{
    return m_Objects;
}
