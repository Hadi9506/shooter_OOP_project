#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Camera.h"
#include "World.h"
#include "Enemy.h"
#include "Shooter.h"
#include "Shader.h"
#include "TextRenderer.h"
#include "GUI/main_gui.h"
#include "Item.h"

// Global game state variables
int playerHealth = 100;
int score = 0;
int currentAmmo = 30;
int reserveMags = 0;
int partialMagAmmo = 0;

// Reload helper function that manages ammunition
// Takes current magazine size, reserves, partial ammo, and current ammo to reload properly
void reload(int mag_size, int &reserved_mags, int &partial_ammos, int &current_ammos) {
    // Calculate how many bullets are needed to fill the current magazine
    int needed = mag_size - current_ammos;
    
    // If magazine is already full, do nothing
    if (needed == 0) return;
    
    // First priority: use partial ammo from previously ejected magazines
    int use_partial = (partial_ammos < needed) ? partial_ammos : needed;
    current_ammos += use_partial;
    partial_ammos -= use_partial;
    needed -= use_partial;

    // Second priority: if still not full, use full reserve magazines
    if (needed > 0 && reserved_mags > 0) {
        // Take one full magazine from reserves
        reserved_mags--;
        
        // Fill as much as needed from that magazine
        int give = (mag_size < needed) ? mag_size : needed;
        current_ammos += give;
        needed -= give;
        
        // If magazine wasn't fully used, leftover becomes partial ammo
        if (give < mag_size) {
            partial_ammos += (mag_size - give);
        }
    }
}

// Forward declarations for callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
unsigned int createCubeVAO();

// Screen dimensions that can change with window resizing
int SCR_WIDTH = 1200;
int SCR_HEIGHT = 800;

// Current game screen state
GameScreen currentGameScreen = GameScreen::START_MENU;
bool pauseKeyPressed = false;

