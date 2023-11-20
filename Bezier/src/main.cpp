#if 1
#include <iostream>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "objects.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_clickCallBack(GLFWwindow* window, int button, int state, int mod);
//set binomial coefficients
void set_BC_arr(std::vector<float>& BC_arr, int n);
//calculate line
void set_line_points(std::vector<float>& line_points_arr, const std::vector<float>& BC_arr, const std::vector<float>& ctr_points, int n, float precision);

// settings
#ifndef SIZE
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
#endif // !SIZE

std::vector<float> init_ctr_points = {
     0.5f, 0.5f,
     0.5f, -0.5f,
    -0.5f, -0.5f,
};
std::vector<float> BC_arr{};
int BC_order = 2;
std::vector<float> ctr_points = {
     0.5f, 0.5f,
     0.5f, -0.5f,
    -0.5f, -0.5f,
};
int current_ctr_point = 1;
float precision = 0.05, speed = 0.01;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_clickCallBack);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    Shader BezierShader("D:\\Source\\repos\\for_CG\\Bezier\\src\\Shader\\vertex shader\\Bezier.vert", "D:\\Source\\repos\\for_CG\\Bezier\\src\\Shader\\fragment shader\\Bezier.frag");


    unsigned int ctr_VBO, ctr_VAO;
    glGenVertexArrays(1, &ctr_VAO);
    glGenBuffers(1, &ctr_VBO);

    glBindVertexArray(ctr_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, ctr_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*ctr_points.size(), ctr_points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<float>line_points{};

    unsigned int line_VBO, line_VAO;
    glGenVertexArrays(1, &line_VAO);
    glGenBuffers(1, &line_VBO);

    glBindVertexArray(line_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*line_points.size(), line_points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //binomial coefficients array
    set_BC_arr(BC_arr, BC_order);

    //initial line
    set_line_points(line_points, BC_arr, ctr_points, ctr_points.size() / 2 - 1, precision);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render here
        BezierShader.use();
        glBindVertexArray(ctr_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, ctr_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ctr_points.size(), ctr_points.data(), GL_STATIC_DRAW);
        BezierShader.setVec3("Color", glm::vec3(1.0f, 1.0f, 0.0f));
        glDrawArrays(GL_POINTS, 0, ctr_points.size() / 2);

        set_line_points(line_points, BC_arr, ctr_points, ctr_points.size() / 2 - 1, precision);
        BezierShader.use();
        glBindVertexArray(line_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * line_points.size(), line_points.data(), GL_STATIC_DRAW);
        BezierShader.setVec3("Color", glm::vec3(0.0f, 1.0f, 1.0f));
        glDrawArrays(GL_LINES, 0, line_points.size()/2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &ctr_VAO);
    glDeleteBuffers(1, &ctr_VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ctr_points[2 * current_ctr_point + 1] += speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ctr_points[2 * current_ctr_point + 1] -= speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ctr_points[2 * current_ctr_point] -= speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ctr_points[2 * current_ctr_point] += speed;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        ctr_points = init_ctr_points;
        BC_order = 2;
        set_BC_arr(BC_arr, BC_order);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        speed = 0.005;
    }
    else speed = 0.01;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void mouse_clickCallBack(GLFWwindow* window, int button, int state, int mod)
{
    double x, y;
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS) {
        glfwGetCursorPos(window, &x, &y);
        for (int i = 0; i < ctr_points.size() / 2; i++) {
            if (abs((ctr_points[2 * i]+1) * SCR_WIDTH / 2 - x) <= 5 && abs((ctr_points[2 * i + 1]-1) * SCR_HEIGHT / 2 + y) <= 5)
                current_ctr_point = i;
        }
        std::cout << current_ctr_point << " " << x - SCR_WIDTH / 2 << " " << -y + SCR_HEIGHT / 2 << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS) {
        glfwGetCursorPos(window, &x, &y);
        ctr_points.push_back((x - SCR_WIDTH / 2) * 2 / SCR_WIDTH);
        ctr_points.push_back((SCR_HEIGHT / 2 - y) * 2 / SCR_HEIGHT);
        BC_order++;
        set_BC_arr(BC_arr, BC_order);
    }
}

void set_BC_arr(std::vector<float>& BC_arr, int n)
{
    BC_arr.clear();
    int k, j;
    for (k = 0; k <= n; k++)
    {
        BC_arr.push_back(1);
        for (j = n; j >= k + 1; j--)//1*n*(n-1)*(n-2)*...*(k+2)*(k+1)
        {
            BC_arr[k] *= j;
        }
        for (j = n - k; j >= 2; j--)//иой╫ / ( (n-k)*(n-k-1)*...*3*2*1 )
        {
            BC_arr[k] /= j;
        }
    }
}

void set_line_points(std::vector<float>& line_points_arr, const std::vector<float>& BC_arr, const std::vector<float>& ctr_points, int n, float precision)
{
    if (n + 1 != BC_arr.size()) {
        std::cout << "ERROR: orders of binomial coefficients array and Bezier points not match";
        return;
    }
    if (n + 1 != ctr_points.size()/2) {
        std::cout << "ERROR: orders of control points array and Bezier points not match";
        return;
    }

    line_points_arr.clear();

    line_points_arr.push_back(*(ctr_points.rbegin() + 1));
    line_points_arr.push_back(*ctr_points.rbegin());

    //t in t^mt^{n-m}
    double t = precision;
    int point_num = 1;
    while (t<=1)
    {
        line_points_arr.push_back(0);
        line_points_arr.push_back(0);

        double p = pow(t, n);
        for (int i = 0; i < n + 1; i++) {
            line_points_arr[4 * point_num - 2] += BC_arr[i] * p * ctr_points[2 * i];
            line_points_arr[4 * point_num - 1] += BC_arr[i] * p * ctr_points[2 * i + 1];
            p = p * (1 - t) / t;
        }
        line_points_arr.push_back(line_points_arr[4 * point_num - 2]);
        line_points_arr.push_back(line_points_arr[4 * point_num - 1]);
        point_num++;
        t += precision;
    }

    line_points_arr.push_back(ctr_points[0]);
    line_points_arr.push_back(ctr_points[1]);

    return;
}
#endif