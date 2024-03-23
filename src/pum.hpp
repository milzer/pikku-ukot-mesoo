#pragma once

#include "pixelapp.hpp"

class CPum : public CPixelApp
{
public:
    CPum();
    virtual ~CPum();

protected:
    bool onInit();
    bool onEvent(const sf::Event& event);
    void onUpdate(const sf::Time& frameTime);
    void onDraw();
};
