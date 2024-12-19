#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath>

const float M_PI = 3.14159265358979323846f;  

bool IsBlind = false;

struct RayPoint {
    sf::Vertex start;  
    sf::Vertex end;    
    sf::Time creationTime;  
};

class Game {
public:
    Game() : window(sf::VideoMode(800, 600), "LiDAR Game") {

        clock.restart();

        player.setPosition(400, 300);  
        playerAngle = 0;  
        coneWidth = 60;    
        scanRange = 200;   
        player.setRadius(15);  
        player.setFillColor(sf::Color::Blue);  
        player.setOrigin(player.getRadius(), player.getRadius());  

        mapWidth = 20;
        mapHeight = 15;
        createMap();  
    }

    void run() {
        while (window.isOpen()) {
            processEvents();  
            update();          
            render();          
        }
    }

private:
    sf::RenderWindow window;
    sf::CircleShape player;  
    float playerAngle;  
    float coneWidth;    
    float scanRange;    
    int mapWidth, mapHeight;  
    std::vector<std::vector<int>> map;  
    std::vector<RayPoint> rayPoints;  
    sf::Clock clock;  

    void createMap() {

        map.resize(mapHeight, std::vector<int>(mapWidth, 0));

        map[4][5] = 1;  
        map[4][6] = 1;  
        map[5][5] = 1;  
        map[6][5] = 1;  
        map[10][8] = 1; 
        map[9][8] = 1;  
        map[8][8] = 1;  

        for (int i = 3; i < 13; ++i) {
            map[8][i] = 1;  
        }
    }

    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            handleInput(event);
        }
    }

    void handleInput(const sf::Event& event) {

        float moveSpeed = 5.0f;  
        sf::Vector2f newPosition = player.getPosition();  

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            newPosition.x -= moveSpeed;  
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            newPosition.x += moveSpeed;  
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            newPosition.y -= moveSpeed;  
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            newPosition.y += moveSpeed;  
        }

        if (!checkCollision(newPosition)) {
            player.setPosition(newPosition);  
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            playerAngle -= 2;  
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            playerAngle += 2;  
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            scan();  
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
            if (IsBlind) {
                IsBlind = false;
            }
            else {
                IsBlind = true;
            }
        }
    }

    bool checkCollision(const sf::Vector2f& newPosition) {

        int gridX = static_cast<int>(newPosition.x / 40);  
        int gridY = static_cast<int>(newPosition.y / 40);  

        if (gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
            if (map[gridY][gridX] == 1) {  
                return true;  
            }
        }

        return false;  
    }

    void update() {

    }

    void scan() {
        rayPoints.clear();  

        for (float i = -coneWidth / 2.0f; i <= coneWidth / 2.0f; i += 0.05f) {
            float angle = (playerAngle + i) * M_PI / 180.0f;  
            sf::Vector2f rayEnd = getRayEnd(angle, scanRange);  

            sf::Vector2f collisionPoint = getRayCollision(player.getPosition(), rayEnd);

            if (collisionPoint != sf::Vector2f(-1, -1)) {  
                rayEnd = collisionPoint;  
            }

            RayPoint ray;
            ray.start = sf::Vertex(player.getPosition(), sf::Color::Green);
            ray.end = sf::Vertex(rayEnd, sf::Color::Green);
            ray.creationTime = clock.getElapsedTime();  

            rayPoints.push_back(ray);
        }

        std::cout << "Ray Points Size: " << rayPoints.size() << std::endl;
    }

    sf::Vector2f getRayCollision(const sf::Vector2f& rayStart, const sf::Vector2f& rayEnd) {

        float dx = rayEnd.x - rayStart.x;
        float dy = rayEnd.y - rayStart.y;

        for (float t = 0; t < 1.0f; t += 0.0005f) {  
            float x = rayStart.x + t * dx;
            float y = rayStart.y + t * dy;

            int gridX = static_cast<int>(x / 40);  
            int gridY = static_cast<int>(y / 40);  
            if (gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
                if (map[gridY][gridX] == 1) {  
                    return sf::Vector2f(x, y);  
                }
            }
        }

        return sf::Vector2f(-1, -1);  
    }

    sf::Vector2f getRayEnd(float angle, float distance) {

        float dx = cos(angle) * distance;
        float dy = sin(angle) * distance;
        return player.getPosition() + sf::Vector2f(dx, dy);
    }

    void render() {
        window.clear(sf::Color::Black);  

        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                if (map[y][x] == 1) {  
                    sf::RectangleShape wall(sf::Vector2f(40, 40));  
                    wall.setPosition(x * 40, y * 40);  
                    if (IsBlind) {
                        wall.setFillColor(sf::Color::Black); 
                    }
                    else {
                        wall.setFillColor(sf::Color::Red);  
                    }
                    window.draw(wall);  
                }
            }
        }

        window.draw(player);  

        sf::Time currentTime = clock.getElapsedTime();  

        for (auto& rayPoint : rayPoints) {

            sf::Time elapsed = currentTime - rayPoint.creationTime;
            int alpha = 255 - static_cast<int>(elapsed.asSeconds() * 255);  

            if (alpha <= 0) {
                continue;  
            }

            rayPoint.start.color.a = alpha;
            rayPoint.end.color.a = alpha;

            window.draw(&rayPoint.start, 2, sf::Lines);
        }

        window.display();  
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
