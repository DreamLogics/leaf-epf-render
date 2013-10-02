#ifndef CANIMATOR_H
#define CANIMATOR_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include "css/css_animation.h"

namespace CSS {
    class Stylesheet;
}

class CBaseObject;
class QTimer;
class CSection;
class QThread;

class CAnimator : public QObject
{
    Q_OBJECT
public:
    explicit CAnimator(QObject *parent = 0);

    static CAnimator* get(QThread*);

    struct registered_animation
    {
        CBaseObject* m_pObject;
        QString m_sAnimation;
        int m_iTime;
        int m_iDelay;
        int m_iIterations;
        CSS::easing_function m_efEasing;
        CSS::direction m_dirDirection;

        CSS::Animation* m_pAnim;

        int m_iStartTime;
        int m_iFrames;
        int m_iCurFrame;
        bool m_bAlternate;
    };

    void registerAnimation(CBaseObject* obj, QString animation, int ms_time, CSS::easing_function ef, int ms_delay, int iterations, CSS::direction direction);
    void unregisterAnimation(CBaseObject* obj);
    bool isRegistered(CBaseObject* obj);

    void stylesheetChange(CSS::Stylesheet* css);

    void setSection(CSection* section);
    
signals:
    
public slots:

    void update();
    
private:

    QMap<CBaseObject*,registered_animation> m_Animations;
    QTimer* m_pTimer;
    unsigned int m_iTime;
    CSection* m_pSection;
};

#endif // CANIMATOR_H
