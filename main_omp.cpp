/*
* Nombre: main_omp.cpp
* Autores:
  - Andrés Montoya, 21552
  - Fernanda Esquivel, 21542
  - Francisco Castillo, 21562
* Descripción: Screen Saver de Galaxia creado con programación paralela (OpenMP).
* Lenguaje: C++
* Recursos: CLion, SFML
* Historial:
  - Creado el 03.09.2024
  - Modificado el 04.09.2024
*/

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <random>
#include <omp.h>
#include <iostream>
#include <limits>

struct Star
{
    sf::ConvexShape shape;
    float lifetime;
};

// Función para crear la forma de estrella
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

// Función para solicitar parámetros personalizados
void askParameters(int &numPoints, float &maxRadius, float &speed, float &baseRotationSpeed) {
    char userChoice;
    std::cout << "¿Desea usar los parametros por defecto? (y/n): ";
    std::cin >> userChoice;

    if (userChoice == 'n' || userChoice == 'N') {
        std::cout << "Ingresa los parametros personalizados:\n";

        // Solicitar numPoints
        do {
            std::cout << "Numero de puntos (5000 - 30000): ";
            std::cin >> numPoints;
            if (std::cin.fail() || numPoints < 5000 || numPoints > 30000) {
                std::cout << "Valor invalido. Debe estar entre 5000 y 30000.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } while (numPoints < 5000 || numPoints > 30000);

        // Solicitar maxRadius
        do {
            std::cout << "Radio maximo (300.0 - 450.0): ";
            std::cin >> maxRadius;
            if (std::cin.fail() || maxRadius < 300.0f || maxRadius > 450.0f) {
                std::cout << "Valor invalido. Debe estar entre 300.0 y 450.0.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } while (maxRadius < 300.0f || maxRadius > 450.0f);

        // Solicitar speed
        do {
            std::cout << "Velocidad (0.035 - 0.099): ";
            std::cin >> speed;
            if (std::cin.fail() || speed < 0.035f || speed > 0.099f) {
                std::cout << "Valor invalido. Debe estar entre 0.035 y 0.099.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } while (speed < 0.035f || speed > 0.099f);

        // Solicitar baseRotationSpeed
        do {
            std::cout << "Velocidad de rotacion base (0.0007 - 0.007): ";
            std::cin >> baseRotationSpeed;
            if (std::cin.fail() || baseRotationSpeed < 0.0007f || baseRotationSpeed > 0.007f) {
                std::cout << "Valor invalido. Debe estar entre 0.0007 y 0.007.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } while (baseRotationSpeed < 0.0007f || baseRotationSpeed > 0.007f);
    } else {
        std::cout << "Usando parametros por defecto...\n";
    }
}

int main()
{
    // Parámetros por defecto de la galaxia
    int numPoints = 15000;             // Número de puntos en la galaxia
    float maxRadius = 450.0f;          // Radio máximo de la galaxia
    const float angleIncrement = 0.70f; // Incremento de ángulo para los brazos espirales
    float speed = 0.035f;              // Velocidad de los puntos moviéndose hacia el centro
    float baseRotationSpeed = 0.0007f; // Velocidad base de la rotación de la galaxia
    const int numArms = 5;             // Número de brazos espirales

    // Solicitar los parámetros al usuario
    askParameters(numPoints, maxRadius, speed, baseRotationSpeed);

    // Crear la ventana
    sf::RenderWindow window(sf::VideoMode(800, 800), "Spiral Galaxy");

    // Centro de la ventana
    sf::Vector2f center(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    // Vector para almacenar todos los puntos de la galaxia
    std::vector<sf::CircleShape> points(numPoints);

    // Fuente para el contador de FPS
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

    // Reloj para medir el tiempo entre cuadros
    sf::Clock clock;

    // Vector para almacenar las estrellas adicionales
    std::vector<Star> extraStars;
    sf::Clock starClock;

    // Generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    // Generar los puntos en la galaxia (como círculos)
#pragma omp parallel for
    for (int i = 0; i < numPoints; ++i)
    {
        float angleOffset = dis(gen) * 2 * 3.14159f;
        float armAngle = (i * angleIncrement) + (2 * 3.14159f / numArms) * (rand() % numArms) + angleOffset * 0.5f;

        float radius = maxRadius * std::sqrt(dis(gen)) * (0.9f + 0.2f * dis(gen));
        float x = center.x + radius * std::cos(armAngle);
        float y = center.y + radius * std::sin(armAngle);

        // Crear un punto con un radio de 1 o 2 píxeles (círculos)
        sf::CircleShape star((rand() % 10) < 3 ? 2.0f : 1.0f);
        star.setPosition(x, y);

        // Determinar el color basado en la distancia desde el centro
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

        // Actualizar el contador de FPS
        std::stringstream ss;
        ss << "FPS: " << static_cast<int>(fps);
        fpsText.setString(ss.str());

        window.clear(sf::Color::Black);

        // Actualizar la posición de los puntos de la galaxia
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

        // Dibujar los puntos de la galaxia
        for (int i = 0; i < numPoints; ++i)
        {
            window.draw(points[i]);
        }

        // Generar estrellas adicionales al azar
        if (starClock.getElapsedTime().asSeconds() > 0.1f)
        {
            Star newStar;
            float starRadius = dis(gen) * maxRadius;
            float starAngle = dis(gen) * 2 * 3.14159f;
            newStar.shape = createStar(6.0f, 5); // Crear una estrella más grande
            newStar.shape.setPosition(
                center.x + starRadius * std::cos(starAngle),
                center.y + starRadius * std::sin(starAngle)
            );
            newStar.shape.setFillColor(sf::Color(255, 255, 255, 255)); // Opacidad inicial completa
            newStar.lifetime = 1.5f; // Tiempo de vida de la estrella en segundos
            extraStars.push_back(newStar);
            starClock.restart();
        }

        // Dibujar y actualizar las estrellas adicionales
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
                float alpha = static_cast<sf::Uint8>(255 * (star.lifetime / 1.5f)); // Ajustar opacidad basada en el tiempo de vida
                star.shape.setFillColor(sf::Color(255, 255, 255, alpha));
                ++i;
            }
        }

        window.draw(fpsText); // Dibujar el contador de FPS

        window.display(); // Mostrar la ventana
    }

    return 0;
}
