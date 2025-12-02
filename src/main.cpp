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


int playerHealth = 100;
int score = 0;
int currentAmmo = 30;       // Ammo in currently loaded mag
int reserveMags = 0;         // Number of spare mags
int partialMagAmmo = 0;      // Ammo in ejected mag


//reload helper 
void reload(int mag_size,int &reserved_mags,int &partial_ammos,int &current_ammos){
        // Calculate how many bullets needed to fill current mag
        int needed = mag_size - current_ammos;
        // If mag is already full: do nothing
        if (needed == 0) return;
        // 1. Use partial ammo first
        int use_partial = (partial_ammos < needed) ? partial_ammos : needed;;
        current_ammos += use_partial;
        partial_ammos -= use_partial;
        needed -= use_partial;

        // 2. If still not full, use full reserve magazines
        if (needed > 0 && reserved_mags > 0) {
            // Take 1 full mag
            reserved_mags--;
            // Fill as much as needed from that full mag
            int give = (mag_size < needed) ? mag_size : needed;;
            current_ammos += give;
            needed -= give;
            // If mag wasn’t fully used, leftover becomes partial ammo
            if (give < mag_size) {
                partial_ammos += (mag_size - give);
            }
        }
    }


// Forward declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
unsigned int createCubeVAO();

int SCR_WIDTH = 1200;
int SCR_HEIGHT = 800;

// Game state
GameScreen currentGameScreen = GameScreen::START_MENU;
bool pauseKeyPressed = false;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OOP Project", nullptr, nullptr);
    if (!window) { std::cerr << "Failed GLFW\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed GLAD\n"; return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, guiClickCallback);

    Camera camera(glm::vec3(0.0f, 2.0f, 5.0f));
    glfwSetWindowUserPointer(window, &camera);

    // MAIN 3D SHADER
    Shader shader3D("resources/basic.vert", "resources/basic.frag");
    
    // NEW: CROSSHAIR 2D SHADER
    Shader shaderCrosshair("resources/crosshair.vert", "resources/crosshair.frag");

    unsigned int cubeVAO = createCubeVAO();

    // CROSSHAIR VAO (2D positions)
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

    World world;
    world.generate();

    EnemyManager enemies;
    enemies.spawn(glm::vec3(-3,1.5,-1), glm::vec3(1,0.2,0.2));  // RED
    enemies.spawn(glm::vec3( 3,1.5,-1), glm::vec3(0.2,1,0.2));  // GREEN
    enemies.spawn(glm::vec3( 0,1.5,-2), glm::vec3(1,0.2,1));    // MAGENTA

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 
                                            (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 100.0f);

    bool canShoot = true;
    bool enterpressed = false;
    bool lclick = false;

    // Initialize GUI
    initializeGUI(SCR_WIDTH, SCR_HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        static float lastFrame = 0.0;
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime    = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // SCREEN LOGIC
        if (currentGameScreen == GameScreen::START_MENU) {
            renderStartMenuScreen();
            
            int clicked = handleStartMenuClick();
            if (clicked == 0) {
                // PLAY button clicked
                currentGameScreen = GameScreen::GAMEPLAY;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                playerHealth = 100;
                score = 0;
                currentAmmo = 30;
            }
            if (clicked == 1) {
                // SETTINGS button clicked
                std::cout << "Settings clicked\n";
            }
            if (clicked == 2) {
                // EXIT button clicked
                glfwSetWindowShouldClose(window, true);
            }
        }
        else if (currentGameScreen == GameScreen::GAMEPLAY) {
            processInput(window);
            camera.physics(deltaTime);
            
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && canShoot && !lclick) {
                Shooter::fire(camera, world, enemies);
                canShoot = false;
                lclick = true;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
                canShoot = true;
                lclick = false;
            }


            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && canShoot && !enterpressed) {
                Shooter::fire(camera, world, enemies);
                canShoot = false;
                enterpressed = true;
            }
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE){
                canShoot = true;
                enterpressed = false;
            }

            // PAUSE LOGIC - P KEY
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

            glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 3D RENDERING
            shader3D.use();
            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 VP = projection * view;
            world.render(cubeVAO, VP, shader3D.ID);
            enemies.render(cubeVAO, VP, shader3D.ID);

            enemies.update(deltaTime, camera.position);  // MOVEMENT
            enemies.attackPlayer(camera.position, playerHealth, deltaTime);
            
            TextRenderer hudRenderer(SCR_WIDTH, SCR_HEIGHT);
            hudRenderer.RenderHUD(playerHealth, score, currentAmmo,reserveMags, SCR_WIDTH, SCR_HEIGHT);
            
            // CROSSHAIR (2D OVERLAY)
            glDisable(GL_DEPTH_TEST);
            shaderCrosshair.use();  //2D SHADER
            glBindVertexArray(crosshairVAO);
            glDrawArrays(GL_LINES, 0, 4);  // 2 lines (4 vertices)
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            // Check game over
            if (playerHealth <= 0) {
                currentGameScreen = GameScreen::START_MENU;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        else if (currentGameScreen == GameScreen::PAUSE_MENU) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            
            // Resume on P press
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
                if (!pauseKeyPressed) {
                    currentGameScreen = GameScreen::GAMEPLAY;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    pauseKeyPressed = true;
                    std::cout << "Game Resumed\n";
                }
            } else {
                pauseKeyPressed = false;
            }
            
            // ESC to go back to menu
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                currentGameScreen = GameScreen::START_MENU;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    cleanupGUI();
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &crosshairVAO);
    glDeleteBuffers(1, &crosshairVBO);
    glfwTerminate();
    return 0;
}

// ------------------- CALLBACKS & createCubeVAO (KEEP EXACTLY AS YOU HAVE) -------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = SCR_WIDTH / 2.0f;
    static float lastY = SCR_HEIGHT / 2.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // Only update camera when in gameplay and cursor is disabled
    if (currentGameScreen == GameScreen::GAMEPLAY) {
        Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        camera->processMouse(xoffset, yoffset);
    } else {
        // Update GUI mouse for menu screens
        guiMouseCallback(window, xpos, ypos);
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    static float lastFrame = 0.0f;
    float currentFrame = (float)glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //jump 
    if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS) camera->jump();

    //reload logic 
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) 
        reload(30,reserveMags,partialMagAmmo,currentAmmo);
    
    //move with both wasd or up,down,left,right
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) camera->processKeyboard(0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) camera->processKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS) camera->processKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS) camera->processKeyboard(3, deltaTime);
}

unsigned int createCubeVAO() {
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

    unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        0,3,7, 7,4,0,
        1,2,6, 6,5,1,
        0,1,5, 5,4,0,
        3,2,6, 6,7,3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return VAO;
}