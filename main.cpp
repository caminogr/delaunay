#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()


struct Point {
    float x, y;
};

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
    for (int i = 0; i < 100; ++i) {
      points.push_back({
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1, 
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1
      });
    }

    glPointSize(8.0f);
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

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
