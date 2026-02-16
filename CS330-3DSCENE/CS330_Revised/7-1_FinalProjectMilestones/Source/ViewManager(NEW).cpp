///////////////////////////////////////////////////////////////////////////////
// viewmanager.cpp
// =================
// Responsible for managing the camera view, projection mode, and user input
// related to viewing the 3D scene. This includes handling keyboard and mouse
// input, configuring perspective and orthographic projections, and updating
// shader uniforms used for rendering.
//
// This class acts as the bridge between user interaction and the visual
// representation of the scene.
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace
{
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 800;

    const char* VIEW_NAME = "view";
    const char* PROJECTION_NAME = "projection";

    Camera* g_pCamera = nullptr;

    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    float gDeltaTime = 0.0f;
    float gLastFrame = 0.0f;
}

/***********************************************************
 * Constructor
 ***********************************************************/
ViewManager::ViewManager(ShaderManager* pShaderManager)
    : m_pShaderManager(pShaderManager),
      m_pWindow(nullptr),
      m_useOrthographicProjection(false)
{

	 // Initialize the camera with a default position and orientation
    // suitable for viewing the full 3D scene at startup.
    g_pCamera = new Camera();
    g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
    g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
    g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Configure camera behavior
    g_pCamera->Zoom = 80.0f;
    g_pCamera->MovementSpeed = 20.0f;
}

/***********************************************************
 * Destructor
 ***********************************************************/
ViewManager::~ViewManager()
{
    delete g_pCamera;
    g_pCamera = nullptr;
}

/***********************************************************
 * Create Display Window
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        windowTitle,
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, Mouse_Position_Callback);
    glfwSetScrollCallback(window, Scroll_Callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pWindow = window;
    return window;
}

/***********************************************************
 * Mouse Callback
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow*, double xMousePos, double yMousePos)
{
	// Prevent sudden camera jump on first mouse movement
    if (gFirstMouse)
    {
        gLastX = static_cast<float>(xMousePos);
        gLastY = static_cast<float>(yMousePos);
        gFirstMouse = false;
    }

	 // Calculate offset since last frame
    float xOffset = static_cast<float>(xMousePos) - gLastX;
    float yOffset = gLastY - static_cast<float>(yMousePos);

	// Store current position for next callback
    gLastX = static_cast<float>(xMousePos);
    gLastY = static_cast<float>(yMousePos);
	
	// Apply mouse movement to camera rotation
    if (g_pCamera)
        g_pCamera->ProcessMouseMovement(xOffset, yOffset);
}

/***********************************************************
 * Scroll Callback
 ***********************************************************/
void ViewManager::Scroll_Callback(GLFWwindow*, double, double yOffset)
{
    if (!g_pCamera) return;
	
	// Adjust camera zoom level using scroll input
    g_pCamera->Zoom -= static_cast<float>(yOffset);

	// Clamp zoom to prevent extreme distortion or inversion
    g_pCamera->Zoom = glm::clamp(g_pCamera->Zoom, 1.0f, 120.0f);
}

/***********************************************************
 * Keyboard Input
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
    if (!g_pCamera || !m_pWindow) return;

	// Allow user to close the application
    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_pWindow, true);

	// Standard WASD-style camera movement
    if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS) g_pCamera->ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS) g_pCamera->ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS) g_pCamera->ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS) g_pCamera->ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS) g_pCamera->ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS) g_pCamera->ProcessKeyboard(UP, gDeltaTime);

	// Toggle between perspective and orthographic projection
    if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS)
        ToggleProjection();
}

/***********************************************************
 * Toggle Projection Mode
 ***********************************************************/
void ViewManager::ToggleProjection()
{
    m_useOrthographicProjection = !m_useOrthographicProjection;
}

/***********************************************************
 * Prepare Scene View
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	// Calculate frame timing for smooth camera movement
    float currentFrame = static_cast<float>(glfwGetTime());
    gDeltaTime = currentFrame - gLastFrame;
    gLastFrame = currentFrame;

	// Process user input before rendering
    ProcessKeyboardEvents();

	// Generate view matrix from camera position and orientation
    glm::mat4 view = g_pCamera->GetViewMatrix();
    glm::mat4 projection;

	// Select projection type based on current mode
    if (m_useOrthographicProjection)
    {
        float scale = 10.0f;
        projection = glm::ortho(-scale, scale, -scale, scale, 0.1f, 100.0f);
    }
    else
    {
        projection = glm::perspective(
            glm::radians(g_pCamera->Zoom),
            static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT,
            0.1f,
            100.0f
        );
    }

	// Send updated matrices to the active shader program
    if (m_pShaderManager)
    {
        m_pShaderManager->setMat4Value(VIEW_NAME, view);
        m_pShaderManager->setMat4Value(PROJECTION_NAME, projection);
        m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
    }
}