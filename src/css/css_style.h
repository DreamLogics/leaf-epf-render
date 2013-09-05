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
#include "cdocument.h"

namespace CSS
{

static QStringList color_props = (QStringList() << "color" << "background-color");
static QStringList int_props = (QStringList() << "top" << "bottom" << "left" << "right" << "height" << "width" << "margin-top" << "margin-bottom" << "margin-left"
                                << "margin-right" << "padding-top" << "padding-bottom" << "padding-left" << "padding-right");
static QStringList double_props = (QStringList() << "opacity");
static QStringList redraw_props = (QStringList() << "height" << "width" << "background-image" << "opacity" << "render-mode");

static QStringList height_props = (QStringList() << "height" << "top" << "bottom" << "margin-top" << "margin-bottom" << "padding-top" << "padding-bottom" << "min-height" << "max-height");

//static QStringList combined_props = (QStringList() << "margin" << "padding");

enum RenderMode
{
    rmNone = 0,
    rmNormal,
    rmOverlay,
    rmMultiply,
    rmReplace,
    rmScreen
};

enum ScaleMode
{
    smNone = 0,
    smScale,
    smScaleWidth,
    smScaleHeight
};

enum ColorFormat
{
    cfHex = 0,
    cfRgb,
    cfRgba
};

RenderMode renderModeFromString(QString str);
QColor stringToColor(QString color);
QString colorToString(QColor color, ColorFormat format=cfHex);

class Stylesheet;

class Property
{
public:
    Property(QString value, Stylesheet* css, /*bool scales*/ScaleMode scale, bool isHeightProp, bool null=false);

    QString toString();
    int toInt();
    double toDouble();
    QColor toColor();

    QString value() const; //unscaled

    bool isNull();

    /*void setValue(QString val, bool scale=false);
    void setValue(int val, bool scale=false);
    void setValue(double val, bool scale=false);*/
    void setValue(QString val, ScaleMode scale=smNone);
    void setValue(int val, ScaleMode scale=smNone);
    void setValue(double val, ScaleMode scale=smNone);
    void setValue(QColor val, ColorFormat format=cfHex);

    //bool scales();
    ScaleMode scaleMode();

private:
    QString m_sValue;
    //bool m_bScale;
    ScaleMode m_eScale;
    bool m_bNull;
    bool m_bHeightProp;
    Stylesheet* m_pCSS;
    bool m_bReadOnly;

    friend class Stylesheet;
};

class Selector
{
public:
    Selector(Stylesheet*);
    ~Selector();

    Property* property(QString key);
    void setProperty(QString key, Property* prop);
    QStringList properties();

private:
    QMap<QString,Property*> m_props;
    Stylesheet* m_pCSS;

};

class Stylesheet
{
public:
    Stylesheet(QString css, CDocument* doc);
    Stylesheet(CLayout* layout, int target_height, int target_width, CDocument* doc);
    ~Stylesheet();

    Property* property(QString selector, QString key);
    Property* property(CBaseObject* obj, QString key);
    Property* property(CLayer* l, QString key);
    Property* property(CSection* s, QString key);

    Selector* selector(QString selector);
    Selector* selector(CBaseObject* obj);

    void setScale(double height_factor, double width_factor);
    double heightScaleFactor() const;
    double widthScaleFactor() const;

    void addCSS(QString css);

    void setVariable(QString,QString);
    QString variable(QString);

private:

    void parse(QString css);

private:
    QMap<QString,Selector*> m_selectors;
    double m_dHSF;
    double m_dWSF;
    CDocument* m_pDocument;
    QMap<QString,QString> m_variables;
};

}

#endif // CSS_STYLE_H
