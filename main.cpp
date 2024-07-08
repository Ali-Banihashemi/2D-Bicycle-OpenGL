#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#define M_PI 3.14

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 color;

void main()
{
    FragColor = color;
}
)";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void processInput(GLFWwindow* window, float& centerX, float& centerY, float& angle, float& pedalX, float& pedalY)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        centerX += 0.005f;   // Move to the right
        angle -= 1.5f;      // Rotate clockwise

        float rad = 3*angle * M_PI / 180.0f;

        pedalX =  0.07f * cosf(rad);
        pedalY =  0.07f * sinf(rad);
        
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        centerX -= 0.005f;   // Move to the right
        angle += 1.5f;      // Rotate clockwise
        float rad = 3*angle * M_PI / 180.0f;
        pedalX = 0.07f * cosf(rad);
        pedalY = 0.07f * sinf(rad);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        centerY += 0.005f;   // Move to the right     // Rotate clockwise
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        centerY -= 0.005f;   // Move to the right   // Rotate clockwise
    }
}

void drawHollowCircle(unsigned int shaderProgram, float centerX, float centerY, float radius, int numSegments, float r, float g, float b, float thickness)
{
    float* vertices = new float[numSegments * 3];

    for (int i = 0; i < numSegments; ++i)
    {
        float angle = 2.0f * M_PI * float(i) / float(numSegments);
        vertices[i * 3] = centerX + radius * cosf(angle);
        vertices[i * 3 + 1] = centerY + radius * sinf(angle);
        vertices[i * 3 + 2] = 0.0f;
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numSegments * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set the stroke thickness
    glLineWidth(thickness);

    // Set the color uniform
    glUseProgram(shaderProgram);
    int colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(colorLocation, r, g, b, 1.0f);

    glDrawArrays(GL_LINE_LOOP, 0, numSegments);

    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete[] vertices;
}

void drawLine(unsigned int shaderProgram, float startX, float startY, float endX, float endY, float r, float g, float b, float thickness)
{
    float vertices[] = {
        startX, startY, 0.0f,
        endX, endY, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set the line thickness
    glLineWidth(thickness);

    // Set the color uniform
    glUseProgram(shaderProgram);
    int colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(colorLocation, r, g, b, 1.0f);

    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void drawLine2(unsigned int shaderProgram, float centerX, float centerY, float length, float angle, float r, float g, float b, float thickness)
{
    float halfLength = length / 2.0f;
    float rad = angle * M_PI / 180.0f;

    float startX = centerX + halfLength * cosf(rad);
    float startY = centerY + halfLength * sinf(rad);
    float endX = centerX - halfLength * cosf(rad);
    float endY = centerY - halfLength * sinf(rad);

    drawLine(shaderProgram, startX, startY, endX, endY, r, g, b, thickness);
}

void drawCircle(unsigned int shaderProgram, float centerX, float centerY, float radius, int numSegments, float r, float g, float b)
{
    float* vertices = new float[(numSegments + 2) * 3];

    // Center point
    vertices[0] = centerX;
    vertices[1] = centerY;
    vertices[2] = 0.0f;

    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = 2.0f * M_PI * float(i) / float(numSegments);
        vertices[(i + 1) * 3] = centerX + radius * cosf(angle);
        vertices[(i + 1) * 3 + 1] = centerY + radius * sinf(angle);
        vertices[(i + 1) * 3 + 2] = 0.0f;
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (numSegments + 2) * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set the color uniform
    glUseProgram(shaderProgram);
    int colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(colorLocation, r, g, b, 1.0f);

    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete[] vertices;
}


int main()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Bike", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1000, 1000);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Initial line parameters
    float centerX = -0.5f, centerY = 0.0f;
    float angle = 0.0f;
    float length = 0.3f;

    float pedalX = 0.07f;
    float pedalY = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, centerX, centerY, angle, pedalX,pedalY);

        glClearColor(1.0f, 0.9f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        //pedal aghab
        drawLine(shaderProgram, centerX + 0.27f, centerY, centerX - pedalX + 0.27f, centerY - pedalY, 0.2f, 0.2f, 0.2f, 5.0f);
        drawLine2(shaderProgram, centerX - pedalX + 0.27f, centerY - pedalY, 0.04, 0.0f, 0.05f, 0.05f, 0.05f, 8.0f);
        // charkh aghab
        drawLine2(shaderProgram, centerX, centerY, length, angle, 0.65f, 0.65f, 0.65f, 5.0f);
        drawLine2(shaderProgram, centerX, centerY, length, angle+45.0f, 0.65f, 0.65f, 0.65f, 5.0f);
        drawLine2(shaderProgram, centerX, centerY, length, angle+90.0f, 0.65f, 0.65f, 0.65f, 5.0f);
        drawLine2(shaderProgram, centerX, centerY, length, angle+135.0f, 0.65f, 0.65f, 0.65f, 5.0f);
        drawHollowCircle(shaderProgram, centerX , centerY, length / 2, 200, 0.0f, 0.0f, 0.0f, 7.0f);
        drawCircle(shaderProgram, centerX-0.106f, centerY-0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawCircle(shaderProgram, centerX - 0.106f, centerY + 0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawCircle(shaderProgram, centerX + 0.106f, centerY - 0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawCircle(shaderProgram, centerX + 0.106f, centerY + 0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawHollowCircle(shaderProgram, centerX, centerY, length / 2-0.008f, 200, 0.65f, 0.65f, 0.65f, 5.0f);
        
        //charkh jolo
        drawLine2(shaderProgram, centerX + 0.6f, centerY, length, angle, 0.65f, 0.65f, 0.65f, 5.0f);
        drawLine2(shaderProgram, centerX+0.6f, centerY, length, angle + 45.0f, 0.65f, 0.65f, 0.65f, 5.0f);
        drawLine2(shaderProgram, centerX + 0.6f, centerY, length, angle + 90.0f, 0.65f, 0.65f, 0.65f, 5.0f);
        drawLine2(shaderProgram, centerX + 0.6f, centerY, length, angle + 135.0f, 0.65f, 0.65f, 0.65f, 5.0f);
        drawHollowCircle(shaderProgram, centerX + 0.6f, centerY, length/2, 200, 0.0f, 0.0f, 0.0f, 7.0f);
        drawCircle(shaderProgram, centerX - 0.106f + 0.6f, centerY - 0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawCircle(shaderProgram, centerX - 0.106f + 0.6f, centerY + 0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawCircle(shaderProgram, centerX + 0.106f + 0.6f, centerY - 0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawCircle(shaderProgram, centerX + 0.106f + 0.6f, centerY + 0.106f, 0.0055f, 200, 0.0f, 0.0f, 0.0f);
        drawHollowCircle(shaderProgram, centerX + 0.6f, centerY, length / 2 - 0.008f, 200, 0.65f, 0.65f, 0.65f, 5.0f);

        //badane
        drawLine(shaderProgram, centerX + 0.6f, centerY, centerX + 0.52f, centerY+0.3f, 1.0f, 0.17f, 0.27f,8.0f);
        drawCircle(shaderProgram, centerX + 0.6f, centerY, 0.006, 200, 1.0f, 0.17f, 0.27f);
        drawLine(shaderProgram,  centerX + 0.52f, centerY + 0.3f, centerX + 0.505f, centerY + 0.36f, 0.65f, 0.65f, 0.65f, 8.0f);
        drawLine(shaderProgram, centerX + 0.47f, centerY + 0.36f, centerX + 0.54f, centerY + 0.36f, 0.0f, 0.0f, 0.0f, 8.0f);
        drawCircle(shaderProgram, centerX + 0.47f, centerY + 0.36f, 0.015, 50, 0.0f, 0.0f, 0.0f);
        drawCircle(shaderProgram, centerX + 0.54f, centerY + 0.36f, 0.012, 50, 0.0f, 0.0f, 0.0f);
        drawLine(shaderProgram, centerX, centerY, centerX + 0.27f, centerY, 1.0f, 0.17f, 0.27f, 8.0f);
        drawLine(shaderProgram, centerX, centerY, centerX + 0.22f, centerY + 0.2f, 1.0f, 0.17f, 0.27f, 8.0f);
        drawLine(shaderProgram, centerX + 0.27f, centerY, centerX + 0.21f, centerY + 0.23f, 1.0f, 0.17f, 0.27f, 8.0f);
        drawLine(shaderProgram, centerX + 0.21f, centerY + 0.23f, centerX + 0.20f, centerY + 0.26f, 0.65f, 0.65f, 0.65f, 8.0f);
        drawLine(shaderProgram, centerX + 0.16f, centerY + 0.255f, centerX + 0.23f, centerY + 0.26f, 0.0f, 0.0f, 0.0f, 8.0f);
        drawCircle(shaderProgram, centerX + 0.16f, centerY + 0.265f, 0.015f, 200, 0.0f, 0.0f, 0.0f);
        drawLine(shaderProgram, centerX + 0.16f, centerY + 0.275f, centerX + 0.23f, centerY + 0.26f, 0.0f, 0.0f, 0.0f, 8.0f);
        drawLine(shaderProgram, centerX + 0.27f, centerY, centerX + 0.53f, centerY + 0.26f, 1.0f, 0.17f, 0.27f, 8.0f);
        drawLine(shaderProgram, centerX + 0.22f, centerY + 0.2f, centerX + 0.53f, centerY + 0.28f, 1.0f, 0.17f, 0.27f, 8.0f);
        drawCircle(shaderProgram, centerX, centerY, 0.006, 200, 1.0f, 0.17f, 0.27f);

        //pedal jolo
        drawCircle(shaderProgram, centerX + 0.27f, centerY, 0.04, 200, 0.45f, 0.45f, 0.45f);
        drawLine(shaderProgram, centerX + 0.27f, centerY, centerX + pedalX+0.27f, centerY+pedalY, 0.2f, 0.2f, 0.2f, 5.0f);
        drawLine2(shaderProgram, centerX + pedalX + 0.27f, centerY + pedalY, 0.04, 0.0f, 0.05f, 0.05f, 0.05f, 8.0f);
        drawCircle(shaderProgram, centerX + 0.27f, centerY, 0.02, 200, 1.0f, 0.17f, 0.27f);
        //
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
