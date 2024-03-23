#include "pum.hpp"

CPum::CPum() : CPixelApp("Pikku-ukot mesoo", 320, 200)
{
    m_window.setVerticalSyncEnabled(true);
}

CPum::~CPum()
{
}

bool CPum::onInit()
{
    return true;
}

bool CPum::onEvent(const sf::Event& event)
{
    // return true to prevent default handling
    return false;
}

void CPum::onUpdate(const sf::Time& frameTime)
{
}

void CPum::onDraw()
{
    for (int i = 0; i < m_pixels.size(); i += 4)
    {
        m_pixels[i + 0] = std::rand() & 0xff;
        m_pixels[i + 1] = std::rand() & 0xff;
        m_pixels[i + 2] = std::rand() & 0xff;
        m_pixels[i + 3] = 255;
    }
}
