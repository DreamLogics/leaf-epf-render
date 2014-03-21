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

#ifndef CMATHMLOBJECT_H
#define CMATHMLOBJECT_H

#include <cbaseobject.h>
#include <iepfobjectfactory.h>

class CMathMLObjectFactory : public IEPFObjectFactory
{
public:
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class QwtMmlDocument;
class CMathMLObject;

class JSMathMLObjectProxy : public JSBaseObjectProxy
{
    Q_OBJECT
public:
    JSMathMLObjectProxy(CMathMLObject* obj);


public slots:


signals:



private:
    CMathMLObject* m_pObject;
};

class CMathMLObject : public CBaseObject
{
    Q_OBJECT
public:
    CMathMLObject(QString id, CLayer* layer);
    virtual ~CMathMLObject();

    virtual void preload();

    virtual void layout(QRectF relativeTo,QList<CBaseObject*> updatelist = QList<CBaseObject*>());

    virtual void paint(QPainter *painter);
    virtual void paintBuffered(QPainter *p);

    virtual QObject* jsProxy() const;
/*
    virtual void mouseDoubleClickEvent ( QPoint pos );
    virtual void mousePressEvent( QPoint pos );
    virtual void mouseReleaseEvent( QPoint pos );
    virtual void mouseMoveEvent( QPoint pos );

    virtual void keyPressEvent(int key, QString val);
    virtual void keyReleaseEvent(int key, QString val);*/


private:

    JSMathMLObjectProxy* m_pJSProxy;
    QwtMmlDocument* m_pMathMLDoc;
};

#endif // CMATHMLOBJECT_H
