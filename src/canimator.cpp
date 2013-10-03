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
#include "epfevent.h"

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
    QMap<int,registered_animation>::Iterator it;
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
                {
                    if (!regani.m_bFinished)
                    {
                        if (regani.m_bTransition)
                        {
                            //notify transitioner
                        }
                        else
                        {
                            regani.m_bFinished = true;
                            EPFComponent* epfc = dynamic_cast<EPFComponent*>(regani.m_pObject);
                            if (epfc)
                                epfc->sendEvent("animationFinished",regani.m_sAnimation);
                        }
                    }
                    else
                        continue;
                }
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

int CAnimator::registerAnimation(CBaseObject* obj, QString animation, int ms_time, CSS::easing_function ef, int ms_delay, int iterations, CSS::direction dir, int current_anim)
{
    CSS::Stylesheet* css = obj->document()->stylesheet();
    CSS::Animation* ani;

    ani = css->animation(animation);
    if (ani)
        return registerAnimation(obj,ani,ms_time,ef,ms_delay,iterations,dir,false,current_anim);
    return -1;
}

int CAnimator::registerAnimation(CBaseObject *obj, CSS::Animation *animation, int ms_time, CSS::easing_function ef, int ms_delay, int iterations, CSS::direction direction, bool bTransition, int current_anim)
{
    qDebug() << "register animation for object" << obj->id();
    registered_animation regani;
    CSS::Stylesheet* css = obj->document()->stylesheet();
    CSS::Animation* ani;
    CSS::KeyFrame* kf;

    int newid=0;

    if (isRegistered(current_anim,obj))
    {
        qDebug() << "animation already registered";
        if (m_Animations[current_anim].m_sAnimation == animation)
            return current_anim;
        else
        {
            newid = current_anim;
            unregisterAnimation(current_anim,obj);
        }
    }
    else
    {
        for (int i=0;i<=m_Animations.size();i++)
        {
            if (!m_Animations.contains(i))
            {
                newid = i;
                break;
            }
        }
    }

    ani = animation;
    if (ani)
    {

        if (ms_time == 0)
            return -1;

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
        regani.m_bFinished = false;
        regani.m_bTransition = bTransition;
        m_Animations.insert(newid,regani);
        return newid;
    }
    return -1;
}

void CAnimator::unregisterAnimation(int ranim, CBaseObject *obj)
{
    if (m_Animations.contains(ranim))
    {
        if (obj && m_Animations[ranim].m_pObject == obj)
            m_Animations.remove(ranim);
        else if (!obj)
            m_Animations.remove(ranim);
    }
}

bool CAnimator::isRegistered(int ranim, CBaseObject *obj)
{
    if (m_Animations.contains(ranim))
    {
        if (obj && m_Animations[ranim].m_pObject == obj)
            return true;
        else if (!obj)
            return true;
        else
            return false;
    }
    return false;
}

void CAnimator::stylesheetChange(CSS::Stylesheet* css)
{
    QMap<int,registered_animation>::Iterator it;
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
                kf->addProperty(props[i],css->property(regani.m_pObject,props[i]));
            }

            ani->generateFrames(kf);
            delete kf;
            regani.m_pAnim = ani;
        }
        else
            unregisterAnimation(it.key());
    }
}

void CAnimator::setSection(CSection *section)
{
    m_pSection = section;
}

void CAnimator::reverseAnimation(int ranim, CBaseObject *obj)
{
    if (!isRegistered(ranim,obj))
        return;

    registered_animation rani = m_Animations[ranim];
    rani.m_bAlternate = true;
    rani.m_iCurFrame = rani.m_iFrames - rani.m_iCurFrame - 1;
    m_Animations[ranim] = rani;
}
