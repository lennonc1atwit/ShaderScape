#include <Window.h>
#include <functional>

static void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW returned an error:\n\t%s (%i)\n", description, error);
}

static void GLAPIENTRY GLErrorCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (severity == GL_DEBUG_TYPE_ERROR)
        std::cout << "[OpenGL Error](" << type << ") " << message << std::endl;
}

static void handleKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

Window::Window(const char* title, int width, int height)
{
    
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        fprintf(stderr, "Could not start GLFW\n");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    Width = width; Height = height;
    _glfwWindowPtr = glfwCreateWindow(Width, Height, title, nullptr, nullptr);

    // Check for Valid Context
    if (_glfwWindowPtr == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(_glfwWindowPtr, handleKeyboardInput);
    glfwSetFramebufferSizeCallback(_glfwWindowPtr, OnResize);
    
    //glDebugMessageCallback(GLErrorCallback, 0);


    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(_glfwWindowPtr);
    gladLoadGL(glfwGetProcAddress);

    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(10.f);

    // Configure miscellaneous OpenGL settings
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);

    // Print various OpenGL information to stdout
    printf("%s: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
    printf("GLFW\t %s\n", glfwGetVersionString());
    printf("OpenGL\t %s\n", glGetString(GL_VERSION));
    printf("GLSL\t %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

Window::~Window()
{
    // Terminate GLFW (no need to call glfwDestroyWindow)
    glfwTerminate();
}

void Window::BeginFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::EndFrame()
{
    SwapBuffers();
}