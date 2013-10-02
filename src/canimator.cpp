#include "canimator.h"
#include "cbaseobject.h"
#include "css/css_style.h"
#include "css/css_animation.h"
#include <QTimer>
#include <QEasingCurve>
#include <QDebug>
#include <QMap>
#include <QThread>
#include <QMutex>
#include <QList>

#define FRAMERATE 24

QMutex g_gettermutex;

CAnimator::CAnimator(QObject *parent) :
    QObject(parent)
{
    m_pSection = 0;
    m_iTime = 0;
    m_pTimer = new QTimer();
    m_pTimer->setInterval(FRAMERATE);
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(update()));
    m_pTimer->start();
}

CAnimator* CAnimator::get(QThread *th)
{
    //static CAnimator* p=0;
    g_gettermutex.lock();
    static QMap<QThread*,CAnimator*> plist;
    CAnimator* p;
    if (!plist.contains(th))
    {
        p = new CAnimator();
        plist.insert(th,p);
        g_gettermutex.unlock();
        return p;
    }
    p = plist[th];
    g_gettermutex.unlock();
    return p;
}

void CAnimator::update()
{
    QMap<CBaseObject*,registered_animation>::Iterator it;
    //QMap<CBaseObject*,registered_animation> nmap;
    double pos;
    bool bShouldLayout = false;


    if (!m_pSection)
        return;

    for (it=m_Animations.begin();it != m_Animations.end();it++)
    {
        registered_animation regani = it.value();
        //qDebug() << "CAnimator::update()" << m_iTime << regani.m_iStartTime;
        if (regani.m_pObject->section() != m_pSection)
            continue;

        bShouldLayout = true;

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

            pos = (double)regani.m_iCurFrame / (double)regani.m_iFrames;

            //qDebug() << "curframes" << regani.m_iCurFrame << "frames" << regani.m_iFrames;
            //qDebug() << "pos pre-ease" << pos;

            if (regani.m_efEasing == CSS::efEase)
            {
                QEasingCurve ec(QEasingCurve::InOutQuad);
                pos = ec.valueForProgress(pos);
            }
            else if (regani.m_efEasing == CSS::efEaseIn)
            {
                QEasingCurve ec(QEasingCurve::InQuad);
                pos = ec.valueForProgress(pos);
            }
            else if (regani.m_efEasing == CSS::efEaseOut)
            {
                QEasingCurve ec(QEasingCurve::OutQuad);
                pos = ec.valueForProgress(pos);
            }

            //qDebug() << "pos post-ease" << pos;

            if (regani.m_dirDirection == CSS::dirReverse || (regani.m_dirDirection == CSS::dirAlternate && regani.m_bAlternate)
                    || (regani.m_dirDirection == CSS::dirAlternateReverse && !regani.m_bAlternate) )
                pos = 1 - pos;

            QStringList props = regani.m_pAnim->properties();
            for (int i=0;i<props.size();i++)
            {
                regani.m_pObject->setCSSOverrideProp(props[i],regani.m_pAnim->keyedProperty(props[i],pos));
            }
            regani.m_iCurFrame++;
            m_Animations[it.key()] = regani;
            //m_Animations.insert(it,it.key(),regani);
            //nmap.insert(it.key(),regani);
        }
    }

    //m_Animations = nmap;

    if (bShouldLayout)
        m_pSection->layout();

    m_iTime+=FRAMERATE;
}

void CAnimator::registerAnimation(CBaseObject* obj, QString animation, int ms_time, CSS::easing_function ef, int ms_delay, int iterations, CSS::direction dir)
{
    qDebug() << "register animation for object" << obj->id();
    registered_animation regani;
    CSS::Stylesheet* css = obj->document()->stylesheet();
    CSS::Animation* ani;
    CSS::KeyFrame* kf;

    if (isRegistered(obj))
        return;
        //unregisterAnimation(obj);

    ani = css->animation(animation);
    if (ani)
    {

        if (ms_time == 0)
            return;

        kf = new CSS::KeyFrame();
        QStringList props = ani->properties();

        for (int i=0;i<props.size();i++)
        {
            kf->addProperty(props[i],css->property(obj,props[i]).clone());
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
    //qDebug() << "unregister animation for object" << obj->id();
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
                kf->addProperty(props[i],css->property(it.key(),props[i]));
            }

            ani->generateFrames(kf);
            delete kf;
            regani.m_pAnim = ani;
        }
        else
            unregisterAnimation(regani.m_pObject);
    }
}

void CAnimator::setSection(CSection *section)
{
    m_pSection = section;
}
