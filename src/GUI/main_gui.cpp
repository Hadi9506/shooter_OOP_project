#include "main_gui.h"

StartScreen* g_startScreen = nullptr;
PauseScreen* g_pauseScreen = nullptr;
EndScreen* g_endScreen = nullptr;

// ===================== INITIALIZE GUI =====================
void initializeGUI(float screenWidth, float screenHeight) {
    g_startScreen = new StartScreen(screenWidth, screenHeight);
    g_pauseScreen = new PauseScreen(screenWidth, screenHeight);
    g_endScreen = new EndScreen(screenWidth, screenHeight);
}

// ===================== RENDER START MENU SCREEN =====================
void renderStartMenuScreen() {
    if (g_startScreen) {
        g_startScreen->renderStartScreen();
    }
}

// ===================== RENDER PAUSE MENU SCREEN =====================
void renderPauseMenuScreen() {
    if (g_pauseScreen) {
        g_pauseScreen->renderPauseScreen();
    }
}

// ===================== RENDER END MENU SCREEN =====================
void renderEndMenuScreen() {
    if (g_endScreen) {
        g_endScreen->renderEndScreen(0, false);
    }
}

// ===================== HANDLE START MENU CLICK =====================
int handleStartMenuClick() {
    if (!g_startScreen) return -1;
    return g_startScreen->handleButtonClick();
}

// ===================== HANDLE PAUSE MENU CLICK =====================
int handlePauseMenuClick() {
    if (!g_pauseScreen) return -1;
    return g_pauseScreen->handleButtonClick();
}

// ===================== HANDLE END MENU CLICK =====================
int handleEndMenuClick() {
    if (!g_endScreen) return -1;
    return g_endScreen->handleButtonClick();
}

// ===================== UPDATE START MENU MOUSE =====================
void updateStartMenuMouse(double xpos, double ypos) {
    if (g_startScreen) {
        g_startScreen->updateMouse(xpos, ypos);
    }
}

// ===================== UPDATE PAUSE MENU MOUSE =====================
void updatePauseMenuMouse(double xpos, double ypos) {
    if (g_pauseScreen) {
        g_pauseScreen->updateMouse(xpos, ypos);
    }
}

// ===================== UPDATE END MENU MOUSE =====================
void updateEndMenuMouse(double xpos, double ypos) {
    if (g_endScreen) {
        g_endScreen->updateMouse(xpos, ypos);
    }
}

// ===================== HANDLE START MENU MOUSE PRESS =====================
void handleStartMenuMousePress() {
    if (g_startScreen) {
        g_startScreen->handleMousePress();
    }
}

// ===================== HANDLE START MENU MOUSE RELEASE =====================
void handleStartMenuMouseRelease() {
    if (g_startScreen) {
        g_startScreen->handleMouseRelease();
    }
}

// ===================== HANDLE PAUSE MENU MOUSE PRESS =====================
void handlePauseMenuMousePress() {
    if (g_pauseScreen) {
        g_pauseScreen->handleMousePress();
    }
}

// ===================== HANDLE PAUSE MENU MOUSE RELEASE =====================
void handlePauseMenuMouseRelease() {
    if (g_pauseScreen) {
        g_pauseScreen->handleMouseRelease();
    }
}

// ===================== HANDLE END MENU MOUSE PRESS =====================
void handleEndMenuMousePress() {
    if (g_endScreen) {
        g_endScreen->handleMousePress();
    }
}

// ===================== HANDLE END MENU MOUSE RELEASE =====================
void handleEndMenuMouseRelease() {
    if (g_endScreen) {
        g_endScreen->handleMouseRelease();
    }
}

// ===================== SET END SCREEN RESULT =====================
void setEndScreenResult(bool won, int score) {
    if (g_endScreen) {
        g_endScreen->setGameResult(won, score);
    }
}

// ===================== MOUSE CALLBACK =====================
void guiMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    updateStartMenuMouse(xpos, ypos);
    updatePauseMenuMouse(xpos, ypos);
    updateEndMenuMouse(xpos, ypos);
}

// ===================== CLICK CALLBACK =====================
void guiClickCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            handleStartMenuMousePress();
            handlePauseMenuMousePress();
            handleEndMenuMousePress();
        } else if (action == GLFW_RELEASE) {
            handleStartMenuMouseRelease();
            handlePauseMenuMouseRelease();
            handleEndMenuMouseRelease();
        }
    }
}

// ===================== CLEANUP GUI =====================
void cleanupGUI() {
    if (g_startScreen) {
        delete g_startScreen;
        g_startScreen = nullptr;
    }
    if (g_pauseScreen) {
        delete g_pauseScreen;
        g_pauseScreen = nullptr;
    }
    if (g_endScreen) {
        delete g_endScreen;
        g_endScreen = nullptr;
    }
}