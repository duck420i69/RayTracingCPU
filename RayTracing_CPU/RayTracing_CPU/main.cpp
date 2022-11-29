#include "threading.h"
#include "loader.h"

#include "GLFW/GLFW3.h"
#include <cstdlib>



int main() {
    GLFWwindow* window;

    std::vector<vec4> frameBuffer;
    float WINDOW_WIDHT = 1200;
    float WINDOW_HEIGHT = 800;
    frameBuffer.resize((int)WINDOW_WIDHT * (int)WINDOW_HEIGHT);

    vec4 direction = { 1.0, 0.0, 0.0, 1.0 };
    direction.normalize();


    Camera cam({ -800, 0.2, 0, 1.0 }, direction, { WINDOW_WIDHT, WINDOW_HEIGHT }, 60);
    Scene scene = loadobj("test/Sponza-master/sponza.obj");


    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(WINDOW_WIDHT, WINDOW_HEIGHT, "Ray tracing", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ThreadPool threadPool;
    threadPool.startWork(cam, scene, frameBuffer);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;


        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawPixels(width, height, GL_RGBA, GL_FLOAT, &frameBuffer.front());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);


}

