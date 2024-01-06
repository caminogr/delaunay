#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>

const double EPSILON = 1e-4;

struct Point {
  float x, y;

  Point(float x, float y) : x(x), y(y) {}

  bool operator<(const Point& other) const {
    if (x == other.x) {
        return y < other.y;
    }
    return x < other.x;
  }

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }

  friend std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "(" << point.x << ", " << point.y << ")";
    return os;
  }
};

struct Edge {
  Point start, end;

  Edge(const Point& startPoint, const Point& endPoint) : start(startPoint), end(endPoint) {}

  bool passThrough(const Point& point) const {
    float cross = (end.x - start.x) * (point.y - start.y) - (end.y - start.y) * (point.x - start.x);
    // crossが0でなければ辺上にはない
    if (std::fabs(cross) > EPSILON) {
        return false;
    }

    // 点と辺の端点間の距離のチェック
    float dStartToPoint = std::sqrt((start.x - point.x) * (start.x - point.x) + (start.y - point.y) * (start.y - point.y));
    float dEndToPoint = std::sqrt((end.x - point.x) * (end.x - point.x) + (end.y - point.y) * (end.y - point.y));
    float dStartToEnd = std::sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));

    return std::fabs(dStartToPoint + dEndToPoint - dStartToEnd) < EPSILON; // 点が辺上にあるかどうか
  }

  bool operator==(const Edge& other) const {
    return (start == other.start && end == other.end) || (start == other.end && end == other.start);
  }

  friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
    os << "start: " << edge.start << ", end: " << edge.end << "]";
    return os;
  }
};

struct Triangle {
  Point a, b, c;

  Triangle(const Point& pointA, const Point& pointB, const Point& pointC) : a(pointA), b(pointB), c(pointC) {}

  Edge getEdgeAB() const {
    return Edge(a, b);
  }

  Edge getEdgeBC() const {
   return Edge(b, c);
  }

  Edge getEdgeCA() const {
    return Edge(c, a);
  }

  bool hasPoint(const Point& point) const {
    return a == point || b == point || c == point;
  }

  bool operator==(const Triangle& other) const {
    return hasPoint(other.a) && hasPoint(other.b) && hasPoint(other.c);
  }

  friend std::ostream& operator<<(std::ostream& os, const Triangle& triangle) {
    os << "[" << triangle.a << ", " << triangle.b << ", " << triangle.c << "]";
    return os;
  }
};

struct Circle {
  Point center;
  float radius;

  Circle(const Point& center, float radius) : center(center), radius(radius) {}
};

#endif
