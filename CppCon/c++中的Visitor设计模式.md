# The Visitor Design Pattern in Cpp

> 链接：[youtube](https://www.youtube.com/watch?v=PEcy1vYHb8A)

```c++
#include <bits/stdc++.h>

struct Circle {
    Circle(float r) : radius_(r) {}

    float getRadius() const { return radius_; }
    float radius_;
};

struct Square {
    Square(float s) : slide_(s) {}
    float getSlide() const { return slide_; }
    
    float slide_;
};

class Draw {
public:
    void operator()(const Circle & c) {
        std::cout << "draw circle, which radius is " << c.getRadius() << '\n';
    }

    void operator()(const Square & s) {
        std::cout << "draw square, which slide is " << s.getSlide() << '\n';
    }
};

using Shape = std::variant<Circle, Square>;

void drawAllShapes(const std::vector<Shape> &shapes) {
    for (const auto &s : shapes) {
        std::visit(Draw{}, s);
    }
}

int main() {

    std::vector<Shape> shapes;
    shapes.emplace_back(Circle(1.1));
    shapes.emplace_back(Square(2));
    shapes.emplace_back(Circle(2.1));

    drawAllShapes(shapes);
    return 0;
}

```

+ 解除依赖
+ 分离 operations 和 types