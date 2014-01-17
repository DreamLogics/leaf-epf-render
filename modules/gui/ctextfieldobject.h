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

#ifndef CTEXTFIELDOBJECT_H
#define CTEXTFIELDOBJECT_H

#include "../../src/cbaseobject.h"
#include "../../src/iepfobjectfactory.h"
#include <QElapsedTimer>
#include <QMutex>

class CTextFieldObjectFactory : public IEPFObjectFactory
{
    virtual CBaseObject* create(QString id, CLayer *layer);
};

class QLineEdit;

class CTextFieldObject;

class JSTextFieldObjectProxy : public JSBaseObjectProxy
{
    Q_OBJECT
public:
    JSTextFieldObjectProxy(CTextFieldObject* obj);

    void _textChanged(QString str);

public slots:
    void setText(QString str);

signals:

    void textChanged(QString str);

private:
    CTextFieldObject* m_pObject;
};

class CTextFieldObject : public CBaseObject
{
    Q_OBJECT
public:
    CTextFieldObject(QString id, CLayer* layer);
    virtual ~CTextFieldObject();

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

    virtual bool eventFilter(QObject *p, QEvent *ev);

/*public slots:
    void setTextLine(QString str);*/

    void setText(QString str);

public slots:

    void setValue(QString s);

signals:

    void createTextField();
    void updateWidgetGeometry(QRect r);
    void showWidget();
    void hideWidget();
    void destroyWidget();
    void textChanged(QString str);

private:
    bool m_bHasFocus;
    QString m_sValue;
    QElapsedTimer m_HoldTimer;
    QMutex m_mValueMutex;
    JSTextFieldObjectProxy* m_pJSProxy;
};

#endif // CTextFieldOBJECT_H
