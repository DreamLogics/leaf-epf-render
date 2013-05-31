#include "css_painters.h"
#include <QPainter>
#include "cdocument.h"
#include <QPen>
#include <QtCore/qmath.h>

using namespace CSS;

void paintBorder(QPainter* pPainter, QRectF qrBorderRect, double dWidth, QString strColor, QString strStyle)
{

}

void paintBackgroundColor(QPainter* pPainter, QRectF qrBgRect, QString strColor)
{

}

void paintBackgroundImage(QPainter* pPainter, QRectF qrBgRect, QRectF qrSize, QString strSrc, CDocument* pDocument)
{

}

void paintOuterGlow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize)
{
    int compmode = pPainter->compositionMode();
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
    int compmode = pPainter->compositionMode();
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
    int compmode = pPainter->compositionMode();
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
