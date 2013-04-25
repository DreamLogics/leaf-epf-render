#ifndef CSS_STYLE_H
#define CSS_STYLE_H

#include <QMap>
#include <QString>
#include "cbaseobject.h"
#include "clayer.h"
#include "csection.h"

namespace CSS
{

class CSSProperty
{
public:
    CSSProperty(QString value);

    QString toString();
    int toInt();
    double toDouble();

private:
    QString m_sValue;
    bool m_bScale;
};

class CSSSelector
{
public:
    CSSSelector();
    ~CSSSelector();

    CSSProperty* property(QString key);
    void setProperty(QString key, CSSProperty* prop);

private:
    QMap<QString,CSSProperty*> m_props;
};

class Stylesheet
{
public:
    Stylesheet(QString css);
    ~Stylesheet();

    CSSProperty* property(QString selector, QString key);
    CSSProperty* property(CBaseObject* obj, QString key);
    CSSProperty* property(CLayer* l, QString key);
    CSSProperty* property(CSection* s, QString key);

private:
    QMap<QString,CSSSelector*> m_selectors;
};

}

#endif // CSS_STYLE_H
