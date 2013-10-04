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

#include "css_painters.h"
#include <QPainter>
#include "../cdocument.h"
#include "../cbaseobject.h"
#include <QPen>
#include <QtCore/qmath.h>
#include <QImage>
#include <QDebug>
#include <QLinearGradient>
#include <qmath.h>
#include <QRadialGradient>

namespace CSS
{


void paintBorder(QPainter* pPainter, QRectF qrBorderRect, CSS::Property bordertop, CSS::Property borderbottom, CSS::Property borderleft, CSS::Property borderright)
{
    QString borderstyle;
    QRectF r;
    QRegExp borderreg("([0-9]+)[^ ]* +([a-z]+) +(#[0-9a-fA-F]{3,6}|rgb *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)|rgba *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\))");

    if (!bordertop.isNull() && borderreg.indexIn(bordertop.toString()) != -1)
    {
        borderstyle = borderreg.cap(2);
        r.setHeight(borderreg.cap(1).toInt());
        r.setWidth(qrBorderRect.width());
        pPainter->fillRect(r,stringToColor(borderreg.cap(3)));
    }

    if (!borderbottom.isNull() && borderreg.indexIn(borderbottom.toString()) != -1)
    {
        borderstyle = borderreg.cap(2);
        r.setHeight(borderreg.cap(1).toInt());
        r.setWidth(qrBorderRect.width());
        r.moveBottom(qrBorderRect.height());
        pPainter->fillRect(r,stringToColor(borderreg.cap(3)));
    }

    if (!borderleft.isNull() && borderreg.indexIn(borderleft.toString()) != -1)
    {
        borderstyle = borderreg.cap(2);
        r.setWidth(borderreg.cap(1).toInt());
        r.setHeight(qrBorderRect.height());
        pPainter->fillRect(r,stringToColor(borderreg.cap(3)));
    }

    if (!borderright.isNull() && borderreg.indexIn(borderright.toString()) != -1)
    {
        borderstyle = borderreg.cap(2);
        r.setWidth(borderreg.cap(1).toInt());
        r.setHeight(qrBorderRect.height());
        r.moveRight(qrBorderRect.width());
        pPainter->fillRect(r,stringToColor(borderreg.cap(3)));
    }
}

void paintBackgroundColor(QPainter* pPainter, QRectF qrBgRect, QColor strColor)
{
    pPainter->fillRect(qrBgRect,strColor);
}

void paintBackgroundImage(QPainter* pPainter, QRectF qrBgRect, QString strSize, QString strSrc, CDocument* pDocument)
{
    QRectF qrSize;

    //qDebug() << "paintBackgroundImage :" << strSrc;

    QRegExp srcreg("[\"'\\(\\)]");

    if (strSrc.left(4) == "src(")
        strSrc = strSrc.mid(4);

    strSrc = strSrc.replace(srcreg,"");

    //qDebug() << "paintBackgroundImage :" << strSrc;

    QImage img = QImage::fromData(pDocument->resource(strSrc));

    if (img.isNull())
        return;

    //qDebug() << "paintBackgroundImage - size :" << strSize;

    if (strSize != "")
    {
        QRegExp percreg("([0-9]+)%");
        QRegExp sizereg("([0-9]+)px +([0-9]+)px");
        QRegExp sizeautoreg("([0-9]+)px");
        QString propstr = strSize;
        if (propstr == "cover")
            img = img.scaled(qrBgRect.width(),qrBgRect.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        else if (propstr == "contain")
            img = img.scaled(qrBgRect.width(),qrBgRect.height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        else if (sizereg.indexIn(propstr) != -1)
            img = img.scaled(sizereg.cap(1).toInt(),sizereg.cap(2).toInt(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        else if (sizeautoreg.indexIn(propstr) != -1)
            img = img.scaledToWidth(sizeautoreg.cap(1).toInt(),Qt::SmoothTransformation);
        else if (percreg.indexIn(propstr) != -1)
            img = img.scaled(((float)(percreg.cap(1).toInt())/100)*img.width(),((float)(percreg.cap(2).toInt())/100)*img.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        else
            img = img.scaled(qrBgRect.width(),qrBgRect.height(),Qt::KeepAspectRatio);
    }

    qrSize = img.rect();

    if (img.height() > qrBgRect.height())
        qrSize.setHeight(qrBgRect.height());
    if (img.width() > qrBgRect.width())
        qrSize.setWidth(qrBgRect.width());

    pPainter->drawImage(qrBgRect,img,qrSize);
}

void paintOuterGlow(QPainter* pPainter, QRectF qrRect, QColor strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize)
{
    QPainter::CompositionMode compmode = pPainter->compositionMode();
    switch (iRenderMode)
    {
    case rmNone:
        return;
        break;
    case rmOverlay:
        pPainter->setCompositionMode(QPainter::CompositionMode_Overlay);
        break;
    case rmMultiply:
        pPainter->setCompositionMode(QPainter::CompositionMode_Multiply);
        break;
    case rmReplace:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        break;
    case rmScreen:
        pPainter->setCompositionMode(QPainter::CompositionMode_Screen);
        break;
    default:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        break;
    }

    QColor c(strColor);
    QPen p(c);
    p.setWidth(iSpread);
    pPainter->setPen(p);

    pPainter->setOpacity(dOpacity);

    pPainter->drawRect(qrRect);

    double stepsize = dOpacity / iSize;
    double steps;
    QRectF drawrect = qrRect;
    drawrect.adjust(-iSpread,-iSpread,iSpread,iSpread);
    p.setWidth(1);

    for (steps=dOpacity;steps > 0;steps-=stepsize)
    {
        pPainter->setOpacity(steps);
        pPainter->drawRect(drawrect);
        drawrect.adjust(-1,-1,1,1);
    }

    pPainter->setCompositionMode(compmode);
}

void paintInnerGlow(QPainter* pPainter, QRectF qrRect, QColor strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize)
{
    QPainter::CompositionMode compmode = pPainter->compositionMode();
    switch (iRenderMode)
    {
    case rmNone:
        return;
        break;
    case rmOverlay:
        pPainter->setCompositionMode(QPainter::CompositionMode_Overlay);
        break;
    case rmMultiply:
        pPainter->setCompositionMode(QPainter::CompositionMode_Multiply);
        break;
    case rmReplace:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        break;
    case rmScreen:
        pPainter->setCompositionMode(QPainter::CompositionMode_Screen);
        break;
    default:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        break;
    }

    QRectF drawrect = qrRect;
    drawrect.adjust(iSpread,iSpread,-iSpread,-iSpread);

    QColor c(strColor);
    QPen p(c);
    p.setWidth(iSpread);
    pPainter->setPen(p);

    pPainter->setOpacity(dOpacity);

    pPainter->drawRect(drawrect);

    double stepsize = dOpacity / iSize;
    double steps;
    p.setWidth(1);

    for (steps=dOpacity;steps > 0;steps-=stepsize)
    {
        drawrect.adjust(1,1,-1,-1);
        pPainter->setOpacity(steps);
        pPainter->drawRect(drawrect);
    }

    pPainter->setCompositionMode(compmode);
}

void paintDropShadow(QPainter* pPainter, QRectF qrRect, QColor strColor, RenderMode iRenderMode, double dOpacity, int iLightDir, int iDistance, int iSpread, int iSize)
{
    QPainter::CompositionMode compmode = pPainter->compositionMode();
    switch (iRenderMode)
    {
    case rmNone:
        return;
        break;
    case rmOverlay:
        pPainter->setCompositionMode(QPainter::CompositionMode_Overlay);
        break;
    case rmMultiply:
        pPainter->setCompositionMode(QPainter::CompositionMode_Multiply);
        break;
    case rmReplace:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        break;
    case rmScreen:
        pPainter->setCompositionMode(QPainter::CompositionMode_Screen);
        break;
    default:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        break;
    }

    QRectF drawrect = qrRect;
    QColor c(strColor);
    QPen p(c);
    pPainter->setPen(p);

    pPainter->setOpacity(dOpacity);

    double yd,xd;
    double rad = iLightDir * (M_PI/180);

    yd = qCos(rad);
    xd = qSin(rad);

    drawrect.adjust(iDistance*xd,iDistance*yd,iDistance*xd,iDistance*yd);

    pPainter->fillRect(drawrect,c);

    double stepsize = dOpacity / iSize;
    double steps;

    drawrect.adjust(-iSpread,-iSpread,iSpread,iSpread);
    p.setWidth(1);

    for (steps=dOpacity;steps > 0;steps-=stepsize)
    {
        pPainter->setOpacity(steps);
        pPainter->drawRect(drawrect);
        drawrect.adjust(-1,-1,1,1);
    }

    pPainter->setCompositionMode(compmode);
}

void paintColorOverlay(QPainter *pPainter, QRectF qrRect, QColor mixcolor, RenderMode iRenderMode)
{

    QPainter::CompositionMode mode = pPainter->compositionMode();

    switch (iRenderMode) {
    case rmNormal:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        break;
    case rmOverlay:
        pPainter->setCompositionMode(QPainter::CompositionMode_Overlay);
        break;
    case rmMultiply:
        pPainter->setCompositionMode(QPainter::CompositionMode_Multiply);
        break;
    case rmReplace:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        break;
    case rmScreen:
        pPainter->setCompositionMode(QPainter::CompositionMode_Screen);
        break;
    default:
        pPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        break;
    }

    pPainter->fillRect(qrRect,mixcolor);

    pPainter->setCompositionMode(mode);
}

void paintBorder(QPainter* pPainter, CBaseObject* pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    paintBorder(pPainter,r,css->property(pObj,"border-top"),css->property(pObj,"border-bottom"),css->property(pObj,"border-left"),css->property(pObj,"border-right"));
}

void paintBackgroundColor(QPainter* pPainter, CBaseObject* pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    if (!css->property(pObj,"background-color").isNull())
    {
        CSS::paintBackgroundColor(pPainter,r,css->property(pObj,"background-color").toColor());
    }
}

void paintBackgroundImage(QPainter* pPainter, CBaseObject* pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    if (!css->property(pObj,"background-image").isNull())
    {
        QString size;

        if (!css->property(pObj,"background-size").isNull())
            size = css->property(pObj,"background-size").toString();
        else if (!css->property(pObj,"background-image-size").isNull())
            size = css->property(pObj,"background-image-size").toString();

        CSS::paintBackgroundImage(pPainter,r,size,css->property(pObj,"background-image").toString(),pObj->document());
    }

}

void paintBackgroundGradient(QPainter *pPainter, CBaseObject *pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    paintBackgroundGradient(pPainter, r, css->property(pObj,"background-gradient"),css->property(pObj,"background-gradient-type"),css->property(pObj,"background-gradient-spread"),css->property(pObj,"background-gradient-angle"),css->property(pObj,"background-gradient-center"),css->property(pObj,"background-gradient-focal"));
}

void paintBackgroundGradient(QPainter *pPainter, QRectF r, CSS::Property bggrad, CSS::Property gradtypeprop, CSS::Property gradspreadprop, CSS::Property anglegrad, CSS::Property gradcenter, CSS::Property gradfocal)
{
    //CSS::Stylesheet* css = pObj->document()->stylesheet();

    if (!bggrad.isNull())
    {
        QRegExp gradpointreg("\\( *([0-9\\.]+) *, *(#[0-9a-fA-F]{3,6}|rgb *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)|rgba *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)) *\\)");
        QString gradtype = gradtypeprop.toString();
        QString gradspread = gradspreadprop.toString();
        int offset=0;
        QString propval = bggrad.toString();

        int degree = anglegrad.toInt();
        int radius = degree;
        if (degree < -360 || degree > 360)
            degree = 0;
        if (degree < 0)
            degree = 360 + degree;
        int part = degree / 45;
        degree = degree % 45;
        double radian = degree * M_PI / 180;
        int x1,x2;
        int y1,y2;

        if (gradtype == "radial")
        {
            QRadialGradient rgrad;
            while (gradpointreg.indexIn(propval,offset) != -1)
            {
                QColor c = stringToColor(gradpointreg.cap(2));
                rgrad.setColorAt(gradpointreg.cap(1).toDouble(),c);
                offset += gradpointreg.cap(0).size();
            }

            QRegExp xyreg("([0-9\\.]+) +([0-9\\.]+)");
            double centerx,centery;
            double focalx,focaly;

            if (xyreg.indexIn(gradcenter.toString()) != -1)
            {
                centerx = r.width() * xyreg.cap(1).toDouble();
                centery = r.height() * xyreg.cap(2).toDouble();
            }
            else
            {
                centerx = r.width() / 2;
                centery = r.height() / 2;
            }
            if (xyreg.indexIn(gradfocal.toString()) != -1)
            {
                focalx = r.width() * xyreg.cap(1).toDouble();
                focaly = r.height() * xyreg.cap(2).toDouble();
            }
            else
            {
                focalx = r.width() / 2;
                focaly = r.height() / 2;
            }

            if (radius == 0)
                radius = 360;

            if (gradspread == "repeat")
                rgrad.setSpread(QGradient::RepeatSpread);
            else if (gradspread == "reflect")
                rgrad.setSpread(QGradient::ReflectSpread);

            rgrad.setCenter(centerx,centery);
            rgrad.setFocalPoint(focalx,focaly);
            rgrad.setRadius(radius);

            pPainter->fillRect(r,rgrad);
        }
        else
        {
            QLinearGradient lgrad;
            while (gradpointreg.indexIn(propval,offset) != -1)
            {
                QColor c = stringToColor(gradpointreg.cap(2));
                lgrad.setColorAt(gradpointreg.cap(1).toDouble(),c);
                offset += gradpointreg.cap(0).size();
            }

            x1 = x2 = r.width()/2;
            y1 = 0;
            y2 = r.height();

            if (!anglegrad.isNull())
            {
                double delta = qTan(radian);

                switch (part)
                {
                case 1:
                    delta *= r.width()/2;
                    x1 = r.width();
                    x2 = 0;
                    y1 += delta;
                    y2 -= delta;
                    break;
                case 2:
                    delta *= r.width()/2;
                    x1 = r.width();
                    x2 = 0;
                    y1 = r.height()/2 + delta;
                    y2 = r.height()/2 - delta;
                    break;
                case 3:
                    delta *= r.height()/2;
                    y1 = y2;
                    y2 = 0;
                    x1 = r.width() - delta;
                    x2 = delta;
                    break;
                case 4:
                    delta *= r.height()/2;
                    y1 = y2;
                    y2 = 0;
                    x1 = r.width()/2 - delta;
                    x2 = r.width()/2 + delta;
                    break;
                case 5:
                    delta *= r.width()/2;
                    x1 = 0;
                    x2 = r.width();
                    y1 = r.height() - delta;
                    y2 = delta;
                    break;
                case 6:
                    delta *=  r.width()/2 + r.height()/2;
                    x1 = 0;
                    x2 = r.width();
                    y1 = r.height()/2 - delta;
                    y2 = r.height()/2 + delta;
                    break;
                case 7:
                    delta *=  r.height()/2;
                    x1 = r.width()/2 - delta;
                    x2 = r.width()/2 + delta;
                    break;
                default:
                    delta *=  r.height()/2;
                    x1 += delta;
                    x2 -= delta;
                    break;
                }
            }

            if (gradspread == "repeat")
                lgrad.setSpread(QGradient::RepeatSpread);
            else if (gradspread == "reflect")
                lgrad.setSpread(QGradient::ReflectSpread);

            lgrad.setStart(x1,y1);
            lgrad.setFinalStop(x2,y2);
            pPainter->fillRect(r,lgrad);
        }

    }
}

void paintOuterGlow(QPainter* pPainter, CBaseObject* pObj)
{

}
void paintInnerGlow(QPainter* pPainter, CBaseObject* pObj)
{

}
void paintDropShadow(QPainter* pPainter, CBaseObject* pObj)
{

}
void paintColorOverlay(QPainter* pPainter, CBaseObject* pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    if (!css->property(pObj,"color-overlay").isNull())
    {
        QRegExp cov("(#[0-9a-fA-F]{3,6}|rgb *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)|rgba *\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+) *, *([0-9]+) *\\)) +([a-zA-Z]+)");
        if (cov.indexIn(css->property(pObj,"color-overlay").toString()) != -1)
        {
            //qDebug() << "color overlay" << cov.cap(1) << cov.cap(cov.captureCount());
            CSS::paintColorOverlay(pPainter,r,stringToColor(cov.cap(1)),CSS::renderModeFromString(cov.cap(cov.captureCount())));
        }
    }
}

}
