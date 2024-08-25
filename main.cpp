#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <sstream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "Spiral Galaxy");

    // Parameters for the galaxy
    const int numPoints = 15000;             // Number of points in the galaxy
    const float maxRadius = 450.0f;          // Maximum radius of the galaxy
    const float angleIncrement = 0.70f;      // Angle increment for the spiral arms
    const float speed = 0.035f;              // Speed of the points moving towards the center
    float baseRotationSpeed = 0.0007f;       // Base speed of the rotation of the galaxy
    const int numArms = 5;                   // Number of spiral arms

    // Center of the window
    sf::Vector2f center(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    // Vector to store all the points in the galaxy
    std::vector<sf::CircleShape> points(numPoints);

    // Set a font for the fps counter
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        return -1;
    }

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    // Clock to measure the time between frames
    sf::Clock clock;

    // Generate the points in the galaxy
    for (int i = 0; i < numPoints; ++i) {
        // Add a random offset to the angle
        float angleOffset = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
        float armAngle = (i * angleIncrement) + (2 * 3.14159f / numArms) * (rand() % numArms) + angleOffset * 0.5f;

        // Calculate the initial position of the point
        float radius = maxRadius * std::sqrt(static_cast<float>(rand() % 1000) / 1000.0f) * (0.9f + 0.2f * (static_cast<float>(rand()) / RAND_MAX));

        // Calculate the position of the point
        float x = center.x + radius * std::cos(armAngle);
        float y = center.y + radius * std::sin(armAngle);

        // Create a point with a radius, generate a random number and if it is less than 0.7, create a point with a radius of 1 pixel, otherwise create a point with a radius of 2 pixels
        sf::CircleShape star((rand() % 10) < 3 ? 2.0f : 1.0f);
        star.setPosition(x, y);

        // Set a random color for the point
        sf::Color color(rand() % 256, rand() % 256, rand() % 256);
        star.setFillColor(color);

        // Add the point to the vector
        points[i] = star;
    }

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Calculate the time between frames
        float currentTime = clock.restart().asSeconds();
        float fps = 1.0f / currentTime;

        // Update the fps counter
        std::stringstream ss;
        ss << "FPS: " << static_cast<int>(fps);
        fpsText.setString(ss.str());

        window.clear(sf::Color::Black); // Clear the window with a black color

        // Update the position of the points
        for (int i = 0; i < numPoints; ++i) {
            // Get the current position of the point
            sf::Vector2f pos = points[i].getPosition();

            // Calculate the distance from the center
            float dx = pos.x - center.x;
            float dy = pos.y - center.y;

            // Calculate the angle and radius of the point
            float angle = std::atan2(dy, dx);
            float radius = std::sqrt(dx * dx + dy * dy);

            // Adjust rotation speed based on distance from center
            float rotationSpeed = baseRotationSpeed * (1.0f + (maxRadius - radius) / maxRadius);

            // Rotate the point faster if it is closer to the center
            angle += rotationSpeed;

            float attractionSpeed = speed * (1.0f + (maxRadius - radius) / maxRadius);

            // Move the point towards the center
            radius -= attractionSpeed;

            // If the point is too close to the center, move it to the edge of the galaxy
            if (radius < 0) {
                radius = maxRadius;

                // Add some random variation to the angle and radius
                angle += static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f; // Slight random variation in angle
                radius *= 0.9f + static_cast<float>(rand()) / RAND_MAX * 0.2f; // Slight random variation in radius

                // Set a new random color for the point
                sf::Color newColor(rand() % 256, rand() % 256, rand() % 256);
                points[i].setFillColor(newColor);
            }

            // Calculate the new position of the point
            float x = center.x + radius * std::cos(angle);
            float y = center.y + radius * std::sin(angle);

            // Set the new position of the point
            points[i].setPosition(x, y);

            // Draw the point
            window.draw(points[i]);
        }

        window.draw(fpsText); // Draw the fps counter

        window.display(); // Display the window
    }

    return 0;
}
