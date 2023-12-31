#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <iostream>
#include <chrono>

#include "SimplexNoise.h"

const double EPSILON = 1e-6;

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
};

std::ostream& operator<<(std::ostream& os, const Point& point) {
  os << "(" << point.x << ", " << point.y << ")";
  return os;
}

struct Edge {
  Point start, end;

  Edge(const Point& startPoint, const Point& endPoint) : start(startPoint), end(endPoint) {}

  bool operator==(const Edge& other) const {
    return (start == other.start && end == other.end) || (start == other.end && end == other.start);
  }
};
std::ostream& operator<<(std::ostream& os, const Edge& edge) {
  os << "start: " << edge.start << ", end: " << edge.end << "]";
  return os;
}

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

};
std::ostream& operator<<(std::ostream& os, const Triangle& triangle) {
  os << "[" << triangle.a << ", " << triangle.b << ", " << triangle.c << "]";
  return os;
}

struct Circle {
  Point center;
  float radius;

  Circle(const Point& center, float radius) : center(center), radius(radius) {}
};

std::vector<Point> debugPoints = {};
std::vector<Edge> debugEdges = {};
std::vector<Triangle> debugTriangles = {};
std::vector<Circle> debugCircles = {};

float cross_product(const Point& A, const Point& B) {
  return A.x * B.y - A.y * B.x;
}

const float WINDOW_WIDTH = 800;
const float WINDOW_HEIGHT = 800;

Triangle get_triangle_including_window() {
  Point center(0.0f, 0.0f);

  // TMP
  /* float window_diagonal = std::sqrt(std::pow(800/2, 2) + std::pow(600/2, 2)); */
  float window_diagonal = std::sqrt(std::pow(WINDOW_WIDTH, 2) + std::pow(WINDOW_HEIGHT, 2));

  // windowに外接する円の半径める
  float circumscribed_circle_r = window_diagonal / 2;
     /* A */
    /* /○\ */
   /* B---C */ 
  // windowに外接する円外接する三角形を構成する頂点の座標
  Point A = Point(
      center.x,
      center.x + 2 * circumscribed_circle_r / (WINDOW_HEIGHT * 1/2)
    );
  Point B = Point(
      center.x - std::sqrt(3) * circumscribed_circle_r / (WINDOW_WIDTH * 1/2),
      center.x - circumscribed_circle_r / (WINDOW_HEIGHT * 1/2)
    );
  Point C = Point(
      center.x + std::sqrt(3) * circumscribed_circle_r / (WINDOW_WIDTH * 1/2),
      center.x - circumscribed_circle_r / (WINDOW_HEIGHT * 1/2)
    );
    
  Triangle outermost_triangle = Triangle(A, B, C);
  return outermost_triangle;
}

bool is_same_sign(float a, float b, float c) {
  return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0);
}

bool is_point_in_triangle(
  const Point& point,
  const Triangle& triangle
) {
  float cross1 = cross_product(
      {triangle.a.x - triangle.b.x, triangle.a.y - triangle.b.y},
      {point.x - triangle.b.x, point.y - triangle.b.y}
    );
  float cross2 = cross_product(
      {triangle.c.x - triangle.a.x, triangle.c.y - triangle.a.y},
      {point.x - triangle.a.x, point.y - triangle.a.y}
    );
  float cross3 = cross_product(
      {triangle.b.x - triangle.c.x, triangle.b.y - triangle.c.y},
      {point.x - triangle.c.x, point.y - triangle.c.y}
  );
  return is_same_sign(cross1, cross2, cross3);
}

bool is_triangle_in_circle(
  const Triangle& triangle,
  const Circle& circle
) {
  // 各Pointに対して円の中心からの距離を計算し、それが円の半径よりも小さいかどうかを確認
  float ad = std::sqrt( std::pow(triangle.a.x - circle.center.x, 2) +  std::pow(triangle.a.y - circle.center.y, 2));
  float bd = std::sqrt( std::pow(triangle.b.x - circle.center.x, 2) +  std::pow(triangle.b.y - circle.center.y, 2));
  float cd = std::sqrt( std::pow(triangle.c.x - circle.center.x, 2) +  std::pow(triangle.c.y - circle.center.y, 2));

  return ad <= (circle.radius + EPSILON) && bd <= (circle.radius + EPSILON) && cd <= (circle.radius + EPSILON);
}



