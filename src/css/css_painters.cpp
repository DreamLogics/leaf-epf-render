#include "css_painters.h"
#include <QPainter>
#include "cdocument.h"
#include "cbaseobject.h"
#include <QPen>
#include <QtCore/qmath.h>
#include <QImage>
#include <QDebug>
#include <QLinearGradient>

namespace CSS
{


void paintBorder(QPainter* pPainter, QRectF qrBorderRect, double dWidth, QString strColor, QString strStyle)
{

}

void paintBackgroundColor(QPainter* pPainter, QRectF qrBgRect, QString strColor)
{
    pPainter->fillRect(qrBgRect,QColor(strColor));
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

void paintOuterGlow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize)
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

void paintInnerGlow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize)
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

void paintDropShadow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iLightDir, int iDistance, int iSpread, int iSize)
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

void paintColorOverlay(QPainter *pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity)
{
    double opac = pPainter->opacity();
    pPainter->setOpacity(dOpacity);
    QColor mixcolor(strColor);
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
    pPainter->setOpacity(opac);
}

void paintBorder(QPainter* pPainter, CBaseObject* pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    if (!css->property(pObj,"border")->isNull())
    {
        QRegExp cov("([0-9]+)[a-zA-Z ]+([a-zA-Z]+) +(#[0-9a-fA-F]{3,6})");
        if (cov.indexIn(css->property(pObj,"border")->toString()) != -1)
        {
            paintBorder(pPainter,r,cov.cap(1).toDouble(),cov.cap(2),cov.cap(3));
        }
    }
}

void paintBackgroundColor(QPainter* pPainter, CBaseObject* pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    if (!css->property(pObj,"background-color")->isNull())
    {
        CSS::paintBackgroundColor(pPainter,r,css->property(pObj,"background-color")->toString());
    }
}

void paintBackgroundImage(QPainter* pPainter, CBaseObject* pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    if (!css->property(pObj,"background-image")->isNull())
    {
        QString size;

        if (!css->property(pObj,"background-size")->isNull())
            size = css->property(pObj,"background-size")->toString();
        else if (!css->property(pObj,"background-image-size")->isNull())
            size = css->property(pObj,"background-image-size")->toString();

        CSS::paintBackgroundImage(pPainter,r,size,css->property(pObj,"background-image")->toString(),pObj->document());
    }

}

void paintBackgroundGradient(QPainter *pPainter, CBaseObject *pObj)
{
    CSS::Stylesheet* css = pObj->document()->stylesheet();
    QRectF r = pObj->boundingRect();
    r.moveTop(0);
    r.moveLeft(0);

    CSS::Property* bggrad = css->property(pObj,"background-gradient");

    if (!bggrad->isNull())
    {
        QRegExp gradpointreg("\\( *([0-9\\.]+) *, *(#[0-9a-fA-F]{3,6}) *, *([0-9\\.]+) *\\)");
        QString gradtype = css->property(pObj,"background-gradient")->toString();
        int offset=0;
        QString propval = bggrad->toString();

        if (gradtype == "radial")
        {

        }
        else
        {
            QLinearGradient lgrad;
            while (gradpointreg.indexIn(propval,offset) != -1)
            {
                QColor c(gradpointreg.cap(2));
                c.setAlphaF(gradpointreg.cap(3).toDouble());
                lgrad.setColorAt(gradpointreg.cap(1).toDouble(),c);
                offset += gradpointreg.cap(0).size();
            }
            lgrad.setStart(r.width()/2,0);
            lgrad.setFinalStop(r.width()/2,r.height());
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

    if (!css->property(pObj,"color-overlay")->isNull())
    {
        QRegExp cov("(#[0-9a-fA-F]{3,6}) +([a-zA-Z]+) +([0-9\\.]+)");
        if (cov.indexIn(css->property(pObj,"color-overlay")->toString()) != -1)
        {
            CSS::paintColorOverlay(pPainter,r,cov.cap(1),CSS::renderModeFromString(cov.cap(2)),cov.cap(3).toDouble());
        }
    }
}

}
