#include "main_gui.h"

StartScreen* g_startScreen = nullptr;

// ===================== INITIALIZE GUI =====================
void initializeGUI(float screenWidth, float screenHeight) {
    g_startScreen = new StartScreen(screenWidth, screenHeight);
}

// ===================== RENDER START MENU SCREEN =====================
void renderStartMenuScreen() {
    if (g_startScreen) {
        g_startScreen->renderStartScreen();
    }
}

// ===================== HANDLE START MENU CLICK =====================
int handleStartMenuClick() {
    if (!g_startScreen) return -1;
    return g_startScreen->handleButtonClick();
}

// ===================== UPDATE START MENU MOUSE =====================
void updateStartMenuMouse(double xpos, double ypos) {
    if (g_startScreen) {
        g_startScreen->updateMouse(xpos, ypos);
    }
}

// ===================== HANDLE MOUSE PRESS =====================
void handleStartMenuMousePress() {
    if (g_startScreen) {
        g_startScreen->handleMousePress();
    }
}

// ===================== HANDLE MOUSE RELEASE =====================
void handleStartMenuMouseRelease() {
    if (g_startScreen) {
        g_startScreen->handleMouseRelease();
    }
}

// ===================== MOUSE CALLBACK =====================
void guiMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    updateStartMenuMouse(xpos, ypos);
}

// ===================== CLICK CALLBACK =====================
void guiClickCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            handleStartMenuMousePress();
        } else if (action == GLFW_RELEASE) {
            handleStartMenuMouseRelease();
        }
    }
}

// ===================== CLEANUP GUI =====================
void cleanupGUI() {
    if (g_startScreen) {
        delete g_startScreen;
        g_startScreen = nullptr;
    }
}