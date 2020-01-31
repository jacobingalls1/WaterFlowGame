#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <map>
using namespace std;

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace sf;


int msq(int in) {
    for (int i = 0; i < 100; i++) {
        if (pow(i, 2) >= in) return i;
    }
    return -1;
}

bool find(vector<int> v, int i) {
    for (int in : v) {
        if (in == i) return true;
    }
    return false;
}


const int SCREEN_BASE = 1000, MAX_POINTS = 9, TILE_BASE = msq(MAX_POINTS), TILE_SIDE = SCREEN_BASE / TILE_BASE,
        EDGES = rand() % MAX_POINTS * 3, SCREEN_SIZE = TILE_SIDE * TILE_BASE;
const float DISP_MAG = 1;
int totalFluid = 1 + MAX_POINTS - EDGES, mouseX = SCREEN_SIZE / 2, mouseY = SCREEN_SIZE / 2;
vector<Vector2f> nodes(MAX_POINTS);
map<int, vector<int>> edges;
vector<int> values(MAX_POINTS);
vector<int> clickShadow(TILE_BASE * TILE_BASE, 0);

Font arial;



int getSecD(int sec, float f, float targ) {
    int i;
    for (i = 0; i < sec && i * (f / sec) < targ; i++);
    return i - 1;
}

int getSec(Vector2f sect) {
    Vector2i ret(getSecD(TILE_BASE, SCREEN_SIZE, sect.x), getSecD(TILE_BASE, SCREEN_SIZE, sect.y));
    return (ret.y * TILE_BASE) + ret.x;
}

void flow(Vector2f sect, bool positive, int amnt=1) {
    int node = getSec(sect);
    for (int i : edges[node]) {
        if (positive) {
            values[i]++;
            values[node]--;
        } else {
            values[i]--;
            values[node]++;
        }
    }
}

void rectify(Vector2f & v) {
    int scph = SCREEN_SIZE - TILE_SIDE /
            4;
    if (v.x < 0) v.x = 0.001;
    if (v.x > scph) v.x = scph;
    if (v.y < 0) v.y = 0.001;
    if (v.y > scph) v.y = scph;
}

void drawShadow(RenderWindow& window) {
    for (int i = 0; i < clickShadow.size(); i++) {
        if (clickShadow[i] > 0) {
            int ph = clickShadow[i]--;
            RectangleShape rect;
            rect.setSize(Vector2f(TILE_SIDE, TILE_SIDE));
            rect.setFillColor(Color(ph * 5, ph * 5, ph * 5));
            rect.setPosition(TILE_SIDE * (i % 3), TILE_SIDE * (i / 3));
            window.draw(rect);
        }
    }
}

void win(RenderWindow& window) {
    Text num;
    num.setFont(arial);
    num.setString("You win!");
    num.setPosition(Vector2f(SCREEN_SIZE / 2 - 50, SCREEN_SIZE / 2 - 10));
    num.setFillColor(Color::Blue);
    num.setCharacterSize(TILE_SIDE / 4);
//    window.clear(Color::Green);
    for (int i = 0; i < 200; i++) {
        num.setPosition(rand() % (SCREEN_SIZE - 50), rand() % (SCREEN_SIZE - 10));
        window.draw(num);
        window.display();
        Event event;
        while (window.pollEvent(event)) {
            return;
        }
    }
}