/* 追加する点を含んでいるの三角形を取得 */
Triangle get_triangle_include_point(
  const Point& point,
  const std::vector<Triangle>& triangles
) {
  for (int i = 0; i < triangles.size(); ++i) {
    if (is_point_in_triangle(point, triangles[i])) {
      return triangles[i];
    }
  }
  throw std::runtime_error("not found triangle including point");
}


/* 全ての三角形をチェック: */
/* 分割中の全ての三角形に対して、その外接円が追加された点を含むかどうかをチェックします。 */

/* 外接円内の点の判定: */
/* 三角形の外接円の中心から追加された点までの距離を計算し、それが外接円の半径よりも小さいかどうかを確認します。距離が半径よりも小さい場合、その点は外接円内にあります。 */



/* Circle get_circumscribed_circle(const Point& center, radius) { */
/*   Circle circle = Circle(Point(0.0f, 0.0f), 0.0f); */
/*   return circle; */
/* } */

bool are_triangles_adjacent(const Triangle& triangleA, const Triangle& triangleB) {
  if (triangleA == triangleB) {
    return false;
  };

  Edge edgesA[] = {
    triangleA.getEdgeAB(),
    triangleA.getEdgeBC(),
    triangleA.getEdgeCA(),
  };

  Edge edgesB[] = {
    triangleB.getEdgeAB(),
    triangleB.getEdgeBC(),
    triangleB.getEdgeCA(),
  };

  int count = 0;
  for (const Edge& edgeA : edgesA) {
      for (const Edge& edgeB : edgesB) {
          if (edgeA == edgeB) {
            return true;
            /* count++; */
          }
      }
  }

  /* if (count == 1) { */
  /*   return true; */
  /* } */
  return false; // 共有する辺はない
}

Circle get_circumscribed_circle(const Triangle& triangle) {
  float x1 = triangle.a.x;
  float y1 = triangle.a.y;
  float x2 = triangle.b.x;
  float y2 = triangle.b.y;
  float x3 = triangle.c.x;
  float y3 = triangle.c.y;

  float x1_2 = x1 * x1;
  float y1_2 = y1 * y1;
  float x2_2 = x2 * x2;
  float y2_2 = y2 * y2;
  float x3_2 = x3 * x3;
  float y3_2 = y3 * y3;

  float c = 2 * ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1));
  float cx = ((y3 - y1) * (x2_2 - x1_2 + y2_2 - y1_2) + (y1 - y2) * (x3_2 - x1_2 + y3_2 - y1_2))/c;
  float cy = ((x1 - x3) * (x2_2 - x1_2 + y2_2 - y1_2) + (x2 - x1) * (x3_2 - x1_2 + y3_2 - y1_2))/c;

  float r = std::sqrt(pow(x1 - cx, 2) + pow(y1 - cy, 2));
  return Circle({cx, cy}, r);
}

std::vector<int> get_adjcent_tringles_inner_circumscribed_circle(const Triangle& triangle, const std::vector<Triangle>& triangles) {
  Circle circumscribed_circle = get_circumscribed_circle(triangle);
  
  std::vector<int> should_remove_list = {};
  for (int i = 0; i < triangles.size(); i++) {
    auto a = is_triangle_in_circle(triangles[i], circumscribed_circle);
    auto b = are_triangles_adjacent(triangle, triangles[i]);

    if (
      is_triangle_in_circle(triangles[i], circumscribed_circle) &&
      are_triangles_adjacent(triangle, triangles[i])
    ) {
      should_remove_list.push_back(i);
    }
  }
  return should_remove_list;
}

Edge get_opposite_edge(const Point& point, const Triangle& triangle) {
  if (triangle.a.x == point.x && triangle.a.y == point.y) {
    return triangle.getEdgeBC();
  } else if (triangle.b.x == point.x && triangle.b.y == point.y) {
    return triangle.getEdgeCA();
  } else {
    return triangle.getEdgeAB();
  }
}


bool has_edge(const Triangle& triangle, const Edge& edge) {
  Edge edges[] = {
    triangle.getEdgeAB(),
    triangle.getEdgeBC(),
    triangle.getEdgeCA(),
  };

  for (const Edge& e : edges) {
    if (e == edge) {
      return true;
    }
  }

  return false;
}

