//
// Created by iguti on 07/02/2024.
//

#ifndef ANTS_MAIN_CPP
#define ANTS_MAIN_CPP

#define WIDTH 1920
#define HEIGHT 1080

#include <SFML/Graphics.hpp>
#include <iostream>
#include <omp.h>
#include <complex>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */



enum GridType {
    FOOD = 0,
    HOME = 1,
};

class Map {


public:

    enum EnvType {
        RECOURCE = 0,
        WALL = 1,
        WALL_DESTROY = 2,
        MUD = 3,
        RIVER = 4,
        NONE = 5,
    };

    std::vector<sf::Color> colors = {sf::Color(102, 48, 117),
                                     sf::Color(54, 51, 48),
                                     sf::Color(82, 78, 74),
                                     sf::Color(145, 121, 89),
                                     sf::Color(89, 145, 134),
                                     sf::Color::Transparent
    };

    const sf::Color getColor(EnvType type) {
        return colors[type];
    }

    float windowWidth = WIDTH;
    float windowHeight = HEIGHT;
    sf::VertexArray gridMap;

    Map() {
        gridMap = sf::VertexArray(sf::Points, windowWidth*windowHeight);
        for (int x = 0; x < windowWidth; x++) {
            for (int y = 0; y < windowHeight; y++) {
                gridMap[x*windowHeight + y].position.x = x;
                gridMap[x*windowHeight + y].position.y = y;
                gridMap[x*windowHeight + y].color = getColor(EnvType::NONE);
            }
        }
    }

    EnvType getMode(int x, int y, const GridType &mode) {
        if ((x >= windowWidth || x < 0) || (y >= windowHeight || y < 0)) {
            return EnvType::NONE;
        }
        if (gridMap[x*windowHeight+y].color == getColor(EnvType::RECOURCE) && mode == GridType::FOOD) {
            if (rand() % 10 == 0) {
                gridMap[x*windowHeight+y].color = getColor(EnvType::NONE);
            }
            return EnvType::RECOURCE;
        }
        if (gridMap[x*windowHeight+y].color == getColor(EnvType::WALL)) {
            return EnvType::WALL;
        }
        if (gridMap[x*windowHeight+y].color == getColor(EnvType::RIVER)) {
            return EnvType::RIVER;
        }
        if (gridMap[x*windowHeight+y].color == getColor(EnvType::MUD)) {
            return EnvType::MUD;
        }
        if (gridMap[x*windowHeight+y].color == getColor(EnvType::WALL_DESTROY)) {
            if (rand() % 100 == 0) {
                gridMap[x*windowHeight+y].color = getColor(EnvType::NONE);
            }
            return EnvType::WALL_DESTROY;
        }
        return EnvType::NONE;
    }

    void placeCircle(int a, int b, sf::Color c, int size) {
        for (int x = 0; x < windowWidth; x++) {
            for (int y = 0; y < windowHeight; y++) {
                if ((a - x)*(a - x) + (b - y)*(b - y) < size) {
                    gridMap[x*windowHeight+y].color = c;
                }
            }
        }
    }

    void placeCircleVoid(int a, int b, sf::Color c, int size) {
        for (int x = 0; x < windowWidth; x++) {
            for (int y = 0; y < windowHeight; y++) {
                if ((a - x)*(a - x) + (b - y)*(b - y) < size && gridMap[x*windowHeight+y].color == getColor(EnvType::NONE)) {
                    gridMap[x*windowHeight+y].color = c;
                }
            }
        }
    }

    void placeRect(int a, int b, sf::Color c, int size) {
        for (int x = 0; x < windowWidth; x++) {
            for (int y = 0; y < windowHeight; y++) {
                if (std::max((a - x)*(a - x),(b - y)*(b - y)) < size) {
                    gridMap[x*windowHeight+y].color = c;
                }
            }
        }
    }

    void placeEnv(int x, int y, EnvType type) {
        if (type == EnvType::RECOURCE) {
            placeCircle(x, y, getColor(type), 1000);
            return;
        }
        if (type == EnvType::WALL) {
            placeCircleVoid(x, y, getColor(EnvType::MUD), 700);
            placeCircle(x, y, getColor(type), 500);

            return;
        }
        placeCircle(x, y, getColor(type), 500);
    }
};


