#include "threading.h"
#include "loader.h"
#include "input.h"

#include "GLFW/GLFW3.h"
#include <cstdlib>



int main() {
    GLFWwindow* window;
  
    float WINDOW_WIDHT = 1200;
    float WINDOW_HEIGHT = 800;

    std::vector<vec4> frameBuffer;
    frameBuffer.resize((int)WINDOW_WIDHT * (int)WINDOW_HEIGHT);

    vec4 direction = { -1.0f, 0.0f, 0.0f, 1.0f };
    direction.normalize();


    Camera cam({ 13, 2, 0, 1.0 }, direction, { WINDOW_WIDHT, WINDOW_HEIGHT }, 35);

    Scene scene = loadobj("test/Odd scene/sphere3.obj");
    //Scene scene = loadobj("test/Sponza-master/sponza.obj");
    //Scene scene = loadobj("test/Old House 2/Old House Files/Old House 2 3D Models.obj");

    scene.global_light_dir = { 0.6f, 1.4f, 0.6f, 1.0f };
    scene.global_light_dir.normalize();

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(WINDOW_WIDHT, WINDOW_HEIGHT, "Ray tracing", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetMouseButtonCallback(window, mouse_button_callback);

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
        threadPool.setDebugPosition(get_buffer_pos(WINDOW_WIDHT, WINDOW_HEIGHT));
        frameBuffer[get_buffer_pos(WINDOW_WIDHT, WINDOW_HEIGHT)] = { 1.0f, 0.0f, 0.0f, 1.0f };
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
