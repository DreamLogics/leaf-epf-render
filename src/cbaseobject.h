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

#ifndef CBASEOBJECT_H
#define CBASEOBJECT_H

#include "leaf-epf-render_global.h"
#include <QGraphicsObject>
#include "css/css_style.h"

class QMouseEvent;

class LEAFEPFRENDERSHARED_EXPORT CBaseObject : public QGraphicsObject
{
    Q_OBJECT
public:
    CBaseObject(QString id, CLayer* layer);
    virtual ~CBaseObject();

    virtual void preload();

    //virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QString id();
    virtual const char* objectType() const;

    virtual CLayer* layer();

    virtual void setBoundingRect(const QRectF&);
    virtual QRectF boundingRect() const;

    virtual void layout(CBaseObject* relative);
    virtual CBaseObject* getObjectAnchor();
    virtual QString getObjectAnchorID();

    virtual QString property(QString key);
    virtual void setProperty(QString key, QString value);

    virtual void mouseDoubleClickEvent ( QMouseEvent * e, QPoint contentpos );
    virtual void mousePressEvent(QMouseEvent *, QPoint contentpos);
    virtual void mouseReleaseEvent(QMouseEvent *, QPoint contentpos);
    virtual void mouseMoveEvent(QMouseEvent *, QPoint contentpos);

    virtual CSS::Stylesheet* style();

    virtual void setCSSOverride(QString css);
    virtual QString cssOverrides();

    virtual bool visible();
    virtual void setVisibility(bool);

    virtual QString css();

    virtual int marginTop() const;
    virtual int marginBottom() const;
    virtual int marginLeft() const;
    virtual int marginRight() const;

    void setMargin(int top, int left, int bottom, int right);
    void setMarginTop(int top);
    void setMarginLeft(int left);
    void setMarginBottom(int bottom);
    void setMarginRight(int right);

    virtual int outerHeight() const;
    virtual int outerWidth() const;
/*
    virtual int paddingTop() const;
    virtual int paddingBottom() const;
    virtual int paddingLeft() const;
    virtual int paddingRight() const;

    void setPadding(int top, int left, int bottom, int right);
    void setPaddingTop(int top);
    void setPaddingLeft(int left);
    void setPaddingBottom(int bottom);
    void setPaddingRight(int right);

    virtual int innerHeight() const;
    virtual int innerWidth() const;*/

public slots:

    virtual QStringList styleClasses() const;
    virtual void addStyleClass(QString classname);
    virtual void removeStyleClass(QString classname);

    virtual QObject* getStyle();

signals:

    void clicked();
    void onMouseDown(int posx, int posy);
    void onMouseMove(int posx, int posy);
    void onMouseUp(int posx, int posy);
    void onMouseEnter();
    void onMouseLeave();
    void onSwipe(int direction);
    void onFocus();
    void onLoseFocus();

private:

    QRectF m_rRect;
    QString m_sID;
    QMap<QString,QString> m_Props;
    //CStyleParser* m_pStyle;
    int m_iVisible;
    QString m_sCSS;
    QString m_sCSSOverrides;

    QImage m_qiRenderBuffer;
    bool m_bNeedsRedraw;

    bool m_bDisabled;



    int m_iMarginTop;
    int m_iMarginLeft;
    int m_iMarginBottom;
    int m_iMarginRight;
/*
    int m_iPaddingTop;
    int m_iPaddingLeft;
    int m_iPaddingBottom;
    int m_iPaddingRight;*/


    QStringList m_StyleClasses;
    CLayer* m_pLayer;
    
};

#endif // CBASEOBJECT_H
