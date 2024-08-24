
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Screen Saver");

    // Create a circle
    sf::CircleShape circle(50); // Radius of the circle
    circle.setFillColor(sf::Color::Green); // Circle color

    // Circle position
    circle.setPosition(375, 275); // Initial position of the circle

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close the window if the user clicks on the close button
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window with a black color
        window.clear(sf::Color::Black);

        // Draw the circle
        window.draw(circle);

        // Show the window
        window.display();
    }

    return 0;
}