Point get_opposite_point(const Edge& edge, const Triangle& triangle) {
  if ((triangle.a.x != edge.start.x || triangle.a.y != edge.start.y) &&
      (triangle.a.x != edge.end.x || triangle.a.y != edge.end.y)) {
    return triangle.a;
  } else if ((triangle.b.x != edge.start.x || triangle.b.y != edge.start.y) &&
             (triangle.b.x != edge.end.x || triangle.b.y != edge.end.y)) {
    return triangle.b;
  } else {
    return triangle.c;
  }
}

void legalize_edge(
  const Edge& checked_edge,
  /* const std::vector<Triangle>& triangles, */
  std::vector<Triangle>& primitive_triangles,
  const Point& added_point
) {

  Triangle target_triangle = Triangle(added_point, checked_edge.start, checked_edge.end);
  std::vector<int> required_flip_adjucents = get_adjcent_tringles_inner_circumscribed_circle(target_triangle, primitive_triangles);


  if (required_flip_adjucents.size() == 0) {
    return;
  }
  if (required_flip_adjucents.size() > 1) {
    throw std::runtime_error("flip triangle is not unique");
  }

  std::vector<int> adjcents_indexs = {};

  for (int i = 0; i < primitive_triangles.size(); ++i) {
    if (has_edge(primitive_triangles[i], checked_edge)) {
      adjcents_indexs.push_back(i);
    }
  }

  if (adjcents_indexs.size() < 2) {
    return;
  }

  /// added_pointを含んでいる方は最初のinitialiのやつでいい
  Triangle new_triangle1 = primitive_triangles[adjcents_indexs[0]];
  Triangle new_triangle2 = primitive_triangles[adjcents_indexs[1]];
  /* added_pointを含んでいる方を取得 */

  Triangle base_triangle = new_triangle1.hasPoint(added_point) ? new_triangle1 : new_triangle2;
  Triangle another_triangle = !new_triangle1.hasPoint(added_point) ? new_triangle1 : new_triangle2;

  //  check illigal
  Point unshared_point = get_opposite_point(checked_edge, another_triangle);
  //  flip
  // 
  new_triangle1 = Triangle(added_point, unshared_point, checked_edge.start);
  new_triangle2 = Triangle(added_point, unshared_point, checked_edge.end);
  // adjcents_indexs

  primitive_triangles[adjcents_indexs[0]] = new_triangle1;
  primitive_triangles[adjcents_indexs[1]] = new_triangle2;

  // 違反点があったらflipした三角形を再作成
    // flip
    // 違反点を含んでいる三角形を取得
    // 違反している三角形を取得
  Edge checked_edge1 = get_opposite_edge(added_point, new_triangle1);
  Edge checked_edge2 = get_opposite_edge(added_point, new_triangle2);

  legalize_edge(checked_edge1, primitive_triangles, added_point);
  legalize_edge(checked_edge2, primitive_triangles, added_point);
}