int main() {
    // Initialize GLFW library for window creation
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the main game window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OOP Project", nullptr, nullptr);
    if (!window) { 
        std::cerr << "Failed GLFW\n"; 
        glfwTerminate(); 
        return -1; 
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed GLAD\n"; 
        return -1;
    }

    // Enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    
    // Register input callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, guiClickCallback);

    // Create and configure camera
    Camera camera(glm::vec3(0.0f, 2.0f, 5.0f));
    glfwSetWindowUserPointer(window, &camera);

    // Load shaders for 3D rendering and crosshair
    Shader shader3D("resources/basic.vert", "resources/basic.frag");
    Shader shaderCrosshair("resources/crosshair.vert", "resources/crosshair.frag");

    // Create vertex array for cube rendering
    unsigned int cubeVAO = createCubeVAO();

    // Create crosshair visualization (2D overlay)
    unsigned int crosshairVAO, crosshairVBO;
    float crosshairVerts[] = {
        -0.02f,  0.0f,    // Horizontal line left
         0.02f,  0.0f,    // Horizontal line right
         0.0f, -0.02f,    // Vertical line bottom
         0.0f,  0.02f     // Vertical line top
    };
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVerts), crosshairVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Generate world geometry
    World world;
    world.generate();

    // Create and spawn enemies
    EnemyManager enemies;
    enemies.spawn(glm::vec3(-3, 1.5, -1), glm::vec3(1, 0.2, 0.2));   // Red enemy
    enemies.spawn(glm::vec3(3, 1.5, -1), glm::vec3(0.2, 1, 0.2));    // Green enemy
    enemies.spawn(glm::vec3(0, 1.5, -2), glm::vec3(1, 0.2, 1));      // Magenta enemy

    // Create projection matrix with proper aspect ratio
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Input state tracking
    bool canShoot = true;
    bool enterpressed = false;
    bool lclick = false;

    // Initialize GUI system
    initializeGUI(SCR_WIDTH, SCR_HEIGHT);

    // Main game loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time for frame-independent updates
        static float lastFrame = 0.0;
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Recalculate projection matrix to handle window resizing
        projection = glm::perspective(glm::radians(60.0f), 
                                     (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Handle START MENU screen
        if (currentGameScreen == GameScreen::START_MENU) {
            glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            
            renderStartMenuScreen();
            
            // Render title text on start menu
            TextRenderer startTextRenderer(SCR_WIDTH, SCR_HEIGHT);
            startTextRenderer.RenderStartText(SCR_WIDTH, SCR_HEIGHT);
            
            // Handle button clicks
            int clicked = handleStartMenuClick();
            if (clicked == 0) {
                // Play button: initialize new game and transition to gameplay
                currentGameScreen = GameScreen::GAMEPLAY;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                
                // Reset all game values
                playerHealth = 100;
                score = 0;
                currentAmmo = 30;
                reserveMags = 0;
                partialMagAmmo = 0;
                
                // Spawn initial enemies
                enemies.clear();
                enemies.spawn(glm::vec3(-3, 1.5, -1), glm::vec3(1, 0.2, 0.2));
                enemies.spawn(glm::vec3(3, 1.5, -1), glm::vec3(0.2, 1, 0.2));
                enemies.spawn(glm::vec3(0, 1.5, -2), glm::vec3(1, 0.2, 1));
                
                // Respawn items at their starting positions
                world.regenerateItems();
            }
            if (clicked == 1) {
                // Exit button: close the application
                glfwSetWindowShouldClose(window, true);
            }
            glEnable(GL_DEPTH_TEST);
        }
        
        // Handle GAMEPLAY screen
        else if (currentGameScreen == GameScreen::GAMEPLAY) {
            // Process player input
            processInput(window);
            camera.physics(deltaTime);
            
            // Handle left mouse button shooting
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && canShoot && !lclick) {
                Shooter::fire(camera, world, enemies);
                canShoot = false;
                lclick = true;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                canShoot = true;
                lclick = false;
            }

            // Handle Enter key shooting (alternative input method)
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && canShoot && !enterpressed) {
                Shooter::fire(camera, world, enemies);
                canShoot = false;
                enterpressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
                canShoot = true;
                enterpressed = false;
            }

            // Handle pause button (P key)
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
                if (!pauseKeyPressed) {
                    currentGameScreen = GameScreen::PAUSE_MENU;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    pauseKeyPressed = true;
                    std::cout << "Game Paused\n";
                }
            } else {
                pauseKeyPressed = false;
            }

            // Clear screen with sky blue color
            glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render 3D scene
            shader3D.use();
            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 VP = projection * view;
            world.render(cubeVAO, VP, shader3D.ID);
            enemies.render(cubeVAO, VP, shader3D.ID);

            // Update game logic
            enemies.update(deltaTime, camera.position);
            enemies.attackPlayer(camera.position, playerHealth, deltaTime);
            
            // Render 2D HUD elements
            TextRenderer hudRenderer(SCR_WIDTH, SCR_HEIGHT);
            hudRenderer.RenderHUD(playerHealth, score, currentAmmo, reserveMags, SCR_WIDTH, SCR_HEIGHT);
            
            // Render crosshair overlay
            glDisable(GL_DEPTH_TEST);
            shaderCrosshair.use();
            glBindVertexArray(crosshairVAO);
            glDrawArrays(GL_LINES, 0, 4);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            // Check if player died
            if (playerHealth <= 0) {
                currentGameScreen = GameScreen::END_SCREEN;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                std::cout << "You Died! Final Score: " << score << "\n";
            }
        }
        
        // Handle PAUSE MENU screen
        else if (currentGameScreen == GameScreen::PAUSE_MENU) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            
            renderPauseMenuScreen();
            
            // Render pause text
            TextRenderer pauseTextRenderer(SCR_WIDTH, SCR_HEIGHT);
            pauseTextRenderer.RenderPauseText(SCR_WIDTH, SCR_HEIGHT);
            
            // Handle pause menu button clicks
            int clicked = handlePauseMenuClick();
            if (clicked == 0) {
                // Resume button: return to gameplay
                currentGameScreen = GameScreen::GAMEPLAY;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                pauseKeyPressed = true;
            }
            else if (clicked == 1) {
                // Restart button: reset game and start over
                currentGameScreen = GameScreen::GAMEPLAY;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                pauseKeyPressed = true;
                
                // Reset all game state
                playerHealth = 100;
                score = 0;
                currentAmmo = 30;
                reserveMags = 0;
                partialMagAmmo = 0;
                
                // Clear and respawn enemies
                enemies.clear();
                enemies.spawn(glm::vec3(-3, 1.5, -1), glm::vec3(1, 0.2, 0.2));
                enemies.spawn(glm::vec3(3, 1.5, -1), glm::vec3(0.2, 1, 0.2));
                enemies.spawn(glm::vec3(0, 1.5, -2), glm::vec3(1, 0.2, 1));
                
                // Respawn items
                world.regenerateItems();
                
                std::cout << "Game Restarted\n";
            }
            else if (clicked == 2) {
                // Main menu button: return to start menu
                currentGameScreen = GameScreen::START_MENU;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                pauseKeyPressed = false;
            }
            
            // Allow P key to resume game
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
                if (!pauseKeyPressed) {
                    currentGameScreen = GameScreen::GAMEPLAY;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    pauseKeyPressed = true;
                }
            } else {
                pauseKeyPressed = false;
            }
            
            glEnable(GL_DEPTH_TEST);
        }
        
        // Handle END SCREEN
        else if (currentGameScreen == GameScreen::END_SCREEN) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            
            renderEndMenuScreen();
            
            // Render end game text
            TextRenderer endTextRenderer(SCR_WIDTH, SCR_HEIGHT);
            bool playerWon = false;
            endTextRenderer.RenderEndText(score, playerWon, SCR_WIDTH, SCR_HEIGHT);
            
            // Handle end menu button clicks
            int clicked = handleEndMenuClick();
            if (clicked == 0) {
                // Play again button: restart game
                currentGameScreen = GameScreen::GAMEPLAY;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                playerHealth = 100;
                score = 0;
                currentAmmo = 30;
                reserveMags = 0;
                partialMagAmmo = 0;
                
                // Respawn all enemies
                enemies.clear();
                enemies.spawn(glm::vec3(-3, 1.5, -1), glm::vec3(1, 0.2, 0.2));
                enemies.spawn(glm::vec3(3, 1.5, -1), glm::vec3(0.2, 1, 0.2));
                enemies.spawn(glm::vec3(0, 1.5, -2), glm::vec3(1, 0.2, 1));
                
                // Respawn all items
                world.regenerateItems();
                
                std::cout << "Starting new game...\n";
            }
            if (clicked == 1) {
                // Main menu button
                currentGameScreen = GameScreen::START_MENU;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            
            glEnable(GL_DEPTH_TEST);
        }

        // Swap buffers and process events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up resources
    cleanupGUI();
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &crosshairVAO);
    glDeleteBuffers(1, &crosshairVBO);
    glfwTerminate();
    return 0;
}

