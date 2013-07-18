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

#ifndef CSCRIPTOBJECT_H
#define CSCRIPTOBJECT_H

#include "cbaseobject.h"
#include "iepfobjectfactory.h"
#include <QScriptEngine>

class CScriptObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class JSPen : public QObject
{
    Q_OBJECT
public:
    JSPen(QPen*);

public slots:

    //int style() const;
    void setStyle(int s);

    qreal dashOffset() const;
    void setDashOffset(qreal doffset);

    qreal miterLimit() const;
    void setMiterLimit(qreal limit);

    qreal widthF() const;
    void setWidthF(qreal width);

    int width() const;
    void setWidth(int width);

    QString color() const;
    void setColor(const QString &color);

    bool isSolid() const;

    //Qt::PenCapStyle capStyle() const;
    void setCapStyle(int pcs);

    //Qt::PenJoinStyle joinStyle() const;
    void setJoinStyle(int pcs);

    bool isCosmetic() const;
    void setCosmetic(bool cosmetic);

private:
    QPen* m_pPen;
};

class JSPainter : public QObject
{
    Q_OBJECT
public:
    JSPainter(QPainter* p, QPen* pen);

public slots:

    qreal opacity() const;
    void setOpacity(qreal opacity);

    void setClipRect(int x, int y, int w, int h/*, Qt::ClipOperation op = Qt::ReplaceClip*/);

    void setClipping(bool enable);
    bool hasClipping() const;

    QRectF clipBoundingRect() const;

    void save();
    void restore();

    void resetTransform();

    void setMatrixEnabled(bool enabled);
    bool matrixEnabled() const;

    void setWorldMatrixEnabled(bool enabled);
    bool worldMatrixEnabled() const;

    void scale(qreal sx, qreal sy);
    void shear(qreal sh, qreal sv);
    void rotate(qreal a);

    void translate(qreal dx, qreal dy);


    void drawPoint(int x, int y);

    void drawLine(int x1, int y1, int x2, int y2);

    void drawRect(int x1, int y1, int w, int h);

    void drawEllipse(int x, int y, int w, int h);

    void drawArc(int x, int y, int w, int h, int a, int alen);

    void drawPie(int x, int y, int w, int h, int a, int alen);

    void drawChord(int x, int y, int w, int h, int a, int alen);

    void drawRoundedRect(int x, int y, int w, int h, qreal xRadius, qreal yRadius/*,
                                Qt::SizeMode mode = Qt::AbsoluteSize*/);

    void drawRoundRect(int x, int y, int w, int h, int = 25, int = 25);

    void drawText(int x, int y, const QString &s);

    void fillRect(int x, int y, int w, int h, const QString &color);

    void eraseRect(int x, int y, int w, int h);


private:
    QPainter* m_pPainter;
    QPen* m_pPen;
};

class CScriptObject : public CBaseObject
{
    Q_OBJECT
public:
    CScriptObject(QString id, CLayer* layer);

    virtual void preload();
    virtual void paint(QPainter *painter);

private:
    QScriptEngine* m_pScript;
};

#endif // CSCRIPTOBJECT_H
