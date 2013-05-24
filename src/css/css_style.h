/****************************************************************************
**
** LEAF EPF Render engine
** http://leaf.dreamlogics.com/
**
** Copyright (C) 2013 DreamLogics
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CSS_STYLE_H
#define CSS_STYLE_H

#include <QMap>
#include <QString>
#include "cbaseobject.h"
#include "clayer.h"
#include "csection.h"
#include "clayout.h"

namespace CSS
{

static QStringList color_props = (QStringList() << "color" << "background-color" << "color-overlay" << "color-multiply" << "color-replace");
static QStringList int_props = (QStringList() << "top" << "bottom" << "left" << "right" << "height" << "width" << "margin-top" << "margin-bottom" << "margin-left"
                                << "margin-right" << "padding-top" << "padding-bottom" << "padding-left" << "padding-right");
static QStringList double_props = (QStringList() << "opacity");
static QStringList redraw_props = (QStringList() << "height" << "width" << "background-image" << "opacity");


class Stylesheet;

class CSSProperty
{
public:
    CSSProperty(QString value, Stylesheet* css, bool scales, bool isHeightProp, bool null=false);

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
    bool m_bHeightProp;
    Stylesheet* m_pCSS;
};

class CSSSelector
{
public:
    CSSSelector();
    ~CSSSelector();

    CSSProperty* property(QString key);
    void setProperty(QString key, CSSProperty* prop);
    QStringList properties();

private:
    QMap<QString,CSSProperty*> m_props;
};

class Stylesheet
{
public:
    Stylesheet(QString css);
    Stylesheet(CLayout* layout, int target_height, int target_width);
    ~Stylesheet();

    CSSProperty* property(QString selector, QString key);
    CSSProperty* property(CBaseObject* obj, QString key);
    CSSProperty* property(CLayer* l, QString key);
    CSSProperty* property(CSection* s, QString key);

    QStringList properties(QString selector);
    QStringList properties(CBaseObject* obj);
    QStringList properties(CLayer* l);
    QStringList properties(CSection* s);

    void setScale(double height_factor, double width_factor);
    double heightScaleFactor();
    double widthScaleFactor();

    void addCSS(QString css);

private:

    void parse(QString css);

private:
    QMap<QString,CSSSelector*> m_selectors;
    double m_dHSF;
    double m_dWSF;
    QStringList m_HeightProps;
};

}

#endif // CSS_STYLE_H
