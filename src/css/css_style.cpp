#include "css_style.h"
#include <QRegExp>

using namespace CSS;

Stylesheet::Stylesheet(QString css)
{
    QRegExp newlines("[\n\r]+");
    QRegExp tabs("[\t]+");
    css.replace(newlines,"");
    css.replace(tabs," ");

    QString selector,propdata,propkey,propvalue;
    QStringList proplist,proper;
    CSSSelector* s;
    CSSProperty* prop;

    QRegExp outerspaces("(^ +| +$)");

    QRegExp propgroupfinder("([^\\{]+)\\{([^\\}]+)\\}");

    int offset = 0;


    while (css.indexOf(propgroupfinder,offset) != -1)
    {
        selector = propgroupfinder.cap(1).replace(outerspaces,"");
        propdata = propgroupfinder.cap(2);

        if (m_selectors.contains(selector))
            s = m_selectors[selector];
        else
            s = new CSSSelector();

        proplist = propdata.split(";");

        for (int i=0;i<proplist.size();i++)
        {
            proper = proplist[i].split(":");
            if (proper.size() == 2)
            {
                propkey = proper[0].replace(outerspaces,"");
                propvalue = proper[1].replace(outerspaces,"");
                prop = new CSSProperty(propvalue);

                s->setProperty(propkey,prop);
            }
        }

        offset += propgroupfinder.cap(0).size();
    }
}

Stylesheet::~Stylesheet()
{

}

