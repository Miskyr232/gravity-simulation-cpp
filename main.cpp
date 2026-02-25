#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

float scale = 10;

Vector2i cameraPos(100, 100);

int W = 1600;
int H = 1000;

RenderWindow window(VideoMode(W, H), "SFML Window");

float dist(Vector2f a, Vector2f b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Vector2f norm(Vector2f v) {
    float d = sqrt(v.x * v.x + v.y * v.y);
    if (d == 0) return Vector2f(0, 0);
    return v / d;
}

class MainObject {
public:
    Vector2f pos;
    Vector2f vel = Vector2f(0, 0);
    Vector2f acc = Vector2f(0, 0);
    float mass;
    float radius;
    float elasticity = 0.9;
    Color color;
    CircleShape circle;

    MainObject(Vector2f cpos, float cradius, float cmass, Color ccolor)
        : radius(cradius), circle(cradius), pos(cpos), mass(cmass), color(ccolor) {}
    void draw() { window.draw(circle); }
    void update() {
        vel += acc;
        pos += vel;
        if (pos.y >= H - radius) vel.y = -vel.y * elasticity;
        circle.setPosition(Vector2f(pos.x / scale + cameraPos.x, pos.y / scale + cameraPos.y));
    }
    void setup() { circle.setFillColor(color); }
};

class Object {
public:
    Vector2f pos;
    Vector2f vel = Vector2f(0, 0);
    Vector2f acc = Vector2f(0, 0);
    float mass;
    float radius;
    float elasticity = 0.9;
    Color color;
    CircleShape circle;
    MainObject *main;

    Object(Vector2f cpos, float cradius, float cmass, Color ccolor, MainObject *main)
        : radius(cradius), circle(cradius), pos(cpos), mass(cmass), color(ccolor), main(main) {
        Vector2f dir = main->pos - pos;
        float d = dist(main->pos, pos);
        dir = norm(dir);
        Vector2f tangential(-dir.y, dir.x);
        vel = tangential * (sqrt(d) / 10);
    }
    void draw() { window.draw(circle); }
    void update() {
        vel += acc;
        pos += vel;
        circle.setPosition(Vector2f(pos.x / scale + cameraPos.x, pos.y / scale + cameraPos.y));
    }
    void setup() { circle.setFillColor(color); }
};

class Program {
    MainObject main = MainObject(Vector2f(0, 0), 3, 1e15f, Color(254, 255, 27));
    vector<Object> objects;

public:

    void input(){
        Event event;
        while(window.pollEvent(event)){
            if(event.type==Event::Closed) window.close();
            if(event.type==Event::KeyPressed){
                if(event.key.code==Keyboard::A) cameraPos.x += 10;
                if(event.key.code==Keyboard::D) cameraPos.x -= 10;
                if(event.key.code==Keyboard::S) cameraPos.y -= 10;
                if(event.key.code==Keyboard::W) cameraPos.y += 10;
                if(event.key.code==Keyboard::Q and scale < 30) scale += 1;
                if(event.key.code==Keyboard::E and scale > 1) scale -= 1;
            }
        }
    }

    void run() {
        for (int i = 1; i < 10000; i++) {
            objects.push_back(Object(Vector2f(rand() % 5000 - 2500, rand() % 5000 - 2500), 1, 999, Color(255, 255, 255), &main));
        }
        for (auto &o : objects) o.setup();
        float deltaTime = 0.05f;

        while (window.isOpen()) {
            input();

            window.clear(Color::Black);

            float eps = 10;
            for (size_t i = 0; i < objects.size(); i++) {
                objects[i].acc = Vector2f(0, 0);
                float d = dist(objects[i].pos, main.pos);
                d = max(d, 5.0f);
                float F = (0.000000001f * (objects[i].mass * main.mass) / (d * d) + eps) * deltaTime;
                Vector2f v = main.pos - objects[i].pos;
                v = norm(v);
                objects[i].acc += v * (F / objects[i].mass);
            }

            for (auto &o : objects) { o.update(); o.draw(); }
            main.update(); main.draw();

            window.display();
        }
    }
};

int main() {
    srand(time(0));
    Program pr;
    pr.run();
    return 0;
}
