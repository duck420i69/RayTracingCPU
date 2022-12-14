#include "input.h"
#include <algorithm>
#include <iostream>

static double mouse_x = 0.0, mouse_y = 0.0; // this look dumb but is there other way?


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        std::cout << mouse_x << " " << mouse_y << "\n";
    }    
}

void get_mouse_pos(double* x, double* y) {
    *x = mouse_x;
    *y = mouse_y;
}

long get_buffer_pos(const int& width, const int& height) {
    return std::max(0.0, std::min(mouse_x + (height - mouse_y) * width, double(width * height - 1)));
}