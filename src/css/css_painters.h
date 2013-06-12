#ifndef CSS_PAINTERS_H
#define CSS_PAINTERS_H

#include "css_style.h"
#include <QRectF>
class QPainter;
class CDocument;

namespace CSS
{

void paintBorder(QPainter* pPainter, QRectF qrBorderRect, double dWidth, QString strColor, QString strStyle);
void paintBackgroundColor(QPainter* pPainter, QRectF qrBgRect, QString strColor);
void paintBackgroundImage(QPainter* pPainter, QRectF qrBgRect, QString strSize, QString strSrc, CDocument* pDocument);
void paintOuterGlow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize);
void paintInnerGlow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize);
void paintDropShadow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iLightDir, int iDistance, int iSpread, int iSize);
void paintColorOverlay(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity);

}

#endif // CSS_PAINTERS_H
