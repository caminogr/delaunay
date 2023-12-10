#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <iostream>


struct Point {
  float x, y;

  Point(float x, float y) : x(x), y(y) {}
};
std::ostream& operator<<(std::ostream& os, const Point& point) {
  os << "(" << point.x << ", " << point.y << ")";
  return os;
}

struct Edge {
  Point start, end;

  Edge(const Point& startPoint, const Point& endPoint) : start(startPoint), end(endPoint) {}
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
};

struct Circle {
  Point center;
  float radius;

  Circle(const Point& center, float radius) : center(center), radius(radius) {}
};

float cross_product(const Point& A, const Point& B) {
  return A.x * B.y - A.y * B.x;
}

Triangle get_triangle_including_window() {
  Point center(0.0f, 0.0f);

  // TMP
  /* float window_diagonal = std::sqrt(std::pow(800/2, 2) + std::pow(600/2, 2)); */
  float window_diagonal = std::sqrt(std::pow(800, 2) + std::pow(800, 2));
  std::cout << "window_diagonal" << std::endl;
  std::cout << window_diagonal << std::endl;

  // windowに外接する円の半径める
  float circumscribed_circle_r = window_diagonal / 2;
     /* A */
    /* /○\ */
   /* B---C */ 
  // windowに外接する円外接する三角形を構成する頂点の座標
  Point A = Point(
      center.x,
      center.x + 2 * circumscribed_circle_r / (800 * 1/2)
    );
  Point B = Point(
      center.x - std::sqrt(3) * circumscribed_circle_r / (800 * 1/2),
      center.x - circumscribed_circle_r / (800 * 1/2)
    );
  Point C = Point(
      center.x + std::sqrt(3) * circumscribed_circle_r / (800 * 1/2),
      center.x - circumscribed_circle_r / (800 * 1/2)
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

  return ad <= circle.radius && bd <= circle.radius && cd <= circle.radius;
}



/* 追加する点を含んでいるの三角形を取得 */
Triangle get_triangle_include_point(
  const Point& point,
  const std::vector<Triangle>& triangles
) {
  // 後ろから走査していく
  int index = triangles.size() - 1;
  while (index >= 0) {
    /* for (const Triangle& triangle : triangles) { */
      if (is_point_in_triangle(point, triangles[index])) {
        return triangles[index];
      } else {
        // TODO: 選択した三角形の見つけ方の最適化
        std::cout << "is_point_in_triangle: false" << std::endl;
    }

    index--;
  }

  return triangles[0];
}


/* 全ての三角形をチェック: */
/* 分割中の全ての三角形に対して、その外接円が追加された点を含むかどうかをチェックします。 */

/* 外接円内の点の判定: */
/* 三角形の外接円の中心から追加された点までの距離を計算し、それが外接円の半径よりも小さいかどうかを確認します。距離が半径よりも小さい場合、その点は外接円内にあります。 */



/* Circle get_circumscribed_circle(const Point& center, radius) { */
/*   Circle circle = Circle(Point(0.0f, 0.0f), 0.0f); */
/*   return circle; */
/* } */

std::vector<int> get_tringles_inner_circumscribed_circle(const Triangle& triangle, const std::vector<Triangle>& triangles) {
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
  Circle circumscribed_circle = Circle({cx, cy}, r);
  
  std::vector<int> should_remove_list = {};
  for (int i = 0; i < triangles.size(); i++) {
    Triangle triangle = triangles[i];
    if (is_triangle_in_circle(triangle, circumscribed_circle)) {
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

Point get_opposite_point(const Edge& edge, const Triangle& triangle) {
  if (
    // Pointがedgeのstartでもendでもない
    triangle.a.x == edge.start.x && triangle.a.y == edge.start.y ||
    triangle.a.x == edge.end.x && triangle.a.y == edge.end.y
  ) {
    return triangle.a;
  } else if (
    triangle.b.x == edge.start.x && triangle.b.y == edge.start.y ||
    triangle.b.x == edge.end.x && triangle.b.y == edge.end.y
  ) {
    return triangle.b;
  } else {
    return triangle.c;
  }
}

void legalize_edge(
  const Triangle& target_triangle,
  /* const std::vector<Triangle>& triangles, */
  std::vector<Triangle>& primitive_triangles,
  const Point& added_point
) {

  // 違反点があったらflipした三角形を再作成
    // flip
    // 違反点を含んでいる三角形を取得
    // 違反している三角形を取得
    std::vector<int> required_flip_triangles = get_tringles_inner_circumscribed_circle(target_triangle, primitive_triangles);
    std::cout << "<= 1であることを期待したコード" << std::endl;
    std::cout << "required_flip_triangles.size(): " << required_flip_triangles.size() << std::endl;
    if (required_flip_triangles.size() <= 0) {
      return;
    }

    //TODO: target_triangleをprimitiveから削除


  // TODO: 「required_flip_triangles.size() <= 1が正しければfor分の記述を消す
    for (int i = required_flip_triangles.size() - 1; 0 <= i; i--) {
      Triangle required_flip_triangle = primitive_triangles[required_flip_triangles[i]];
      // delayney条件を違反するedgeを取得
      // added_point以外の2点が作るline
      Edge illegal_edge = get_opposite_edge(added_point, target_triangle);
      Point unshared_point = get_opposite_point(illegal_edge, required_flip_triangle);

      // target_triangleを削除
      // TODO: 違反辺を含んでいる三角形を削除
      /* primitive_triangles.erase(primitive_triangles.begin() + required_flip_triangles[i]); */
      // 新しく追加した点と違反点(違反している三角形の変を共有していない点)、共有するedgeがもつ点の3つからなる三角形を作成
      Triangle new_triangle1 = Triangle(added_point, unshared_point, illegal_edge.start);
      Triangle new_triangle2 = Triangle(added_point, unshared_point, illegal_edge.end);
      // new_triangle1をprimitiveに追加
      // new_triangle2をprimitiveに追加
      primitive_triangles.push_back(new_triangle1);
      primitive_triangles.push_back(new_triangle2);

      // flipした三角形の周りの三角形を再帰的にチェック
      // new triangle 1
      legalize_edge(new_triangle1, primitive_triangles, added_point);
      // new triangle 2
      legalize_edge(new_triangle2, primitive_triangles, added_point);
  }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 800, "Random Points", nullptr, nullptr);
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
    /* for (int i = 0; i < 2; ++i) { */
    /*   points.push_back({ */
    /*     static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1, */ 
    /*     static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1 */
    /*   }); */
    /* } */
    points.push_back({
      0.5f, 0.5f
    });
    points.push_back({
    .3f, .3f
    });

    glPointSize(8.0f);

    // windowを内包する三角形の頂点を取得
    Triangle outermost_triangle = get_triangle_including_window();

    // vectorの中身をすべて確認
    /* std::vector<Edge> edges; */
    std::vector<Triangle> triangles = {outermost_triangle};

    // pointsを順番に処理していく
    /* for (int i = 0; i < points.size(); ++i) { */
    int i = 0;

    {
      Point point = points.at(i);
      Point a = outermost_triangle.a;
      Point b = outermost_triangle.b;
      Point c = outermost_triangle.c;

      /* triangleからpointに向かうedgeを作成 */
      Edge edge1 = Edge(a, point);
      Edge edge2 = Edge(b, point);
      Edge edge3 = Edge(c, point);

      /* edges.push_back(edge1); */
      /* edges.push_back(edge2); */
      /* edges.push_back(edge3); */
      Triangle triangle1 = Triangle(edge1.start, edge1.end, edge2.start);
      Triangle triangle2 = Triangle(edge2.start, edge2.end, edge3.start);
      Triangle triangle3 = Triangle(edge3.start, edge3.end, edge1.start);
      triangles.push_back(triangle1);
      triangles.push_back(triangle2);
      triangles.push_back(triangle3);
    }

std::vector<Triangle> primitive_triangles = {};
    {
      i = 1;
      Point point = points.at(i);

      auto outer_triangle = get_triangle_include_point(point, triangles);
      Edge edge1 = Edge(outer_triangle.a, point);
      Edge edge2 = Edge(outer_triangle.b, point);
      Edge edge3 = Edge(outer_triangle.c, point);
      /* edges.push_back(edge1); */
      /* edges.push_back(edge2); */
      /* edges.push_back(edge3); */

      Triangle triangle1 = Triangle(edge1.start, edge1.end, edge2.start);
      Triangle triangle2 = Triangle(edge2.start, edge2.end, edge3.start);
      Triangle triangle3 = Triangle(edge3.start, edge3.end, edge1.start);

      /* 外接に含まれている三角形のcheckと削除 */
      std::vector<int> should_remove_list = get_tringles_inner_circumscribed_circle(outer_triangle, triangles);
      for (int i = should_remove_list.size() - 1; 0 <= i; i--) {
        triangles.erase(triangles.begin() + should_remove_list[i]);
      }

    std::vector<Triangle> new_triangles = {triangle1, triangle2, triangle3};
      /* triangles.push_back(triangle1); */
      /* triangles.push_back(triangle2); */
      /* triangles.push_back(triangle3); */
    /* std::vector<Edge> should_check_edges = {}; */

    for (int i = 0; i < new_triangles.size(); i++) {
      // new_trianglesの中で pointを含まないedgre →  check_edge
      legalize_edge(new_triangles[i], primitive_triangles, point);
      // edgeをｗたすver
    }

      
    }

    /* { */
    /*   i = 2; */
    /*   Point point = points.at(i); */

    /*   auto outer_triangle = get_triangle_include_point(point, triangles); */
    /*   Edge edge1 = Edge(point, outer_triangle.a); */
    /*   Edge edge2 = Edge(point, outer_triangle.b); */
    /*   Edge edge3 = Edge(point, outer_triangle.c); */
    /*   /1* edges.push_back(edge1); *1/ */
    /*   /1* edges.push_back(edge2); *1/ */
    /*   /1* edges.push_back(edge3); *1/ */

    /*   Triangle triangle1 = Triangle(edge1.start, edge1.end, edge2.end); */
    /*   Triangle triangle2 = Triangle(edge2.start, edge2.end, edge3.end); */
    /*   Triangle triangle3 = Triangle(edge3.start, edge3.end, edge1.end); */
    /*   triangles.push_back(triangle1); */
    /*   triangles.push_back(triangle2); */
    /*   triangles.push_back(triangle3); */

    /*   /1* 外接に含まれている三角形のcheckと削除 *1/ */
    /*   std::vector<int> should_remove_list = get_tringles_inner_circumscribed_circle(point, outer_triangle, triangles); */
    /*   for (int i = should_remove_list.size() - 1; 0 <= i; i--) { */
    /*     triangles.erase(triangles.begin() + should_remove_list[i]); */
    /*   } */
  /* } */


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw random points
        glBegin(GL_POINTS);
        for (const auto& point : points) {
          glVertex2f(point.x, point.y);
        }
        glEnd();

/*         glBegin(GL_LINES); */
/*           for (const Edge& edge : edges) { */
/*             glVertex2f(edge.start.x, edge.start.y); */
/*             glVertex2f(edge.end.x, edge.end.y); */
/*           } */
/*         glEnd(); */

      // circle
       /* glBegin(GL_LINE_LOOP); */
       /*   for (int i = 0; i <= 360; i++) { */
       /*       float degInRad = i * M_PI / 180; */
       /*       glVertex2f(circle.center.x + cos(degInRad) * circle.radius, circle.center.y + sin(degInRad) * circle.radius); *1/ */
       /*   } */
       /* glEnd(); */


        glBegin(GL_LINE_LOOP);
          for (const Triangle& triangle : triangles) {
            glVertex2f(triangle.a.x, triangle.a.y);
            glVertex2f(triangle.b.x, triangle.b.y);
            glVertex2f(triangle.c.x, triangle.c.y);
          }
        glEnd();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
