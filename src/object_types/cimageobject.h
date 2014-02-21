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

#ifndef CIMAGEOBJECT_H
#define CIMAGEOBJECT_H

#include "../cbaseobject.h"
#include "../iepfobjectfactory.h"

class CImageObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class CImageObject : public CBaseObject
{
    Q_OBJECT
public:
    CImageObject(QString id, CLayer* layer);

    virtual void preload();
    virtual void layout(QRectF relativeTo, QList<CBaseObject *> updatelist);
    virtual void paint(QPainter *painter);

    void setImage(QString src);

protected:

    virtual QObject* makeJsProxy();

private:
    QImage m_Image;
    QString m_sImageSrc;
};

class JSImageObjectProxy : public JSBaseObjectProxy
{
    Q_OBJECT
public:
    JSImageObjectProxy(CImageObject* obj);

public slots:

    void setImage(QString src);

private:
    CImageObject* m_pImageObject;
};

#endif // CIMAGEOBJECT_H
