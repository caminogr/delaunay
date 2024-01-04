#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <iostream>
#include <chrono>

#include "SimplexNoise.h"

const double EPSILON = 1e-5;
const int POINTS_NUM = 10;

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
  Edge edges[] = {
    triangle.getEdgeAB(),
    triangle.getEdgeBC(),
    triangle.getEdgeCA(),
  };
  for (const Edge& edge : edges) {
    if (edge.passThrough(point)) {
      return true;
    }
  }

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

/* 特定の点を含んでいる辺を取得 */
Edge get_edge_include_point(
  const Point& point,
  const Triangle& triangle
) {
  Edge edges[] = {
    triangle.getEdgeAB(),
    triangle.getEdgeBC(),
    triangle.getEdgeCA(),
  };

  for (const Edge& edge : edges) {
    if (edge.passThrough(point)) {
      return edge;
    }
  }
  throw std::runtime_error("not found edge including point");
}

/* 特定の点を含んでいるの三角形を取得 */
std::vector<Triangle> get_triangle_include_point(
  const Point& point,
  const std::vector<Triangle>& triangles
) {
  std::vector<Triangle> triangles_include_point = {};
  for (const Triangle& triangle : triangles) {
    if (is_point_in_triangle(point, triangle)) {
      triangles_include_point.push_back(triangle);
    }
  }
  if (triangles_include_point.size() == 0) {
    throw std::runtime_error("not found triangle including point");
  }
  return triangles_include_point;
}


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
          }
      }
  }

  return false;
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
  std::vector<Triangle>& primitive_triangles,
  const Point& added_point
) {

  Triangle target_triangle = Triangle(added_point, checked_edge.start, checked_edge.end);
  std::vector<int> required_flip_adjucents = get_adjcent_tringles_inner_circumscribed_circle(target_triangle, primitive_triangles);

  if (required_flip_adjucents.size() == 0) {
    return;
  }
  if (required_flip_adjucents.size() > 1) {
    /* throw std::runtime_error("flip triangle is not unique"); */
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

  Triangle new_triangle1 = primitive_triangles[adjcents_indexs[0]];
  Triangle new_triangle2 = primitive_triangles[adjcents_indexs[1]];

  Triangle base_triangle = new_triangle1.hasPoint(added_point) ? new_triangle1 : new_triangle2;
  Triangle another_triangle = !new_triangle1.hasPoint(added_point) ? new_triangle1 : new_triangle2;

  Point unshared_point = get_opposite_point(checked_edge, another_triangle);

  new_triangle1 = Triangle(added_point, unshared_point, checked_edge.start);
  new_triangle2 = Triangle(added_point, unshared_point, checked_edge.end);

  primitive_triangles[adjcents_indexs[0]] = new_triangle1;
  primitive_triangles[adjcents_indexs[1]] = new_triangle2;

  Edge checked_edge1 = get_opposite_edge(added_point, new_triangle1);
  Edge checked_edge2 = get_opposite_edge(added_point, new_triangle2);

  legalize_edge(checked_edge1, primitive_triangles, added_point);
  legalize_edge(checked_edge2, primitive_triangles, added_point);
}

Point wrap_position(Point& point) {
  float xv = point.x;
  float yv = point.y;
  if (point.x < -1 || 1 < point.x) {
    xv = abs(point.x - (int)point.x) * 2.0f - 1.0f;;
  }
  if (point.y < -1 || 1 < point.y) {
    yv = abs(point.y - (int)point.y) * 2.0f - 1.0f;
  }
  point.x = xv;
  point.y = yv;
  return point;
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
  std::vector<Point> ori_points;

  /* for (int i = 0; i < POINTS_NUM; ++i) { */
  /*   float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1; */
  /*   float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1; */
  /*   std::cout << "{x, y}: " << "{" << x << ", " << y << "}" << std::endl; */
  /*   ori_points.push_back({ x, y }); */
  /* } */

/*   {x, y}: {0.685948, -0.247942} */
/* {x, y}: {-0.509265, -0.790713} */
/* {x, y}: {-0.603897, -0.0185628} */
/* {x, y}: {-0.0755093, -0.0568302} */
/* {x, y}: {0.478163, 0.813251} */
/* {x, y}: {-0.845291, 0.151035} */
/* {x, y}: {0.539462, 0.0694113} */
/* {x, y}: {0.381242, 0.502405} */
/* {x, y}: {-0.075986, 0.0878346} */
/* {x, y}: {0.855167, -0.490298} */

  ori_points.push_back({ 0.685948, -0.247942 });
  ori_points.push_back({ -0.509265, -0.790713 });
  ori_points.push_back({ -0.603897, -0.0185628 });
  ori_points.push_back({ -0.0755093, -0.0568302 });
  ori_points.push_back({ 0.478163, 0.813251 });

  ori_points.push_back({ -0.845291, 0.151035 });
  debugPoints.push_back({ -0.845291, 0.151035 });

  /* ori_points.push_back({ 0.539462, 0.0694113 }); */
  /* ori_points.push_back({ 0.381242, 0.502405 }); */
  /* ori_points.push_back({ -0.075986, 0.0878346 }); */
  /* ori_points.push_back({ 0.855167, -0.490298 }); */


  glPointSize(8.0f);
  Triangle outermost_triangle = get_triangle_including_window();

  while (!glfwWindowShouldClose(window)) {
    std::vector<Point> points;
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

    std::cout << "=========" << std::endl;
    for (int i = 0; i < ori_points.size(); ++i) {
      Point point = ori_points.at(i);
      /* float noise_x = SimplexNoise::noise(i + 1, elapsedTime * 0.0001); */
      /* float noise_y = SimplexNoise::noise(point.x * point.y * (i + 1), elapsedTime * 0.0001); */
      /* /1* point = Point(point.x + elapsedTime * 0.001, point.y + elapsedTime * 0.001); *1/ */
      /* point = Point(point.x + noise_x * 0.2, point.y + noise_y * 0.2); */
      /* point = wrap_position(point); */

      std::cout << "{x, y}: " << "{" << point.x << ", " << point.y << "}" << std::endl;

      points.push_back(point);
    }

    std::vector<Triangle> primitive_triangles = {outermost_triangle};

    for (int i = 0; i < ori_points.size(); ++i) {
      auto outer_triangles = get_triangle_include_point(points.at(i), primitive_triangles);
      std::cout << "outer_triangles.size(): " << outer_triangles.size() << std::endl;

      std::vector<Triangle> new_triangles = {};
      // 追加された点が既存の三角形の辺上にある場合
      if (outer_triangles.size() == 2) {
        for (const Triangle& outer_triangle : outer_triangles) {
          Edge bounding_edge = get_edge_include_point(points.at(i), outer_triangle);
          Point opposite_point = get_opposite_point(bounding_edge, outer_triangle);

          debugPoints.push_back(opposite_point);

          Edge new_edge = Edge(opposite_point, points.at(i));
          Triangle triangle1 = Triangle(new_edge.start, new_edge.end, bounding_edge.start);
          Triangle triangle2 = Triangle(new_edge.start, new_edge.end, bounding_edge.end);
          new_triangles.push_back(triangle1);
          new_triangles.push_back(triangle2);
        }
      } else {
        // 追加された点が既存の三角形の辺上にない場合
        auto outer_triangle = outer_triangles.at(0);
        Edge edge1 = Edge(outer_triangle.a, points.at(i));
        Edge edge2 = Edge(outer_triangle.b, points.at(i));
        Edge edge3 = Edge(outer_triangle.c, points.at(i));

        Triangle triangle1 = Triangle(edge1.start, edge1.end, edge2.start);
        Triangle triangle2 = Triangle(edge2.start, edge2.end, edge3.start);
        Triangle triangle3 = Triangle(edge3.start, edge3.end, edge1.start);

        new_triangles.push_back(triangle1);
        new_triangles.push_back(triangle2);
        new_triangles.push_back(triangle3);
      }
      

      for (const Triangle& outer_triangle : outer_triangles) {
        // 分割された三角形はprimitive_trianglesから除去
        int divided_triangle_index = -1;
        for (int j = 0; j < primitive_triangles.size(); j++) {
          if (primitive_triangles[j] == outer_triangle) {
            divided_triangle_index = j;
            break;
          }
        }
        if (divided_triangle_index == -1) {
          throw std::runtime_error("not found divided triangle");
        }
        primitive_triangles.erase(primitive_triangles.begin() + divided_triangle_index);
      }

      for (const Triangle& triangle : new_triangles) {
        primitive_triangles.push_back(triangle);
      }


      for (const Triangle& triangle : new_triangles) {
        Edge checked_edge = get_opposite_edge(points.at(i), triangle);
        legalize_edge(checked_edge, primitive_triangles, points.at(i));
      }
    }

  std::vector<Triangle> display_triangles = {};
  for (const Triangle& primitive_triangle : primitive_triangles) {
    display_triangles.push_back(primitive_triangle);
  }

  for (const Triangle& primitive_triangle : primitive_triangles) {
    if (
      !(primitive_triangle.hasPoint(outermost_triangle.a) || 
        primitive_triangle.hasPoint(outermost_triangle.b) ||
        primitive_triangle.hasPoint(outermost_triangle.c))
    ) {
      /* display_triangles.push_back(primitive_triangle); */
      debugCircles.push_back(get_circumscribed_circle(primitive_triangle));
    }
  }

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

    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    for (const auto& point : debugPoints) {
      glVertex2f(point.x, point.y);
    }
    glColor3f(1.,1.,1.);
    glEnd();

    glBegin(GL_TRIANGLES);
    /* glColor3f(0.5,0,0); */
    for (const auto& triangle : debugTriangles) {
  //radnom color
      glColor3f((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
  
      glVertex2f(triangle.a.x, triangle.a.y);
      glVertex2f(triangle.b.x, triangle.b.y);
      glVertex2f(triangle.c.x, triangle.c.y);
    }
    glColor3f(1.,1.,1.);
    glEnd();


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
