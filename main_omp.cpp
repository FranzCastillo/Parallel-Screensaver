#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <random>
#include <omp.h>

struct Star
{
    sf::ConvexShape shape;
    float lifetime;
};

// Function to create star shapes with a given radius and number of points
sf::ConvexShape createStar(float radius, int points)
{
    sf::ConvexShape star;
    star.setPointCount(points * 2);

#pragma omp parallel for
    for (int i = 0; i < points * 2; i++)
    {
        float angle = i * 3.14159f / points;
        float r = (i % 2 == 0) ? radius : radius / 2;
        star.setPoint(i, sf::Vector2f(std::cos(angle) * r, std::sin(angle) * r));
    }

    star.setOrigin(radius, radius);
    return star;
}

int main()
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode(800, 800), "Spiral Galaxy");

    // Galaxy parameters
    const int numPoints = 15000; // Number of points in the galaxy
    const float maxRadius = 450.0f; // Maximum radius of the galaxy
    const float angleIncrement = 0.70f; // Angle increment for spiral arms
    const float speed = 0.035f; // Speed of points moving toward the center
    float baseRotationSpeed = 0.0007f; // Base rotation speed of the galaxy
    const int numArms = 5; // Number of spiral arms

    // Center of the window
    sf::Vector2f center(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    // Vector to store all galaxy points
    std::vector<sf::CircleShape> points(numPoints);

    // Load font for FPS counter
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        return -1;
    }

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    // Clock to measure time between frames
    sf::Clock clock;

    // Vector to store additional stars
    std::vector<Star> extraStars;
    sf::Clock starClock;

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    // Generate the points in the galaxy (as circles)
#pragma omp parallel for
    for (int i = 0; i < numPoints; ++i)
    {
        float angleOffset = dis(gen) * 2 * 3.14159f;
        float armAngle = (i * angleIncrement) + (2 * 3.14159f / numArms) * (rand() % numArms) + angleOffset * 0.5f;

        float radius = maxRadius * std::sqrt(dis(gen)) * (0.9f + 0.2f * dis(gen));
        float x = center.x + radius * std::cos(armAngle);
        float y = center.y + radius * std::sin(armAngle);

        // Create a point with a radius of 1 or 2 pixels (circles)
        sf::CircleShape star((rand() % 10) < 3 ? 2.0f : 1.0f);
        star.setPosition(x, y);

        // Determine the color based on the distance from the center
        sf::Color color;
        float normalizedRadius = radius / maxRadius;
        if (normalizedRadius < 0.3f)
        {
            color = sf::Color(200, 200, 100 + static_cast<sf::Uint8>(55 * normalizedRadius / 0.3f));
        }
        else if (normalizedRadius < 0.7f)
        {
            color = sf::Color(static_cast<sf::Uint8>(100 * (1 - (normalizedRadius - 0.3f) / 0.4f)),
                              static_cast<sf::Uint8>(100 + 55 * (normalizedRadius - 0.3f) / 0.4f), 200);
        }
        else
        {
            color = sf::Color(80 + static_cast<sf::Uint8>(127 * (normalizedRadius - 0.7f) / 0.3f), 0, 180);
        }
        star.setFillColor(color);

        points[i] = star;
    }

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float currentTime = clock.restart().asSeconds();
        float fps = 1.0f / currentTime;

        // Update FPS counter
        std::stringstream ss;
        ss << "FPS: " << static_cast<int>(fps);
        fpsText.setString(ss.str());

        window.clear(sf::Color::Black);

        // Update the position of galaxy points
#pragma omp parallel for
        for (int i = 0; i < numPoints; ++i)
        {
            sf::Vector2f pos = points[i].getPosition();
            float dx = pos.x - center.x;
            float dy = pos.y - center.y;

            float angle = std::atan2(dy, dx);
            float radius = std::sqrt(dx * dx + dy * dy);

            float rotationSpeed = baseRotationSpeed * (1.0f + (maxRadius - radius) / maxRadius);
            angle += rotationSpeed;

            float attractionSpeed = speed * (1.0f + (maxRadius - radius) / maxRadius);
            radius -= attractionSpeed;

            if (radius < 0)
            {
                radius = maxRadius;
                angle += dis(gen) * 0.2f - 0.1f;
                radius *= 0.9f + dis(gen) * 0.2f;

                float normalizedRadius = radius / maxRadius;
                sf::Color newColor;
                if (normalizedRadius < 0.3f)
                {
                    newColor = sf::Color(200, 200, 100 + static_cast<sf::Uint8>(55 * normalizedRadius / 0.3f));
                }
                else if (normalizedRadius < 0.7f)
                {
                    newColor = sf::Color(static_cast<sf::Uint8>(100 * (1 - (normalizedRadius - 0.3f) / 0.4f)),
                                         static_cast<sf::Uint8>(100 + 55 * (normalizedRadius - 0.3f) / 0.4f), 200);
                }
                else
                {
                    newColor = sf::Color(80 + static_cast<sf::Uint8>(127 * (normalizedRadius - 0.7f) / 0.3f), 0, 180);
                }
                points[i].setFillColor(newColor);
            }

            float x = center.x + radius * std::cos(angle);
            float y = center.y + radius * std::sin(angle);

            points[i].setPosition(x, y);
        }

        // Draw galaxy points
        for (int i = 0; i < numPoints; ++i)
        {
            window.draw(points[i]);
        }

        // Generate additional stars randomly
        if (starClock.getElapsedTime().asSeconds() > 0.1f)
        {
            Star newStar;
            float starRadius = dis(gen) * maxRadius;
            float starAngle = dis(gen) * 2 * 3.14159f;
            newStar.shape = createStar(6.0f, 5); // Create a larger star
            newStar.shape.setPosition(
                center.x + starRadius * std::cos(starAngle),
                center.y + starRadius * std::sin(starAngle)
            );
            newStar.shape.setFillColor(sf::Color(255, 255, 255, 255)); // Full opacity initially
            newStar.lifetime = 1.5f; // Star lifetime in seconds
            extraStars.push_back(newStar);
            starClock.restart();
        }

        // Draw and update additional stars
        for (size_t i = 0; i < extraStars.size();)
        {
            Star& star = extraStars[i];
            window.draw(star.shape);
            star.lifetime -= currentTime;
            if (star.lifetime <= 0.0f)
            {
                extraStars.erase(extraStars.begin() + i);
            }
            else
            {
                float alpha = static_cast<sf::Uint8>(255 * (star.lifetime / 1.5f)); // Adjust opacity based on lifetime
                star.shape.setFillColor(sf::Color(255, 255, 255, alpha));
                ++i;
            }
        }

        window.draw(fpsText); // Draw the FPS counter

        window.display(); // Display the window
    }

    return 0;
}
