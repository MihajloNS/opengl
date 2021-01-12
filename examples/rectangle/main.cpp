#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define ASSERT(x) assert(x);
#define GL_CALL(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

/**
 * @brief Clear all OpengGL errors internally
 */
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

/**
 * @brief Print OpenGL error
 */
static bool GLLogCall(const char* function, const char* file, int32_t line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Environment variable that represents shader file path.
 */
static const std::string kShaderPath = "SHADER_PATH";

/**
 * @brief Structure that represents vertex and fragment shader source code.
 */
struct ShaderSource
{
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
};

static ShaderSource ParseShader(const std::string& filePath);
static uint32_t CompileShader(uint32_t type, const std::string& source);
static uint32_t CreateShader(const std::string& vertexShader,
                             const std::string& fragmentShader);

int main(void)
{
    GLFWwindow *window = nullptr;
    const char *env = nullptr;

    // Initialize the library
    if (!glfwInit())
    {
        return EXIT_FAILURE;
    }

    env = std::getenv(kShaderPath.c_str());
    if (!env)
    {
        std::cout << "Environment variable for shader path not set" << std::endl;
        return EXIT_FAILURE;
    }

    // Create a window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Triangle example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Make the windows context current
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        return EXIT_FAILURE;
    }

    // Unique vertecies
    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f  // 3
    };

    // Index of the specific vertex
    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Create vertex buffer
    uint32_t vertexBuffer = 0;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // Create index buffer
    uint32_t indexBuffer = 0;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    ShaderSource shaderSource = ParseShader(env);

    // Create shaders
    uint32_t shader = CreateShader(shaderSource.vertexShaderSource, shaderSource.fragmentShaderSource);
    glUseProgram(shader);

    // Loop until the user closes the windows
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw rectangle based on the index buffer
        GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();

    return EXIT_SUCCESS;
}

/**
 * @brief Parse shader source code from the relevant file.
 *
 * @param [in] filePath - path to the shader file
 *
 * @return ShaderSource - shaders source code
 */
static ShaderSource ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1,
        VERTEX,
        FRAGMENT
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[static_cast<int32_t>(type)] << line << '\n';
        }
    }
    return {ss[0].str(), ss[1].str()};
}

/**
 * @brief Compiles specific shader based on type and its source code.
 *
 * @param [in] type   - shader type
 * @param [in] source - shader source code
 *
 * @return uint32_t - shader identifier
 */
static uint32_t CompileShader(uint32_t type, const std::string& source)
{
    int32_t result = GL_FALSE;
    uint32_t shaderId = glCreateShader(type);

    const char *src = source.c_str();
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int32_t length = 0;

        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) alloca(length * sizeof(char));
        glGetShaderInfoLog(shaderId, length, &length, message);

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(shaderId);
        return 0;
    }
    return shaderId;
}

/**
 * @brief Creates shaders based on their source code.
 *
 * @param [in] vertexShader   - vertex shader source code
 * @param [in] fragmentShader - fragment shader source code
 *
 * @return uint32_t - program identifier
 */
static uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    uint32_t programId = glCreateProgram();
    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(programId, vs);
    glAttachShader(programId, fs);
    glLinkProgram(programId);
    glValidateProgram(programId);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return programId;
}
