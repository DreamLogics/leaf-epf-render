#ifndef CSS_TRANSITION_H
#define CSS_TRANSITION_H

#include "css_animation.h"
#include <QMap>
#include <QThread>
#include <QStringList>

namespace CSS
{
    class Transitioner
    {
    public:

        struct transition
        {
            Animation* m_pAnimation;
            QString m_sIdentifier;
            int m_iAnimation;
        };

        static Transitioner* get(QThread* th);

        void createTransition(CBaseObject* obj, QString identifier, QList<Property> deltaprops, QStringList transitionable);
        void undoTransition(CBaseObject* obj, QString identifier);

    private:
        QMap<CBaseObject*,transition> m_Transitions;
    };

}
#endif // CSS_TRANSITION_H
