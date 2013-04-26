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
    CSSProperty(QString value, Stylesheet* css, bool scales, bool null=false);

    QString toString();
    int toInt();
    double toDouble();

    bool isNull();

    void setValue(QString val, bool scale=false);
    void setValue(int val, bool scale=false);
    void setValue(double val, bool scale=false);


private:
    QString m_sValue;
    bool m_bScale;
    bool m_bNull;
    Stylesheet* m_pCSS;
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

    void setScale(double height_factor, double width_factor);
    double heightScaleFactor();
    double widthScaleFactor();

private:
    QMap<QString,CSSSelector*> m_selectors;
    double m_dHSF;
    double m_dWSF;
};

}

#endif // CSS_STYLE_H
