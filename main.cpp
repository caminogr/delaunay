#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()

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
    glViewport(0, 0, 800, 600);

    // Seed for random number generation
    srand(static_cast<unsigned int>(time(nullptr)));

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw random points
        glBegin(GL_POINTS);
        for (int i = 0; i < 100; ++i) {
            float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            glVertex2f(x * 2 - 1, y * 2 - 1); // Map to [-1, 1] range
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
