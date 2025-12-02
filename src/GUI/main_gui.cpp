#include "main_gui.h"

// Global pointers to screen objects
StartScreen* g_startScreen = nullptr;      // Start menu screen
PauseScreen* g_pauseScreen = nullptr;      // Pause menu screen
EndScreen* g_endScreen = nullptr;          // End game screen

// Initialize GUI system with screen dimensions
// Creates all menu screens
void initializeGUI(float screenWidth, float screenHeight) {
    g_startScreen = new StartScreen(screenWidth, screenHeight);
    g_pauseScreen = new PauseScreen(screenWidth, screenHeight);
    g_endScreen = new EndScreen(screenWidth, screenHeight);
}

// Render start menu screen
// Displays main menu with play and exit buttons
void renderStartMenuScreen() {
    if (g_startScreen) {
        g_startScreen->renderStartScreen();
    }
}

// Render pause menu screen
// Displays pause menu with resume, restart, and main menu buttons
void renderPauseMenuScreen() {
    if (g_pauseScreen) {
        g_pauseScreen->renderPauseScreen();
    }
}

// Render end menu screen
// Displays game over screen with final score
void renderEndMenuScreen() {
    if (g_endScreen) {
        g_endScreen->renderEndScreen(0, false);
    }
}

// Check for start menu button click
// Returns button ID (0=Play, 1=Exit) or -1 if no click
int handleStartMenuClick() {
    if (!g_startScreen) return -1;
    return g_startScreen->handleButtonClick();
}

// Check for pause menu button click
// Returns button ID (0=Resume, 1=Restart, 2=Main Menu) or -1 if no click
int handlePauseMenuClick() {
    if (!g_pauseScreen) return -1;
    return g_pauseScreen->handleButtonClick();
}

// Check for end menu button click
// Returns button ID (0=Play Again, 1=Main Menu) or -1 if no click
int handleEndMenuClick() {
    if (!g_endScreen) return -1;
    return g_endScreen->handleButtonClick();
}

// Update start menu mouse position and hover states
void updateStartMenuMouse(double xpos, double ypos) {
    if (g_startScreen) {
        g_startScreen->updateMouse(xpos, ypos);
    }
}

// Update pause menu mouse position and hover states
void updatePauseMenuMouse(double xpos, double ypos) {
    if (g_pauseScreen) {
        g_pauseScreen->updateMouse(xpos, ypos);
    }
}

// Update end menu mouse position and hover states
void updateEndMenuMouse(double xpos, double ypos) {
    if (g_endScreen) {
        g_endScreen->updateMouse(xpos, ypos);
    }
}

// Called when mouse button is pressed on start menu
void handleStartMenuMousePress() {
    if (g_startScreen) {
        g_startScreen->handleMousePress();
    }
}

// Called when mouse button is released on start menu
void handleStartMenuMouseRelease() {
    if (g_startScreen) {
        g_startScreen->handleMouseRelease();
    }
}

// Called when mouse button is pressed on pause menu
void handlePauseMenuMousePress() {
    if (g_pauseScreen) {
        g_pauseScreen->handleMousePress();
    }
}

// Called when mouse button is released on pause menu
void handlePauseMenuMouseRelease() {
    if (g_pauseScreen) {
        g_pauseScreen->handleMouseRelease();
    }
}

// Called when mouse button is pressed on end menu
void handleEndMenuMousePress() {
    if (g_endScreen) {
        g_endScreen->handleMousePress();
    }
}

// Called when mouse button is released on end menu
void handleEndMenuMouseRelease() {
    if (g_endScreen) {
        g_endScreen->handleMouseRelease();
    }
}

// Set end screen result (won/lost) and final score for display
void setEndScreenResult(bool won, int score) {
    if (g_endScreen) {
        g_endScreen->setGameResult(won, score);
    }
}

// GLFW mouse movement callback
// Routes mouse position to all active menu screens
void guiMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    updateStartMenuMouse(xpos, ypos);
    updatePauseMenuMouse(xpos, ypos);
    updateEndMenuMouse(xpos, ypos);
}

// GLFW mouse button callback
// Routes mouse clicks to all active menu screens
void guiClickCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // Mouse button pressed
            handleStartMenuMousePress();
            handlePauseMenuMousePress();
            handleEndMenuMousePress();
        } else if (action == GLFW_RELEASE) {
            // Mouse button released
            handleStartMenuMouseRelease();
            handlePauseMenuMouseRelease();
            handleEndMenuMouseRelease();
        }
    }
}

// Clean up and deallocate all GUI resources
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