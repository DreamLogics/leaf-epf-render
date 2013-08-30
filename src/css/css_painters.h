#ifndef CSS_PAINTERS_H
#define CSS_PAINTERS_H

#include "css_style.h"
#include <QRectF>
class QPainter;
//class CDocument;
class CBaseObject;

namespace CSS
{

void paintBorder(QPainter* pPainter, QRectF qrBorderRect, CSS::Property* bordertop, CSS::Property* borderbottom, CSS::Property* borderleft, CSS::Property* borderright);
void paintBackgroundColor(QPainter* pPainter, QRectF qrBgRect, QString strColor);
void paintBackgroundImage(QPainter* pPainter, QRectF qrBgRect, QString strSize, QString strSrc, CDocument* pDocument);
void paintOuterGlow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize);
void paintInnerGlow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize);
void paintDropShadow(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity, int iLightDir, int iDistance, int iSpread, int iSize);
void paintColorOverlay(QPainter* pPainter, QRectF qrRect, QString strColor, RenderMode iRenderMode, double dOpacity);
void paintBackgroundGradient(QPainter *pPainter, QRectF r, CSS::Property* bggrad, CSS::Property* gradtype, CSS::Property* gradspread, CSS::Property* anglegrad, CSS::Property* gradcenter, CSS::Property* gradfocal);


void paintBorder(QPainter* pPainter, CBaseObject* pObj);
void paintBackgroundColor(QPainter* pPainter, CBaseObject* pObj);
void paintBackgroundGradient(QPainter* pPainter, CBaseObject* pObj);
void paintBackgroundImage(QPainter* pPainter, CBaseObject* pObj);
void paintOuterGlow(QPainter* pPainter, CBaseObject* pObj);
void paintInnerGlow(QPainter* pPainter, CBaseObject* pObj);
void paintDropShadow(QPainter* pPainter, CBaseObject* pObj);
void paintColorOverlay(QPainter* pPainter, CBaseObject* pObj);

}

#endif // CSS_PAINTERS_H
