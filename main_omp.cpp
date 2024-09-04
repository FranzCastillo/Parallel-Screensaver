#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <random>
#include <omp.h>

struct Star {
    sf::ConvexShape shape;
    float lifetime;
};

sf::ConvexShape createStar(float radius, int points) {
    sf::ConvexShape star;
    star.setPointCount(points * 2);
    #pragma omp parallel for
    for (int i = 0; i < points * 2; i++) {
        float angle = i * 3.14159f / points;
        float r = (i % 2 == 0) ? radius : radius / 2;
        star.setPoint(i, sf::Vector2f(std::cos(angle) * r, std::sin(angle) * r));
    }
    star.setOrigin(radius, radius);
    return star;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Spiral Galaxy");

    const int numPoints = 15000;
    const float maxRadius = 450.0f;
    const float angleIncrement = 0.70f;
    const float speed = 0.035f;
    float baseRotationSpeed = 0.0007f;
    const int numArms = 5;

    sf::Vector2f center(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    std::vector<sf::CircleShape> points(numPoints);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        return -1;
    }

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    sf::Clock clock;
    std::vector<Star> extraStars;
    sf::Clock starClock;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    #pragma omp parallel for
    for (int i = 0; i < numPoints; ++i) {
        float angleOffset = dis(gen) * 2 * 3.14159f;
        float armAngle = (i * angleIncrement) + (2 * 3.14159f / numArms) * (rand() % numArms) + angleOffset * 0.5f;
        float radius = maxRadius * std::sqrt(dis(gen)) * (0.9f + 0.2f * dis(gen));
        float x = center.x + radius * std::cos(armAngle);
        float y = center.y + radius * std::sin(armAngle);

        sf::CircleShape star((rand() % 10) < 3 ? 2.0f : 1.0f);
        star.setPosition(x, y);

        sf::Color color;
        float normalizedRadius = radius / maxRadius;
        if (normalizedRadius < 0.3f) {
            color = sf::Color(200, 200, 100 + static_cast<sf::Uint8>(55 * normalizedRadius / 0.3f));
        } else if (normalizedRadius < 0.7f) {
            color = sf::Color(static_cast<sf::Uint8>(100 * (1 - (normalizedRadius - 0.3f) / 0.4f)),
                              static_cast<sf::Uint8>(100 + 55 * (normalizedRadius - 0.3f) / 0.4f), 200);
        } else {
            color = sf::Color(80 + static_cast<sf::Uint8>(127 * (normalizedRadius - 0.7f) / 0.3f), 0, 180);
        }
        star.setFillColor(color);
        points[i] = star;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float currentTime = clock.restart().asSeconds();
        float fps = 1.0f / currentTime;
        std::stringstream ss;
        ss << "FPS: " << static_cast<int>(fps);
        fpsText.setString(ss.str());

        window.clear(sf::Color::Black);

        // Parallelize only the computation of new positions
        #pragma omp parallel for
        for (int i = 0; i < numPoints; ++i) {
            sf::Vector2f pos = points[i].getPosition();
            float dx = pos.x - center.x;
            float dy = pos.y - center.y;
            float angle = std::atan2(dy, dx);
            float radius = std::sqrt(dx * dx + dy * dy);

            float rotationSpeed = baseRotationSpeed * (1.0f + (maxRadius - radius) / maxRadius);
            angle += rotationSpeed;
            float attractionSpeed = speed * (1.0f + (maxRadius - radius) / maxRadius);
            radius -= attractionSpeed;

            if (radius < 1) {
                radius = maxRadius;
                angle += dis(gen) * 0.2f - 0.1f;
                radius *= 0.9f + dis(gen) * 0.2f;

                float normalizedRadius = radius / maxRadius;
                sf::Color newColor;
                if (normalizedRadius < 0.3f) {
                    newColor = sf::Color(200, 200, 100 + static_cast<sf::Uint8>(55 * normalizedRadius / 0.3f));
                } else if (normalizedRadius < 0.7f) {
                    newColor = sf::Color(static_cast<sf::Uint8>(100 * (1 - (normalizedRadius - 0.3f) / 0.4f)),
                                         static_cast<sf::Uint8>(100 + 55 * (normalizedRadius - 0.3f) / 0.4f), 200);
                } else {
                    newColor = sf::Color(80 + static_cast<sf::Uint8>(127 * (normalizedRadius - 0.7f) / 0.3f), 0, 180);
                }
                points[i].setFillColor(newColor);
            }

            float x = center.x + radius * std::cos(angle);
            float y = center.y + radius * std::sin(angle);

            // Update position outside of parallel region
            points[i].setPosition(x, y);
        }

        // Render points sequentially to avoid OpenGL issues
        for (int i = 0; i < numPoints; ++i) {
            window.draw(points[i]);
        }

        window.draw(fpsText);
        window.display();
    }

    return 0;
}