int main() {
    auto startTime = std::chrono::high_resolution_clock::now();
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Delaunay", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

    // Set viewport
    int width, height;
    // retina display 対応
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Seed for random number generation
    srand(static_cast<unsigned int>(time(nullptr)));
    std::vector<Point> points;

    for (int i = 0; i < 50; ++i) {
      float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1;
      float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1;
      std::cout << "{x, y}: " << "{" << x << ", " << y << "}" << std::endl;
      points.push_back({ x, y });
    }


    glPointSize(8.0f);
    // windowを内包する三角形の頂点を取得
    Triangle outermost_triangle = get_triangle_including_window();
    std::vector<Triangle> primitive_triangles = {outermost_triangle};
    /* std::vector<Circle> circumscribed_circle = {}; */

  for (int i = 0; i < points.size(); ++i) {
      Point point = points.at(i);

      auto outer_triangle = get_triangle_include_point(point, primitive_triangles);

      Edge edge1 = Edge(outer_triangle.a, point);
      Edge edge2 = Edge(outer_triangle.b, point);
      Edge edge3 = Edge(outer_triangle.c, point);

      Triangle triangle1 = Triangle(edge1.start, edge1.end, edge2.start);
      Triangle triangle2 = Triangle(edge2.start, edge2.end, edge3.start);
      Triangle triangle3 = Triangle(edge3.start, edge3.end, edge1.start);

      std::vector<Triangle> new_triangles = {triangle1, triangle2, triangle3};

      // 分割された三角形はprimitive_trianglesから除去
      int divided_triangle_index;
      for (int j = 0; j < primitive_triangles.size(); j++) {
        if (primitive_triangles[j] == outer_triangle) {
          divided_triangle_index = j;
          break;
        }
        divided_triangle_index = -1;
      }

      primitive_triangles.erase(primitive_triangles.begin() + divided_triangle_index);

      primitive_triangles.push_back(triangle1);
      primitive_triangles.push_back(triangle2);
      primitive_triangles.push_back(triangle3);

      for (int j = 0; j < new_triangles.size(); j++) {
        Edge checked_edge = get_opposite_edge(points[i], new_triangles[j]);
        legalize_edge(checked_edge, primitive_triangles, points[i]);
      }
    }

  std::vector<Triangle> display_triangles = {};
  for (int i = 0; i <= primitive_triangles.size(); ++i) {
    std::cout << "i: " << i << std::endl;
    // TODO: 2つの頂点が同じtriangleがある
    std::cout << primitive_triangles[i] << std::endl;
  }

  for (int i = 0; i <= primitive_triangles.size(); ++i) {
    if (
      !(primitive_triangles[i].hasPoint(outermost_triangle.a) || 
        primitive_triangles[i].hasPoint(outermost_triangle.b) ||
        primitive_triangles[i].hasPoint(outermost_triangle.c))
    ) {
      display_triangles.push_back(primitive_triangles[i]);
      debugCircles.push_back(get_circumscribed_circle(primitive_triangles[i]));
    }
  }

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        float noise = SimplexNoise::noise(sin(elapsedTime));

    
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POINTS);
        for (const auto& point : points) {
          glVertex2f(point.x, point.y);
        }
        glEnd();


        // Draw edges
/*         glBegin(GL_LINES); */
/*           for (const Edge& edge : edges) { */
/*             glVertex2f(edge.start.x, edge.start.y); */
/*             glVertex2f(edge.end.x, edge.end.y); */
/*           } */
/*         glEnd(); */


        /* TODO: drawの最適化 */
        for (const Triangle& triangle : display_triangles) {
          glBegin(GL_LINE_LOOP);
            glVertex2f(triangle.a.x, triangle.a.y);
            glVertex2f(triangle.b.x, triangle.b.y);
            glVertex2f(triangle.c.x, triangle.c.y);
          glEnd();
        }

        /* glBegin(GL_POINTS); */
        /* glColor3f(0.0, 0.0, 0.5); */
        /* for (const auto& point : debugPoints) { */
        /*   glVertex2f(point.x, point.y); */
        /* } */
        /* glColor3f(1.,1.,1.); */
        /* glEnd(); */

        /* glBegin(GL_TRIANGLES); */
        /* /1* glColor3f(0.5,0,0); *1/ */
        /* /1* for (const auto& triangle : debugTriangles) { *1/ */
        /* for (const auto& triangle : display_triangles) { */
      /* //radnom color */
        /*   glColor3f((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX); */
      
        /*   glVertex2f(triangle.a.x, triangle.a.y); */
        /*   glVertex2f(triangle.b.x, triangle.b.y); */
        /*   glVertex2f(triangle.c.x, triangle.c.y); */
        /* } */
        /* glColor3f(1.,1.,1.); */
        /* glEnd(); */


/*         glBegin(GL_LINES); */
/*           glColor3f(0.0,0.5,0); */
/*           for (const Edge& edge : debugEdges) { */
/*             glVertex2f(edge.start.x, edge.start.y); */
/*             glVertex2f(edge.end.x, edge.end.y); */
/*           } */
/*         glColor3f(1.,1.,1.); */
/*         glEnd(); */

        /* glBegin(GL_LINE_LOOP); */
        /*   for (const Circle& circle : debugCircles) { */
        /*     for (int i = 0; i <= 360; i++) { */
        /*       float degInRad = i * M_PI / 180; */
        /*       glVertex2f(circle.center.x + cos(degInRad) * circle.radius, circle.center.y + sin(degInRad) * circle.radius); */
        /*     } */
        /*   } */
        /* glEnd(); */


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