int main() {

    if (!arial.loadFromFile("C:\\Users\\Ophiuchus\\Desktop\\C++\\WaterFlowGame\\data\\arial.ttf")) cerr << "ARIAL";

    srand(time(NULL));


    for (int i = 0; i < TILE_BASE; i++) {
        for (int j = 0; j < TILE_BASE; j++) {
            int ph = j + (TILE_BASE * i);
            if (ph < MAX_POINTS) {
                nodes[ph].x = (j * TILE_SIDE) + rand() % (int)(3 * TILE_SIDE / (float)4);
                nodes[ph].y = (i * TILE_SIDE) + rand() % (int)(3 * TILE_SIDE / (float)4);
            }
            rectify(nodes[ph]);
        }
    }

    for (int i = 0; i < EDGES; i++) {
        int rL(rand() % nodes.size()), rR(rand() % nodes.size());
        while (find(edges[rL], rR) || find(edges[rR], rL)) {
            rL = (rand() % nodes.size());
            rR = (rand() % nodes.size());
        }
        edges[rL].push_back(rR);
        edges[rR].push_back(rL);
    }

    vector<bool> ph(MAX_POINTS);

    for (int i = 0; i < 5; i++) {
        if (rand() % 2) {
            totalFluid--;
            values[rand() % MAX_POINTS]++;
        } else {
            totalFluid++;
            values[rand() % MAX_POINTS]--;
        }
    }

    while (totalFluid != 0) {
        if (rand() % 2) {
            totalFluid--;
            values[rand() % MAX_POINTS]++;
        } else {
            totalFluid++;
            values[rand() % MAX_POINTS]--;
        }
    }


    RenderWindow window(VideoMode(SCREEN_SIZE, SCREEN_SIZE), "flow");
    window.requestFocus();
    window.setMouseCursorVisible(true);
    window.setFramerateLimit(60);



    while( window.isOpen() ) {
        Vector2i mouse = Mouse::getPosition(window);
        if (0 < mouse.x and mouse.x < SCREEN_SIZE and 0 < mouse.y and mouse.y < SCREEN_SIZE) {
            mouseX = mouse.x;
            mouseY = mouse.y;
        } else {
            mouseX = SCREEN_SIZE / 2;
            mouseY = SCREEN_SIZE / 2;
        }

        window.clear(Color::Black);

        drawShadow(window);

        for (int i = 0; i < nodes.size(); i++) {

            Vector2f posOff(TILE_SIDE / 8, TILE_SIDE / 8);

            for (int tar : edges[i]) {
                VertexArray temp(LinesStrip, 2);
                temp[0].position = nodes[tar] + posOff;
                temp[0].color = Color::Red;
                temp[1].position = nodes[i] + posOff;
                temp[1].color = Color::Red;
                window.draw(temp);
            }

            Vector2f v = nodes[i];
            CircleShape circ;
//            RectangleShape circ;
            circ.setRadius(TILE_SIDE / 8);
//            circ.setSize(Vector2f(TILE_SIDE / 8, TILE_SIDE / 8));
            circ.setPosition(v);
            circ.setFillColor(Color::Blue);
            window.draw(circ);
            Text num;
            num.setFont(arial);
            num.setString(to_string(values[i]));
//            num.setString(to_string(i));
            num.setPosition(Vector2f(v.x + TILE_SIDE / (float)12, v.y + TILE_SIDE / (float)20));
            num.setFillColor(Color::White);
            num.setCharacterSize(TILE_SIDE / 8);
            window.draw(num);

        }




//            RectangleShape rect;
//            rect.setPosition(points[i].x, points[i].y);
//            rect.setSize(Vector2f(1, 1));
//            rect.setFillColor(Color::White);
//            window.draw(rect);

        window.display();
        bool ph = true;
        for (int i : values) if (i < 0) ph = false;
        if (ph) {
            win(window);
            window.close();
            main();
        }

        bool shift = false, ctrl = false;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            } else if (event.type == Event::KeyPressed) {
                switch (event.key.code) {
                    case Keyboard::Escape:
                        window.close();
                        break;
                    case Keyboard::Q:
                        window.close();
                        main();
                        break;
                    case Keyboard::Space:
                        vector<Vector2f> ph;
                        for (int i = 0; i < nodes.size(); i++) {
                            Vector2f disp(0, 0);
                            for (int j = 0; j < nodes.size(); j++) {
                                if (i != j) {
                                    disp += nodes[j] - nodes[i];
                                }
                            }
                            ph.push_back(nodes[i] - Vector2f(disp.x / 500, disp.y / 500));
//                            ph.push_back(nodes[i] - Vector2f((500 - nodes[i].x) / 50, (500 - nodes[i].y) / 50));
                            rectify(ph[i]);
                        }
                        nodes = ph;
                        break;
//                    case Keyboard::LShift:
//                        shift = true;
//                        break;
//                    case Keyboard::LControl:
//                        ctrl = true;
//                        break;
                }
            } else if (event.type == Event::KeyReleased) {
                switch (event.key.code) {
                    case Keyboard::LShift:
                        shift = false;
                        break;
                    case Keyboard::LControl:
                        ctrl = false;
                        break;
                }
            }else if (event.type == Event::MouseButtonPressed) {
                int amnt = 1;
                if (ctrl && shift) amnt = 15;
                else if (shift) amnt = 10;
                else if (ctrl) amnt = 5;
                flow(Vector2f(Mouse::getPosition(window)), event.mouseButton.button == Mouse::Left, amnt);
                clickShadow[getSec(Vector2f(Mouse::getPosition(window)))] = 20;
            }
        }
    }
    return EXIT_SUCCESS;
}