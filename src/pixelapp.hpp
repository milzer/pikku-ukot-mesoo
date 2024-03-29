#pragma once

#include <SFML/Graphics.hpp>

class CPixelApp
{
protected:
    std::string m_title;
    sf::Vector2u m_pixelSize;
    std::vector<sf::Uint8> m_pixels;

    sf::RenderWindow m_window;

    sf::Time m_frameTime;
    bool m_paused = false;

public:
    CPixelApp(const std::string& title, const sf::Uint16 width, const sf::Uint16 height);
    virtual ~CPixelApp();

    int run();

protected:
    virtual bool onInit() = 0;
    virtual bool onEvent(const sf::Event& event) = 0;
    virtual void onUpdate(const sf::Time& frameTime) = 0;
    virtual void onDraw() = 0;

private:
    void updateScreenSpriteScale(sf::Sprite& sprite);
};