class Grid {

    public:
        GridType grid = GridType::FOOD;

        float windowWidth = WIDTH;
        float windowHeight = HEIGHT;
        int grdSize = 500;

        std::vector<std::vector<float>> val = std::vector<std::vector<float>>(grdSize, std::vector<float>(grdSize, 10000.));
        std::vector<std::vector<int>> counter = std::vector<std::vector<int>>(grdSize, std::vector<int>(grdSize, 0));

        GridType getGridType() const {
            return grid;
        };

        [[nodiscard]] float get(int x,int y) const {
            x = int((x / windowWidth) * grdSize);
            y = int((y / windowHeight) * grdSize);
            if (x >= grdSize || x < 0 || y < 0 || y >= grdSize) {
                return 10000.;
            }
            return val[x][y];
        }

        void set(int x,int y, float z) {
            x = int((x / windowWidth) * grdSize);
            y = int((y / windowHeight) * grdSize);
            if (x >= grdSize || x < 0 || y < 0 || y >= grdSize) {
                return;
            }
            if (z < val[x][y]) {
                val[x][y] = z + 0.*val[x][y];
                if (this->grid == GridType::HOME) {
                    counter[x][y] = 50; // 50
                } else {
                    counter[x][y] = 100; // 100
                }
            };
        }

        sf::VertexArray draw() {
            sf::VertexArray drawGrid = sf::VertexArray(sf::Points, windowWidth*windowHeight);
            for (int x = 0; x < windowWidth; x++) {
                for (int y = 0; y < windowHeight; y++) {
                    drawGrid[x*windowHeight + y].position.x = x;
                    drawGrid[x*windowHeight + y].position.y = y;
                    drawGrid[x*windowHeight + y].color = sf::Color(0, std::min(255., 255./(this->get(x,y)/100.)), 0);
                }
            }
            return drawGrid;
        }

        void update() {
            for (int x = 0; x < grdSize; x++) {
                for (int y = 0; y < grdSize; y++) {
                    float avg = 0.0;
                    int c = 0;
                    for (int a = -1; a < 2; a++) {
                        for (int b = -1; b < 2; b++) {
                            if (x+a < 0 || x+a >= grdSize) {
                                continue;
                            }
                            if (y+b < 0 || y+b >= grdSize) {
                                continue;
                            }
                            c++;
                            avg += val[x+a][y+b];
                        }
                    }
                    avg /= c;
                    if (avg < val[x][y]) {
                        val[x][y] = (0.7*avg + 0.3*val[x][y]);
                        counter[x][y] = 3;
                    }
                    if (counter[x][y] > 0)
                        counter[x][y]--;

                    if (counter[x][y] <= 1 && val[x][y] < 10000) {
                        val[x][y] *= val[x][y]*2;
                    }
                    if (val[x][y] >= 10000) {
                        val[x][y] = 10000;
                    }
                }
            }
        }

};


class Ant {
    public:

        GridType mode = GridType::FOOD;

        sf::Vector2f position;
        sf::Vector2f velocity;

        const std::vector<float> positions = {
                0.,
                -0.2,
                0.2,
        };

        Ant() {
            this->mode = GridType::FOOD;

            this->position = sf::Vector2f(100, 100);
            float x = (rand()%10000 / 10000.) * 2*3.14;
            float sped = 3.;
            this->velocity = sf::Vector2f(sped*std::cos(x), sped*std::sin(x));
        }

        float distance = 0.f;

        float gridView(float b, const Grid &g) {
            float x = velocity.x*std::cos(b) - std::sin(b)*velocity.y;
            float y = velocity.x*std::sin(b) + std::cos(b)*velocity.y;
            float factor = 5;
            sf::Vector2f vel = sf::Vector2f(x*factor,y*factor);
            sf::Vector2f out = position + vel;
            return g.get(int(out.x), int(out.y));
        }

        void placeGrid(Grid &g) {
            g.set(int(position.x), int(position.y), distance);
        }

