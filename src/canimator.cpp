#include "canimator.h"
#include "cbaseobject.h"
#include "css/css_style.h"
#include "css/css_animation.h"
#include <QTimer>
#include <QEasingCurve>

#define FRAMERATE 30

CAnimator::CAnimator(QObject *parent) :
    QObject(parent)
{
    m_iTime = 0;
    m_pTimer = new QTimer();
    m_pTimer->setInterval(FRAMERATE);
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(update()));
    m_pTimer->start();
}

static CAnimator* CAnimator::get()
{
    static CAnimator* p=0;
    if (!p)
        p = new CAnimator();
    return p;
}

void CAnimator::update()
{
    QMap<CBaseObject*,registered_animation>::Iterator it;
    double pos;
    CSection* s;

    for (it=m_Animations.begin();it != m_Animations.end();it++)
    {
        registered_animation regani = it.value();
        if (it == m_Animations.begin())
            s = regani.m_pObject->section();
        if (m_iTime >= regani.m_iStartTime)
        {
            if (regani.m_iCurFrame > regani.m_iFrames)
            {
                if (regani.m_iIterations > 0)
                {
                    regani.m_iCurFrame = 1;
                    regani.m_iIterations--;
                    regani.m_bAlternate = !regani.m_bAlternate;
                }
                else if (regani.m_iIterations == -1)
                {
                    regani.m_iCurFrame = 1;
                    regani.m_bAlternate = !regani.m_bAlternate;
                }
                else
                    continue;
            }

            pos = regani.m_iCurFrame / regani.m_iFrames;

            if (regani.m_efEasing == efEase)
            {
                QEasingCurve ec(QEasingCurve::InOutQuad);
                pos = ec.valueForProgress(pos);
            }
            else if (regani.m_efEasing == efEaseIn)
            {
                QEasingCurve ec(QEasingCurve::InQuad);
                pos = ec.valueForProgress(pos);
            }
            else if (regani.m_efEasing == efEaseOut)
            {
                QEasingCurve ec(QEasingCurve::OutQuad);
                pos = ec.valueForProgress(pos);
            }

            if (regani.m_dirDirection == dirReverse || (regani.m_dirDirection == dirAlternate && regani.m_bAlternate)
                    || (regani.m_dirDirection == dirAlternateReverse && !regani.m_bAlternate) )
                pos = 1 - pos;

            QStringList props = regani.m_pAnim->properties();
            for (int i=0;i<props.size();i++)
            {
                regani.m_pObject->setCSSOverrideProp(props[i],regani.m_pAnim->keyedProperty(props[i],pos));
            }

            regani.m_iCurFrame++;
            m_Animations[it.key()] = regani;
        }
    }

    s->layout();

    m_iTime+=FRAMERATE;
}

void CAnimator::registerAnimation(CBaseObject* obj, QString animation, int ms_time, easing_function ef, int ms_delay, int iterations, direction dir)
{
    registered_animation regani;
    CSS::Stylesheet* css = obj->document()->stylesheet();
    CSS::Animation* ani;
    CSS::KeyFrame* kf;

    if (isRegistered(obj))
        unregisterAnimation(obj);

    ani = css->animation(animation);
    if (ani)
    {
        kf = new CSS::KeyFrame();
        QStringList props = ani->properties();

        for (int i=0;i<props.size();i++)
        {
            kf->addProperty(props[i],css->property(obj,props[i]).toString(false));
        }

        ani->generateFrames(kf);
        delete kf;

        regani.m_dirDirection = dir;
        regani.m_efEasing = ef;
        regani.m_iDelay = ms_delay;
        regani.m_iIterations = iterations;
        regani.m_iStartTime = m_iTime + ms_delay;
        regani.m_iTime = ms_time;
        regani.m_pAnim = ani;
        regani.m_pObject = obj;
        regani.m_sAnimation = animation;
        regani.m_iFrames = ms_time/FRAMERATE;
        regani.m_iCurFrame = 1;
        regani.m_bAlternate = false;
        m_Animations.insert(obj,regani);
    }
}

void CAnimator::unregisterAnimation(CBaseObject *obj)
{
    m_Animations.remove(obj);
}

bool CAnimator::isRegistered(CBaseObject *obj)
{
    return m_Animations.contains(obj);
}

void CAnimator::stylesheetChange(CSS::Stylesheet* css)
{
    QMap<CBaseObject*,registered_animation>::Iterator it;
    CSS::Animation* ani;
    CSS::KeyFrame* kf;


    for (it=m_Animations.begin();it != m_Animations.end();it++)
    {
        registered_animation regani = it.value();
        ani = css->animation(regani.m_sAnimation);
        if (ani)
        {
            kf = new CSS::KeyFrame();
            QStringList props = ani->properties();

            for (int i=0;i<props.size();i++)
            {
                kf->addProperty(props[i],css->property(obj,props[i]).toString(false));
            }

            ani->generateFrames(kf);
            delete kf;
            regani.m_pAnim = ani;
        }
        else
            unregisterAnimation(regani.m_pObject);
    }
}
