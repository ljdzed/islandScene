#include <structs.h>
// #include <iostream>
//
// #include <glad/gl.h>
// #include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>
//
// // GLTF model loader
// #define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <tiny_gltf.h>
//
// #include <render/shader.h>
//
// #include <vector>
// #include <iostream>
// #define _USE_MATH_DEFINES
// #include <math.h>
// #include <iomanip>



static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void cursor_callback(GLFWwindow *window, double xpos, double ypos);






// struct Object
// {
//     virtual ~Object() = default;  // Virtual destructor for proper cleanup
//
//     // Could be a pure virtual function (making Object an abstract type)
//     virtual void render() = 0;
// };




// This function retrieves and stores the depth map of the default frame buffer
// or a particular frame buffer (indicated by FBO ID) to a PNG image.
static void saveDepthTexture(GLuint fbo, std::string filename) {
    int width = shadowMapWidth;
    int height = shadowMapHeight;
    if (shadowMapWidth == 0 || shadowMapHeight == 0) {
        width = windowWidth;
        height = windowHeight;
    }
    int channels = 3;

    std::vector<float> depth(width * height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glReadBuffer(GL_NONE); // Custom framebuffer without color attachment

    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<unsigned char> img(width * height * 3);
    for (int i = 0; i < width * height; ++i) img[3*i] = img[3*i+1] = img[3*i+2] = depth[i] * 255;

    stbi_write_png(filename.c_str(), width, height, channels, img.data(), width * channels);
}





int main()
{
    std::cout << "Hello, World!" << std::endl;

    // Initialise GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, "Lab 4", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to open a GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);

    // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize OpenGL context." << std::endl;
        return -1;
    }

    // Background
    glClearColor(0.6f, 0.85f, 0.90f, 1.0f); // light blue

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    // Camera setup
    glm::mat4 viewMatrix, projectionMatrix;
    projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

    // Time and frame rate tracking
    static double lastTime = glfwGetTime();
    float time = 0.0f;			// Animation time
    float fTime = 0.0f;			// Time for measuring fps
    unsigned long frames = 0;


    Skybox skybox;
    skybox.initialize(glm::vec3(0,0,0), glm::vec3(100,100,100));

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        viewMatrix = glm::lookAt(eye_center, lookat, up);
        glm::mat4 vp = projectionMatrix * viewMatrix;

        // Skybox Renders First
        glDepthMask(GL_FALSE); // disable to also see city (put it behind everything
        skybox.render(vp, eye_center);
        glDepthMask(GL_TRUE);  // re-enable

        // Object Rendering

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    while (!glfwWindowShouldClose(window));

    // Clean up


    // Close OpenGL window and terminate GLFW
    glfwTerminate();


    return 0;
}


static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        playbackSpeed += 1.0f;
        if (playbackSpeed > 10.0f)
            playbackSpeed = 10.0f;
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        playbackSpeed -= 1.0f;
        if (playbackSpeed < 1.0f) {
            playbackSpeed = 1.0f;
        }
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        playAnimation = !playAnimation;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}