// Handle window resize events and update viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Handle mouse movement and camera updates
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = SCR_WIDTH / 2.0f;
    static float lastY = SCR_HEIGHT / 2.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calculate mouse offset from last position
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // Update camera during gameplay only
    if (currentGameScreen == GameScreen::GAMEPLAY) {
        Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        camera->processMouse(xoffset, yoffset);
    } else {
        // Update GUI mouse for menu screens
        guiMouseCallback(window, xpos, ypos);
    }
}

// Handle keyboard input for movement and actions
void processInput(GLFWwindow* window) {
    // Close window with Escape key
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    static float lastFrame = 0.0f;
    float currentFrame = (float)glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Jump with spacebar
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
        camera->jump();

    // Reload ammunition with R key
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) 
        reload(30, reserveMags, partialMagAmmo, currentAmmo);
    
    // Movement with WASD or arrow keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        camera->processKeyboard(0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
        camera->processKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
        camera->processKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
        camera->processKeyboard(3, deltaTime);
}

// Create cube vertex array object for rendering
unsigned int createCubeVAO() {
    // Define cube vertex positions
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    // Define cube triangle indices
    unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        0,3,7, 7,4,0,
        1,2,6, 6,5,1,
        0,1,5, 5,4,0,
        3,2,6, 6,7,3
    };

    // Create and configure vertex array object
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return VAO;
}