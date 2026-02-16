///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manages the viewing of 3D objects within the viewport - camera, projection
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"
#include "GLFW/glfw3.h"

class ViewManager
{
public:
    ViewManager(ShaderManager* pShaderManager);
    ~ViewManager();

    // Input callbacks
    static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);
    static void Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);

    // Window and rendering
    GLFWwindow* CreateDisplayWindow(const char* windowTitle);
    void PrepareSceneView();

private:
    ShaderManager* m_pShaderManager;
    GLFWwindow* m_pWindow;

    bool m_useOrthographicProjection;

    void ProcessKeyboardEvents();
    void ToggleProjection();
};