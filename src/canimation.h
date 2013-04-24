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

    virtual void addConnection(QString event, QString target, IEPFEvent* ev);

public slots:

    void playNextFrame();

private:

    QString mixColor(QString color1, QString color2, double pos);
    QString mixInt(QString int1, QString int2, double pos);
    QString mixDouble(QString dbl1, QString dbl2, double pos);

    //void generateFrames(QString layout);
    CAnimFrame* generateFrame(QString layout, int frame);

    CAnimFrame* getFrame(QString layout, int frame);

    void finished();

private:
    //QList<CAnimFrame*> m_Frames;
    QMap<QString,QMap<int,CAnimFrame*> > m_Frames;
    IOEPFDocument* m_pDoc;

    QStringList m_ColorProps;
    QStringList m_IntProps;

    QTimer* m_pTimer;
    int m_iCurFrame;

    int m_iFrameCount;

    bool m_bLoop;

    QMap<QString,QList<QPair<QString,IEPFEvent*> > > m_Connections;

    CAnimFrame* m_pGeneratedFrame;
};

#endif // CANIMATION_H