        sf::Vector2f rotateVec(sf::Vector2f& velocity, float b) {
            float x = velocity.x*std::cos(b) - std::sin(b)*velocity.y;
            float y = velocity.x*std::sin(b) + std::cos(b)*velocity.y;
            return sf::Vector2f{x,y};
        }

        void update(std::vector<Grid>& pheromones, sf::Vector2i& p2, Map &m) {
            // check in front

            sf::Vector2f vel;
            float min = 10000;
            float rot = 0;
            for (float p : this->positions) {
                float c;
                if (mode == GridType::HOME) {
                    c = gridView(p, pheromones[0]);
                } else {
                    c = gridView(p, pheromones[1]);
                }

                if (min > c) {
                    vel = rotateVec(this->velocity, p);
                    min = c;
                    rot = p;
                }
            }
            if (min <= 10) {
                vel = this->velocity;
            } else {
                vel = rotateVec(this->velocity, rot);
            }
            velocity = vel;
            position += velocity;
            distance += 5;//std::sqrt(velocity.x*velocity.x + velocity.y*velocity.y);

            if (mode == GridType::HOME) {
                placeGrid(pheromones[1]);
            } else {
                placeGrid(pheromones[0]);
            };


            if (position.x < 0) {
                position.x = 0;
                velocity.x = -velocity.x;
            }
            if (position.x > WIDTH) {
                position.x = WIDTH;
                velocity.x = -velocity.x;
            }

            if (position.y > HEIGHT) {
                position.y = HEIGHT;
                velocity.y = -velocity.y;
            }
            if (position.y < 0) {
                position.y = 0;
                velocity.y = -velocity.y;
            }

            // home
            if ((position.x - 100)*(position.x - 100) + (position.y - 100)*(position.y - 100) < 1000 && mode == GridType::HOME) {
                distance = 0;
                mode = GridType::FOOD;
                velocity.y = -velocity.y;
                velocity.x = -velocity.x;
            }



            Map::EnvType underground = m.getMode(position.x, position.y, this->mode);
            if (underground == Map::RECOURCE) {
                distance = 0;
                mode = GridType::HOME;
                position += velocity;
                velocity.x = -velocity.x;
                velocity.y = -velocity.y;

            }

            if (underground == Map::WALL || underground == Map::WALL_DESTROY) {
                while (underground == Map::WALL || underground == Map::WALL_DESTROY) {
                    underground = m.getMode(position.x, position.y, this->mode);
                    position -= velocity;
                }
                this->velocity = -rotateVec(this->velocity, rand()%100 / 100.f * 1 - 0.5);
            }

            if (underground == Map::MUD) {
                position -= velocity;
                position.x += velocity.x * 0.1;
                position.y += velocity.y * 0.1;
            }

            if (underground == Map::RIVER) {
                position -= velocity;
                position.x += velocity.x * 1.5;
                position.y += velocity.y * 1.5;
            }

        }
};

static std::vector<sf::Texture> textures(2);
void loadTextures() {
    std::string file = "..\\..\\textures\\ant\\ant_default.png";
    textures[0].loadFromFile(file);
    file = "..\\..\\textures\\ant\\ant_food.png";
    textures[1].loadFromFile(file);
}

class AntKingdom {
    public:
        int size = 100000;

        std::vector<Grid> pheromones;
        std::vector<Ant> c;

        // part of drawing
        sf::VertexArray lines;



        AntKingdom(int size=30000) {
            this->size = size;

            Grid food, home;
            home.grid = GridType::HOME;

            this->c = std::vector<Ant>(size);
            this->lines = sf::VertexArray(sf::Points, size);
            pheromones = {food, home};
        }



        int iter = 0;

        void update(Map &m) {
            iter++;
            if (iter % 30 == 0) {
                pheromones[iter % 30].update();
            }
            if (iter % 30 == 1) {
                pheromones[iter % 30].update();
            }
            sf::Vector2i p2{};
            for (int i = 0; i < size; i++) {
                c[i].update(pheromones, p2, m);
            }
        }

