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
    // Crear la ventana
    sf::RenderWindow window(sf::VideoMode(800, 800), "Spiral Galaxy");

    // Parámetros de la galaxia
    const int numPoints = 15000; // Número de puntos en la galaxia
    const float maxRadius = 450.0f; // Radio máximo de la galaxia
    const float angleIncrement = 0.70f; // Incremento de ángulo para los brazos espirales
    const float speed = 0.035f; // Velocidad de los puntos moviéndose hacia el centro
    float baseRotationSpeed = 0.0007f; // Velocidad base de la rotación de la galaxia
    const int numArms = 5; // Número de brazos espirales

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

    // Generar los puntos en la galaxia (como círculos)
    std::random_device rd; // Generador de números aleatorios
    std::mt19937 gen(rd()); // Semilla para el generador
    std::uniform_real_distribution<> dis(0, 1); // Distribución uniforme de 0 a 1

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
            // Amarillo oscuro hacia el centro
            color = sf::Color(
                200,
                200,
                100 + static_cast<sf::Uint8>(55 * normalizedRadius / 0.3f)
            );
        }
        else if (normalizedRadius < 0.7f)
        {
            // Azul oscuro en la zona media
            color = sf::Color(
                static_cast<sf::Uint8>(100 * (1 - (normalizedRadius - 0.3f) / 0.4f)),
                static_cast<sf::Uint8>(100 + 55 * (normalizedRadius - 0.3f) / 0.4f),
                200
            );
        }
        else
        {
            // Violeta oscuro en los bordes
            color = sf::Color(
                80 + static_cast<sf::Uint8>(127 * (normalizedRadius - 0.7f) / 0.3f),
                0,
                180
            );
        }

        star.setFillColor(color);
        // Añadir el punto al vector
        points[i] = star;
    }

    // Bucle principal
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Calcular el tiempo entre cuadros
        float currentTime = clock.restart().asSeconds();
        float fps = 1.0f / currentTime;

        // Actualizar el contador de FPS
        std::stringstream ss;
        ss << "FPS: " << static_cast<int>(fps);
        fpsText.setString(ss.str());

        window.clear(sf::Color::Black); // Limpiar la ventana con un color negro

        // Actualizar la posición de los puntos (círculos de la galaxia)
#pragma omp parallel for
        for (int i = 0; i < numPoints; ++i)
        {
            // Obtener la posición actual del punto
            sf::Vector2f pos = points[i].getPosition();

            // Calcular la distancia desde el centro
            float dx = pos.x - center.x;
            float dy = pos.y - center.y;

            // Calcular el ángulo y radio del punto
            float angle = std::atan2(dy, dx);
            float radius = std::sqrt(dx * dx + dy * dy);

            // Ajustar la velocidad de rotación en función de la distancia desde el centro
            float rotationSpeed = baseRotationSpeed * (1.0f + (maxRadius - radius) / maxRadius);

            // Rotar el punto más rápido si está más cerca del centro
            angle += rotationSpeed;

            float attractionSpeed = speed * (1.0f + (maxRadius - radius) / maxRadius);

            // Mover el punto hacia el centro
            radius -= attractionSpeed;

            // Si el punto está demasiado cerca del centro, moverlo al borde de la galaxia
            if (radius < 0)
            {
                radius = maxRadius;
                angle += dis(gen) * 0.2f - 0.1f; // Variación aleatoria ligera en el ángulo
                radius *= 0.9f + dis(gen) * 0.2f; // Variación aleatoria en el radio

                float normalizedRadius = radius / maxRadius;
                sf::Color newColor;
                if (normalizedRadius < 0.3f)
                {
                    // Amarillo oscuro hacia el centro
                    newColor = sf::Color(
                        200,
                        200,
                        100 + static_cast<sf::Uint8>(55 * normalizedRadius / 0.3f)
                    );
                }
                else if (normalizedRadius < 0.7f)
                {
                    // Azul oscuro en la zona media
                    newColor = sf::Color(
                        static_cast<sf::Uint8>(100 * (1 - (normalizedRadius - 0.3f) / 0.4f)),
                        static_cast<sf::Uint8>(100 + 55 * (normalizedRadius - 0.3f) / 0.4f),
                        200
                    );
                }
                else
                {
                    // Violeta oscuro en los bordes
                    newColor = sf::Color(
                        80 + static_cast<sf::Uint8>(127 * (normalizedRadius - 0.7f) / 0.3f),
                        0,
                        180
                    );
                }
                points[i].setFillColor(newColor);
            }

            // Calcular la nueva posición del punto
            float x = center.x + radius * std::cos(angle);
            float y = center.y + radius * std::sin(angle);

            // Establecer la nueva posición del punto
            points[i].setPosition(x, y);

            // Dibujar el punto
            window.draw(points[i]);
        }

        // Generar estrellas adicionales al azar
        if (starClock.getElapsedTime().asSeconds() > 0.1f)
        {
            Star newStar;
            float starRadius = dis(gen) * maxRadius;
            float starAngle = dis(gen) * 2 * 3.14159f;
            newStar.shape = createStar(6.0f, 5);
            newStar.shape.setPosition(
                center.x + starRadius * std::cos(starAngle),
                center.y + starRadius * std::sin(starAngle)
            );
            newStar.shape.setFillColor(sf::Color(255, 255, 255, 255)); // Mayor opacidad inicial
            newStar.lifetime = 1.5f; // Duración de la estrella en segundos
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
                float alpha = static_cast<sf::Uint8>(255 * (star.lifetime / 1.5f)); // Ajuste del tiempo de vida
                star.shape.setFillColor(sf::Color(255, 255, 255, alpha));
                ++i;
            }
        }

        window.draw(fpsText); // Dibujar el contador de FPS

        window.display(); // Mostrar la ventana
    }

    return 0;
}
