#ifndef ISTYLEEFFECT_H
#define ISTYLEEFFECT_H

class QPainter;

class IStyleEffect
{
public:
    ~IStyleEffect() {}

    void render(QPainter* p) = 0;
};

#endif // ISTYLEEFFECT_H
