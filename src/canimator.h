#ifndef CANIMATOR_H
#define CANIMATOR_H

#include <QObject>
#include <QMutex>
#include <QMap>

namespace CSS {
    class Stylesheet;
    class Animation;
}

class CBaseObject;
class QTimer;

class CAnimator : public QObject
{
    Q_OBJECT
public:
    explicit CAnimator(QObject *parent = 0);

    static CAnimator* get();

    enum easing_function
    {
        efNone=0,
        efEase,
        efEaseIn,
        efEaseOut
    };

    enum direction
    {
        dirNormal=0,
        dirReverse,
        dirAlternate,
        dirAlternateReverse

    };

    struct registered_animation
    {
        CBaseObject* m_pObject;
        QString m_sAnimation;
        int m_iTime;
        int m_iDelay;
        int m_iIterations;
        easing_function m_efEasing;
        direction m_dirDirection;

        CSS::Animation* m_pAnim;

        int m_iStartTime;
        int m_iFrames;
        int m_iCurFrame;
        bool m_bAlternate;
    };

    void registerAnimation(CBaseObject* obj, QString animation, int ms_time, easing_function ef, int ms_delay, int iterations, direction direction);
    void unregisterAnimation(CBaseObject* obj);
    bool isRegistered(CBaseObject* obj);

    void stylesheetChange(CSS::Stylesheet* css);
    
signals:
    
public slots:

    void update();
    
private:

    QMap<CBaseObject*,registered_animation> m_Animations;
    QTimer* m_pTimer;
    unsigned int m_iTime;

};

#endif // CANIMATOR_H
