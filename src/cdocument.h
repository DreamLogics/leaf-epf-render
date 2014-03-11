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
#include <QMutex>
#include <QIODevice>

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
class CDocument;
class CEPFJS;

class ResourceIO : public QIODevice
{
public:
    ResourceIO(QString resource, CDocument* doc);

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
    virtual qint64 size() const;
    virtual qint64 bytesAvailable() const;
    virtual bool isSequential() const;
    virtual bool seek(qint64 pos);
    //virtual qint64 pos() const;

private:
    CDocument* m_pDoc;
    QString m_sResource;
    //qint64 m_iPos;
};

class CDocument : public QObject, public EPFComponent
{
    Q_OBJECT
public:
    CDocument(QString filename, QStringList platforms, QString language);
    ~CDocument();

    int sectionCount();
    CSection* section(int index);
    CSection* sectionByID(QString);
    CSection* sectionByPosition(int x, int y);
    int indexForSection(CSection*);
    void addSection(CSection* section);

    void registerFont(int id);

    int overlayCount();
    COverlay* overlay(int index);
    COverlay* overlayByID(QString);
    void addOverlay(COverlay* overlay);

    CSS::Stylesheet* stylesheet(bool hold_control=false);
    void releaseStylesheet();

    //void setActiveOverlay(COverlay* overlay);
    //COverlay* activeOverlay();

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
        unsigned char lzmaprop;
        qint32 offset;
        qint32 size;
        qint32 size_compressed;
        qint16 type;
        ResourceIO* device;
        QIODevice* external_device;
    };

    QByteArray resource(QString resource);
    void addResource(QString resource, QString resource_file, QString extra, unsigned char lzmaprop, qint32 offset, qint32 size, qint32 size_compressed, qint16 type);
    int resource(QString resource, char* buffer, int len, int offset=0);
    ResourceIO* resourceIO(QString resource);
    Resource resourceInfo(QString resource);

    //void makeConnection(EPFComponent* src, QString event, EPFComponent* target, QString function);
    virtual void onEPFEvent(EPFEvent *ev);



    bool shouldStopLayout();
    void stopLayout(bool b);

    QString filename() const;

signals:

    void finishedLoading();
    void _updateRenderView();

    void setSection(int index);
    //void _setActiveOverlay(QString id);

public slots:

    void updateRenderView();

    QObjectList sections();
    QObjectList overlays();

    QObject* getSectionByID(QString);

    QObject* getCurrentSection();
    void setCurrentSection(QString section_id);

    void playAnimation(QString animation, bool loop=false);

    void setStylesheetVariable(QString key, QString val);
    QString stylesheetVariable(QString key);

    //void setActiveOverlay(QString overlay_id);
    void close();

private slots:

    void load(int height, int width, int sectionid);
    void layout(int height, int width, int section, bool bCurrentSectionOnly);

    void sectionChange(QString id);

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
    //COverlay* m_pActiveOverlay;
    //QString m_sActiveOverlay;
    //QList<COverlay*> m_OverlayStack;
    CSS::Stylesheet* m_pStylesheet;

    QMap<QString,QIODevice*> m_devices;

    QList<int> m_RegisteredFonts;

    bool m_bShouldStopLayout;
    QMutex m_mShouldStopLayoutMutex;

    QMutex m_mCSSMutex;

    int m_iLayoutHeight;
    int m_iLayoutWidth;

    CEPFJS* m_pJS;

    QString m_sFileName;

    friend class CEPFView;
};

#endif // CDOCUMENT_H
