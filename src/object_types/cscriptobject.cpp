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

#include "cscriptobject.h"
#include <QPainter>
#include "../cdocument.h"

CBaseObject* CScriptObjectFactory::create(QString id, CLayer *layer)
{
    return new CScriptObject(id,layer);
}

CScriptObject::CScriptObject(QString id, CLayer *layer) : CBaseObject(id,layer)
{
}

void CScriptObject::preload()
{
    CBaseObject::preload();
    m_pScript = new QScriptEngine();
    QByteArray scriptfile = document()->resource(property("src"));
    if (scriptfile.size() > 0)
    {
        QString scr = QString::fromUtf8(scriptfile.constData(),scriptfile.size());
        m_pScript->evaluate(scr);
    }
}

void CScriptObject::paint(QPainter *painter)
{
    QPen pen;
    JSPen jspen(&pen);
    JSPainter jp(painter,&pen);
    QScriptValue jpsv = m_pScript->newQObject(&jp);
    QScriptValue jspensv = m_pScript->newQObject(&jspen);
    QScriptValue jsthis = m_pScript->newQObject(this);

    m_pScript->globalObject().setProperty("painter",jpsv);
    m_pScript->globalObject().setProperty("pen",jspensv);
    m_pScript->globalObject().setProperty("this",jsthis);

    m_pScript->evaluate("render();");
}

JSPainter::JSPainter(QPainter *p, QPen *pen) : m_pPainter(p), m_pPen(pen)
{

}

qreal JSPainter::opacity() const
{
    return m_pPainter->opacity();
}

void JSPainter::setOpacity(qreal opacity)
{
    m_pPainter->setOpacity(opacity);
}

void JSPainter::setClipRect(int x, int y, int w, int h/*, Qt::ClipOperation op = Qt::ReplaceClip*/)
{
    m_pPainter->setClipRect(x,y,w,h);
}


void JSPainter::setClipping(bool enable)
{
    m_pPainter->setClipping(enable);
}

bool JSPainter::hasClipping() const
{
    return m_pPainter->hasClipping();
}


QRectF JSPainter::clipBoundingRect() const
{
    return m_pPainter->clipBoundingRect();
}


void JSPainter::save()
{
    m_pPainter->save();
}

void JSPainter::restore()
{
    m_pPainter->restore();
}


void JSPainter::resetTransform()
{
    m_pPainter->resetTransform();
}


void JSPainter::setMatrixEnabled(bool enabled)
{
    m_pPainter->setMatrixEnabled(enabled);
}

bool JSPainter::matrixEnabled() const
{
    return m_pPainter->matrixEnabled();
}


void JSPainter::setWorldMatrixEnabled(bool enabled)
{
    m_pPainter->setWorldMatrixEnabled(enabled);
}

bool JSPainter::worldMatrixEnabled() const
{
    return m_pPainter->worldMatrixEnabled();
}


void JSPainter::scale(qreal sx, qreal sy)
{
    m_pPainter->scale(sx,sy);
}

void JSPainter::shear(qreal sh, qreal sv)
{
    m_pPainter->shear(sh,sv);
}

void JSPainter::rotate(qreal a)
{
    m_pPainter->rotate(a);
}


void JSPainter::translate(qreal dx, qreal dy)
{
    m_pPainter->translate(dx,dy);
}



void JSPainter::drawPoint(int x, int y)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawPoint(x,y);
}


void JSPainter::drawLine(int x1, int y1, int x2, int y2)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawLine(x1,y1,x2,y2);
}


void JSPainter::drawRect(int x1, int y1, int w, int h)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawRect(x1,y1,w,h);
}


void JSPainter::drawEllipse(int x, int y, int w, int h)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawEllipse(x,y,w,h);
}


void JSPainter::drawArc(int x, int y, int w, int h, int a, int alen)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawArc(x,y,w,h,a,alen);
}


void JSPainter::drawPie(int x, int y, int w, int h, int a, int alen)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawPie(x,y,w,h,a,alen);
}


void JSPainter::drawChord(int x, int y, int w, int h, int a, int alen)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawChord(x,y,w,h,a,alen);
}


void JSPainter::drawRoundedRect(int x, int y, int w, int h, qreal xRadius, qreal yRadius/*,
                            Qt::SizeMode mode = Qt::AbsoluteSize*/)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawRoundedRect(x,y,w,h,xRadius,yRadius);
}


void JSPainter::drawRoundRect(int x, int y, int w, int h, int a, int b)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawRoundRect(x,y,w,h,a,b);
}


void JSPainter::drawText(int x, int y, const QString &s)
{
    const QPen pen = *m_pPen;
    m_pPainter->setPen(pen);
    m_pPainter->drawText(x,y,s);
}


void JSPainter::fillRect(int x, int y, int w, int h, const QString &color)
{
    m_pPainter->fillRect(x,y,w,h,QColor(color));
}


void JSPainter::eraseRect(int x, int y, int w, int h)
{
    m_pPainter->eraseRect(x,y,w,h);
}

JSPen::JSPen(QPen *pen) : m_pPen(pen)
{

}

/*int JSPen::style() const
{
    return m_pPen->style();
}*/
void JSPen::setStyle(int s)
{
    Qt::PenStyle style = static_cast<Qt::PenStyle>(s);
    m_pPen->setStyle(style);
}

qreal JSPen::dashOffset() const
{
    return m_pPen->dashOffset();
}
void JSPen::setDashOffset(qreal doffset)
{
    m_pPen->setDashOffset(doffset);
}


qreal JSPen::miterLimit() const
{
    return m_pPen->miterLimit();
}

void JSPen::setMiterLimit(qreal limit)
{
    m_pPen->setMiterLimit(limit);
}


qreal JSPen::widthF() const
{
    return m_pPen->widthF();
}

void JSPen::setWidthF(qreal width)
{
    m_pPen->setWidthF(width);
}


int JSPen::width() const
{
    return m_pPen->width();
}

void JSPen::setWidth(int width)
{
    m_pPen->setWidth(width);
}


QString JSPen::color() const
{
    return m_pPen->color().name();
}

void JSPen::setColor(const QString &color)
{
    m_pPen->setColor(QColor(color));
}


bool JSPen::isSolid() const
{
    return m_pPen->isSolid();
}


//Qt::PenCapStyle capStyle() const


void JSPen::setCapStyle(int pcs)
{
    Qt::PenCapStyle style = static_cast<Qt::PenCapStyle>(pcs);
    m_pPen->setCapStyle(style);
}


//Qt::PenJoinStyle joinStyle() const


void JSPen::setJoinStyle(int pcs)
{
    Qt::PenJoinStyle style = static_cast<Qt::PenJoinStyle>(pcs);
    m_pPen->setJoinStyle(style);
}


bool JSPen::isCosmetic() const
{
    return m_pPen->isCosmetic();
}

void JSPen::setCosmetic(bool cosmetic)
{
    m_pPen->setCosmetic(cosmetic);
}
