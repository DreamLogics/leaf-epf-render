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

#ifndef CANIMATION_H
#define CANIMATION_H

#include <QObject>

class CDocument;
class CAnimFrame;

class CAnimation : public QObject
{
    Q_OBJECT
public:
    CAnimation(int frames, int fps, QString src, CDocument* doc);
    virtual ~CAnimation();

    virtual void play(bool loop);
    virtual void stop();
    virtual void reset();

public slots:

    void playNextFrame();

signals:

    void finished();

private:

    QString mixColor(QString color1, QString color2, double pos);
    QString mixInt(QString int1, QString int2, double pos);
    QString mixDouble(QString dbl1, QString dbl2, double pos);

    //void generateFrames(QString layout);
    CAnimFrame* generateFrame(QString layout, int frame);

    CAnimFrame* getFrame(QString layout, int frame);

private:
    //QList<CAnimFrame*> m_Frames;
    QMap<QString,QMap<int,CAnimFrame*> > m_Frames;
    CDocument* m_pDoc;

    QStringList m_ColorProps;
    QStringList m_IntProps;

    QTimer* m_pTimer;
    int m_iCurFrame;

    int m_iFrameCount;

    bool m_bLoop;

    CAnimFrame* m_pGeneratedFrame;
};

#endif // CANIMATION_H