        sf::VertexArray& drawAnts() {
            for (int i = 0; i < size; i++) {
                if (c[i].mode == GridType::HOME) {
                    lines[i].color = sf::Color::Red;
                } else {
                    lines[i].color = sf::Color::White;
                }
                lines[i].position = c[i].position;
            }
            return lines;
        }

    std::vector<sf::Sprite> drawAntsTexture() {
        std::vector<sf::Sprite> sprites(size/10);
        for (int i = 0; i < sprites.size(); i++) {
            sprites[i].setPosition(c[i].position);
            sprites[i].setRotation((std::atan2(c[i].velocity.y, c[i].velocity.x)/3.14)*180+90);
            if (c[i].mode == GridType::HOME) {
                sprites[i].setTexture(textures[1]);
            } else{
                sprites[i].setTexture(textures[0]);
            }
            sprites[i].setScale(sf::Vector2f(0.02f, 0.02f));
            //sprites[i].setColor(sf::Color(0, 255, 0));
        }
        return sprites;
    }
};



int main()
{
    srand (time(nullptr));
    // create the window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "AntSim", sf::Style::Default,
                            sf::ContextSettings(24, 8, 4, 3, 2));

    Map m{};
    std::vector<AntKingdom> kings = std::vector<AntKingdom>(1);
    loadTextures();

    window.setFramerateLimit(30);

    sf::Clock clock;
    float lastTime = 0;
    int iter = 0;
    int minIt = 0;
    float min = 60;

    sf::VertexArray background;
    bool pause = false;
    bool zoom = false;
    bool drawGrid = true;
    Map::EnvType drawSetting = Map::WALL;

    sf::View view(sf::FloatRect(0.f, 0.f, 720/3.f, 1080/3.f));
    // run the program as long as the window is open
//#pragma omp parallel default(shared)

    while (window.isOpen())
    {
        iter++;

        if (!pause) {
#pragma omp for
            for (int i = 0; i<kings.size(); i++) {
                kings[i].update(m);
            }
        }



#pragma omp master
        {

            float currentTime = clock.getElapsedTime().asSeconds();
            float fps = 1.f / (currentTime - lastTime);
            if (fps < min || minIt - iter < -120) {
                min = fps;
                minIt = iter;
            }
            if (iter % 200 == 0) {
                std::cout << fps << " (" << min << ") " << minIt << "\n";
            }

            lastTime = currentTime;

            // clear the window with black color
            window.clear(sf::Color::Black);

            if (drawGrid) {
                if (iter % 30 == 0) {
                    background = kings[0].pheromones[1].draw();
                }
                window.draw(background);
            }


            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                m.placeEnv(sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y, Map::RECOURCE);
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                m.placeEnv(sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y, drawSetting);
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
            {
                m.placeEnv(sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y, Map::NONE);
            }
            while (window.pollEvent(event)) {
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Key::P) {
                        pause = !pause;
                    }
                    if (event.key.code == sf::Keyboard::Key::Z) {
                        zoom = !zoom;
                        view.setCenter(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
                        if (zoom) {
                            window.setView(view);
                        } else {
                            window.setView(window.getDefaultView());
                        }
                    }
                    if (event.key.code == sf::Keyboard::Key::Right) {
                        if (drawSetting == Map::WALL) {
                            drawSetting = Map::WALL_DESTROY;
                        } else if (drawSetting == Map::WALL_DESTROY) {
                            drawSetting = Map::MUD;
                        } else if (drawSetting == Map::MUD) {
                            drawSetting = Map::RIVER;
                        } else if (drawSetting == Map::RIVER) {
                            drawSetting = Map::WALL;
                        }
                    }
                }
            }

            window.draw(m.gridMap);



            // draw everything here...
            if (!zoom) {
                for (int i = 0; i<kings.size(); i++) {
                    window.draw(kings[i].drawAnts());
                }
            } else {
                for (int i = 0; i<kings.size(); i++) {
                    std::vector<sf::Sprite> a = kings[i].drawAntsTexture();
                    for (int j = 0; j<a.size(); j++) {
                        window.draw(a[j]);
                    }
                }
            }


            // end the current frame
            window.display();
        }
#pragma omp barrier
    }

    return 0;
}
#endif //ANTS_MAIN_CPP
