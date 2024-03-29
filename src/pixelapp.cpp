#include "pixelapp.hpp"

CPixelApp::CPixelApp(const std::string& title, const sf::Uint16 width, const sf::Uint16 height) :
    m_title(title),
    m_pixelSize(width, height),
    m_pixels(m_pixelSize.x * m_pixelSize.y * 4)
{
    m_window.create(sf::VideoMode(width * 2, height * 2), m_title, sf::Style::Default);
}

CPixelApp::~CPixelApp()
{
}

int CPixelApp::run()
{
    sf::Event event;
    sf::Clock clock;

    if (!onInit())
    {
        return EXIT_FAILURE;
    }

    sf::Texture screenTexture;
    screenTexture.create(m_pixelSize.x, m_pixelSize.y);
    screenTexture.setSmooth(false);

    sf::Sprite screenSprite(screenTexture);

    updateScreenSpriteScale(screenSprite);


    while (m_window.isOpen())
    {
        m_frameTime = m_paused ? sf::Time::Zero : clock.restart();

        if (m_frameTime.asSeconds() > 0.33f)
        {
            // Skip a frame if it takes too long
            // Happens when the window is moved or resized
            continue;
        }

        while (m_window.pollEvent(event))
        {
            if (onEvent(event))
            {
                break;
            }

            switch (event.type)
            {
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Pause:
                            m_paused = !m_paused;
                            break;
                        case sf::Keyboard::Escape:
                            m_window.close();
                            break;
                        case sf::Keyboard::F:
                        {
                            const auto desktopMode = sf::VideoMode::getDesktopMode();
                            m_window.create(desktopMode, m_title, sf::Style::Fullscreen);
                            updateScreenSpriteScale(screenSprite);
                            break;
                        }
                        case sf::Keyboard::W:
                        {
                            const auto desktopMode = sf::VideoMode::getDesktopMode();
                            m_window.create(desktopMode, m_title, sf::Style::Default);
                            m_window.setSize({ 640u, 400u });
                            m_window.setPosition({
                                static_cast<int>((desktopMode.width - m_window.getSize().x) >> 1),
                                static_cast<int>((desktopMode.height - m_window.getSize().y) >> 1)
                            });
                            break;
                        }
                    }
                    break;
                case sf::Event::Closed:
                    m_window.close();
                    break;
                case sf::Event::Resized:
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    m_window.setView(sf::View(visibleArea));
                    updateScreenSpriteScale(screenSprite);
                    break;
            }
        }

        onUpdate(m_frameTime);
        onDraw();

        screenTexture.update(m_pixels.data());
        m_window.draw(screenSprite);
        m_window.display();
    }

    return EXIT_SUCCESS;
}

void CPixelApp::updateScreenSpriteScale(sf::Sprite& sprite)
{
    const auto windowSize = m_window.getSize();

    float scaleX = static_cast<float>(windowSize.x) / m_pixelSize.x;
    float scaleY = static_cast<float>(windowSize.y) / m_pixelSize.y;
    float scaleToFit = std::min(scaleX, scaleY);

    sprite.setScale(scaleToFit, scaleToFit);

    float posX = (windowSize.x - (m_pixelSize.x * scaleToFit)) / 2.0f;
    float posY = (windowSize.y - (m_pixelSize.y * scaleToFit)) / 2.0f;

    sprite.setPosition(posX, posY);
}
