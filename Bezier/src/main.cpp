#if 1
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Bezier.h"
#include "shader.h"
#include "stb/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Bezier& bezier);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//TODO: consider use factory to register diffrent func for diffrent Bezier line
void mouse_clickCallBack(GLFWwindow* window, int button, int state, int mod);
void load_log(const char* path);

// settings
#ifndef SIZE
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
#endif // !SIZE

int current_ctr_point = 1, current_ctr_line = 0;
float speed = 0.01;

//TODO: consider a way to avoid global variable 
//dependent func: mouse_clickCallBack
Bezier_array lines;
int cur_line = 0;

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

    Bezier line1,line2;
    line2.set_ctr_point(0, -0.9, -0.9);

    lines.add(line1,line2);

    unsigned int ctr_VBO, ctr_VAO;
    glGenVertexArrays(1, &ctr_VAO);
    glGenBuffers(1, &ctr_VBO);

    glBindVertexArray(ctr_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, ctr_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * line1.get_ctr_num() * 2, line1.get_ctr_pointer(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int line_VBO, line_VAO;
    glGenVertexArrays(1, &line_VAO);
    glGenBuffers(1, &line_VBO);

    glBindVertexArray(line_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*line1.get_line_num()*2, line1.get_line_pointer(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);

    load_log("src/Readme.txt");
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, lines[cur_line]);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render here
        for (int index = 0; index < Bezier_array::MAX_SIZE; index++) {
            if (lines.check_flag(index)) {
                BezierShader.use();
                glBindVertexArray(ctr_VAO);
                glBindBuffer(GL_ARRAY_BUFFER, ctr_VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lines[index].get_ctr_num() * 2, lines[index].get_ctr_pointer(), GL_STATIC_DRAW);
                BezierShader.setVec3("Color", glm::vec3(1.0f, 1.0f, 0.0f));
                glDrawArrays(GL_POINTS, 0, lines[index].get_ctr_num());

                BezierShader.use();
                glBindVertexArray(line_VAO);
                glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lines[index].get_line_num() * 2, lines[index].get_line_pointer(), GL_STATIC_DRAW);
                BezierShader.setVec3("Color", glm::vec3(0.0f, 1.0f, 1.0f));
                glDrawArrays(GL_LINES, 0, lines[index].get_line_num());
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &ctr_VAO);
    glDeleteBuffers(1, &ctr_VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, Bezier& bezier)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        bezier.get_ctr_pointer()[2 * current_ctr_point + 1] += speed;
        bezier.set_line_points();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        bezier.get_ctr_pointer()[2 * current_ctr_point + 1] -= speed;
        bezier.set_line_points();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        bezier.get_ctr_pointer()[2 * current_ctr_point] -= speed;
        bezier.set_line_points();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        bezier.get_ctr_pointer()[2 * current_ctr_point] += speed;
        bezier.set_line_points();
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        bezier.reset_ctr_points();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        speed = 0.001;
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
        bool should_break = false;
        for (int index = 0; index < Bezier_array::MAX_SIZE; index++) {
            if (lines.check_flag(index)) {
                for (int i = 0; i < lines[index].get_ctr_num(); i++) {
                    if (abs((lines[index].get_ctr_pointer()[2 * i] + 1) * SCR_WIDTH / 2 - x) <= 5 && abs((lines[index].get_ctr_pointer()[2 * i + 1] - 1) * SCR_HEIGHT / 2 + y) <= 5) {
                        current_ctr_point = i;
                        cur_line = index;
                        should_break = true;
                        break;
                    }
                }
                if (should_break) {
#ifdef _DEBUG
                    std::cout << index << " " << current_ctr_point << " " << x - SCR_WIDTH / 2 << " " << -y + SCR_HEIGHT / 2 << std::endl;
#endif
                    break;
                }
            }
        }

    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS) {
        glfwGetCursorPos(window, &x, &y);
        lines[cur_line].add_ctr_point((x - SCR_WIDTH / 2) * 2 / SCR_WIDTH, (SCR_HEIGHT / 2 - y) * 2 / SCR_HEIGHT);
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_PRESS) {
        lines.add(Bezier());
    }
}

void load_log(const char* path)
{
    std::ifstream infile;
    infile.open(path, std::ios::in);
    if (!infile.is_open())
    {
        std::cout << "ERROR: can't open log at " << path << std::endl;
        return;
    }
    std::string buf;
    while (getline(infile, buf))
    {
        std::cout << buf << std::endl;
    }
    infile.close();
}
#endif