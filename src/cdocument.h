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

#ifndef CDOCUMENT_H
#define CDOCUMENT_H

#include <QRect>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QObjectList>
#include "epfevent.h"

//class COEPFRender;

namespace CSS
{
    class Stylesheet;
}

class CSection;
class COverlay;
class CLayout;
class CAnimation;
class CEPFView;
class EPFEvent;


class CDocument : public QObject, public EPFComponent
{
    Q_OBJECT
public:
    CDocument(QStringList platforms, QString language);
    ~CDocument();

    int sectionCount();
    CSection* section(int index);
    CSection* sectionByID(QString);
    CSection* sectionByPosition(int x, int y);
    int indexForSection(CSection*);
    void addSection(CSection* section);

    int overlayCount();
    COverlay* overlay(int index);
    COverlay* overlayByID(QString);
    void addOverlay(COverlay* overlay);

    CSS::Stylesheet* stylesheet();

    void setActiveOverlay(COverlay* overlay);
    COverlay* activeOverlay();

    QString property(QString key);
    void addProperty(QString key, QString value);

    void addLayout(CLayout* layout);

    CLayout* layoutByID(QString id, bool bMakeCurrent = false);
    CLayout* currentLayout();

    CAnimation* animation(QString id);
    void addAnimation(QString id, int frames, int fps, QString src);

    CEPFView* renderview();
    void setRenderview(CEPFView*);

    struct Resource
    {
        QString container;
        QString extra;
        //qint32 checksum;
        qint32 offset;
        qint32 size;
        qint32 size_compressed;
        qint16 type;
    };

    QByteArray resource(QString resource);
    void addResource(QString resource, QString resource_file, QString extra, /*qint32 checksum, */qint32 offset, qint32 size, qint32 size_compressed, qint16 type);

    //void makeConnection(EPFComponent* src, QString event, EPFComponent* target, QString function);
    virtual void onEPFEvent(EPFEvent *ev);

    void updateRenderView();

signals:

    void finishedLoading();
    void _updateRenderView();

    void setSection(int index);

public slots:

    QObjectList sections();
    QObjectList overlays();

    QObject* getSectionByID(QString);

    QObject* getCurrentSection();
    void setCurrentSection(QString section_id);

    void playAnimation(QString animation, bool loop);

    void setActiveOverlay(QString overlay_id);

    void load(int height, int width);
    void layout(int height, int width);

    void saveBuffers();

private:
    QList<CSection*> m_Sections;
    QList<COverlay*> m_Overlays;
    QMap<QString,QString> m_Props;
    QMap<QString,struct Resource> m_Resources;
    QList<CLayout*> m_Layouts;
    CLayout* m_pCurrentLayout;
    QStringList m_Platforms;
    QString m_sLanguage;
    QMap<QString,CAnimation*> m_Animations;
    CEPFView* m_pRenderView;
    COverlay* m_pActiveOverlay;
    CSS::Stylesheet* m_pStylesheet;
};

#endif // CDOCUMENT_H
