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

#ifndef CSS_PAINTERS_H
#define CSS_PAINTERS_H

#include "css_style.h"
#include <QRectF>
class QPainter;
//class CDocument;
class CBaseObject;

namespace CSS
{

void paintBorder(QPainter* pPainter, QRectF qrBorderRect, CSS::Property bordertop, CSS::Property borderbottom, CSS::Property borderleft, CSS::Property borderright);
void paintBackgroundColor(QPainter* pPainter, QRectF qrBgRect, QColor color);
void paintBackgroundImage(QPainter* pPainter, QRectF qrBgRect, QString strSize, QString strSrc, CDocument* pDocument);
void paintOuterGlow(QPainter* pPainter, QImage &mask, QImage &src, QColor cColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize);
void paintInnerGlow(QPainter* pPainter, QRectF qrRect, QColor cColor, RenderMode iRenderMode, double dOpacity, int iSpread, int iSize);
void paintDropShadow(QPainter* pPainter, QRectF qrRect, QColor cColor, RenderMode iRenderMode, double dOpacity, int iLightDir, int iDistance, int iSpread, int iSize);
void paintColorOverlay(QPainter* pPainter, QRectF qrRect, QColor cColor, RenderMode iRenderMode);
void paintBackgroundGradient(QPainter *pPainter, QRectF r, CSS::Property bggrad, CSS::Property gradtype, CSS::Property gradspread, CSS::Property anglegrad, CSS::Property gradcenter, CSS::Property gradfocal);


void paintBorder(QPainter* pPainter, CBaseObject* pObj);
void paintBackgroundColor(QPainter* pPainter, CBaseObject* pObj);
void paintBackgroundGradient(QPainter* pPainter, CBaseObject* pObj);
void paintBackgroundImage(QPainter* pPainter, CBaseObject* pObj);
void paintOuterGlow(QPainter* pPainter, CBaseObject* pObj, QImage &mask, QImage &src);
void paintInnerGlow(QPainter* pPainter, CBaseObject* pObj);
void paintDropShadow(QPainter* pPainter, CBaseObject* pObj);
void paintColorOverlay(QPainter* pPainter, CBaseObject* pObj);

}

#endif // CSS_PAINTERS_H
