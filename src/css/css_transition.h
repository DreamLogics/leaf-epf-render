#ifndef CSS_TRANSITION_H
#define CSS_TRANSITION_H

#include "css_animation.h"

namespace CSS
{
    class Transitioner
    {
    public:
        void createTransition(QList<Property> props, easing_function easing, direction dir, CBaseObject* obj);
    };

}
#endif // CSS_TRANSITION_H
