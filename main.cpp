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



Triangle get_triangle_including_window() {
  Point center(0.0f, 0.0f);

  // TMP
  /* float window_diagonal = std::sqrt(std::pow(800/2, 2) + std::pow(600/2, 2)); */
  float window_diagonal = std::sqrt(std::pow(800, 2) + std::pow(600, 2));
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
      center.x + 2 * circumscribed_circle_r / (600 * 1/2)
    );
  Point B = Point(
      center.x - std::sqrt(3) * circumscribed_circle_r / (800 * 1/2),
      center.x - circumscribed_circle_r / (600 * 1/2)
    );
  Point C = Point(
      center.x + std::sqrt(3) * circumscribed_circle_r / (800 * 1/2),
      center.x - circumscribed_circle_r / (600 * 1/2)
    );
    
  Triangle outermost_triangle = Triangle(A, B, C);
  return outermost_triangle;
}

Triangle get_triangle_include_point(const Point& point) {

}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Random Points", nullptr, nullptr);
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
    for (int i = 0; i < 2; ++i) {
      points.push_back({
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1, 
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1
      });
    }

    glPointSize(8.0f);

    // windowを内包する三角形の頂点を取得
    Triangle outermost_triangle = get_triangle_including_window();

    // vectorの中身をすべて確認
    std::vector<Edge> edges;

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

      edges.push_back(edge1);
      edges.push_back(edge2);
      edges.push_back(edge3);
    }

    i = 1;
    {
      Point point = points.at(i);

      /* 追加する点を含んでいるの三角形を取得 */
      /* Triangle outer_triangle = triangle_including_point */
      /* 外接に含まれている三角形のcheckと削除 */
      /* 三角形の作成 */
      /* ドロネー条件のチェックとフリップ操作 */
    }


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

        glBegin(GL_LINES);
          for (const Edge& edge : edges) {
            glVertex2f(edge.start.x, edge.start.y);
            glVertex2f(edge.end.x, edge.end.y);
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
