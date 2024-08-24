
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "My SFML Window");

    while (window.isOpen()) {
        sf::Event event;
        while (event.type == sf::Event::Closed)
            window.close();

        window.clear();
        window.display();
    }

    return 0;
}