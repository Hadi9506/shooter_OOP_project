# FPS Crosshair Shooter - OOP Project Documentation

## Project Overview
A 3D First-Person Shooter (FPS) game built with C++ using OpenGL, GLFW, GLM, and GLAD libraries. Features a menu system, gameplay mechanics with enemy AI, HUD display, and a pause/end screen system.

---
## Team Members:
- Member 1: Syed Muhammad Haider Zaidi      HU ID: mz10270
- Member 2: Syed Hadi Abbas Naqvi           HU ID: hn10381
- Member 3: Ghazi Sibtain                   HU ID: gs10403

## Instructor: Nadia Nasir
---

## Table of Contents
1. [Core Components](#core-components)
2. [Game States](#game-states)
3. [Key Features](#key-features)
4. [File Structure](#file-structure)
5. [Build Instructions](#build-instructions)
6. [Controls](#controls)

---

## Core Components

### Main Game Loop (main.cpp)
**Purpose:** Central hub managing game initialization, state transitions, and frame rendering.

**Key Global Variables:**
- `playerHealth`: Current player health (0-100)
- `score`: Accumulated score from eliminated enemies
- `ammo`: Current ammunition count (refills at 30)
- `currentGameScreen`: Active screen state (START_MENU, GAMEPLAY, PAUSE_MENU, END_SCREEN)
- `pauseKeyPressed`: Debounce flag for pause key

**Screen Resolution:**
- `SCR_WIDTH`: 1200 pixels
- `SCR_HEIGHT`: 800 pixels

### Graphics Initialization
```
1. GLFW Window Creation (1200x800)
2. OpenGL 3.3 Core Profile Setup
3. GLAD Loader Integration
4. Depth Testing Enabled
5. VSync Enabled (60 FPS cap)
```

### VAO Setup
**Cube VAO:** Stores vertex and index data for 3D world geometry
**Crosshair VAO:** 2D overlay with 4 line vertices for HUD crosshair

---

## Game States

### 1. START_MENU (GameScreen::START_MENU)
- Renders main menu screen
- Button options: START GAME, EXIT GAME
- Resets player stats (Health: 100, Score: 0, Ammo: 30)
- Spawns 3 enemies at initial positions
- Disables cursor for gameplay

### 2. GAMEPLAY (GameScreen::GAMEPLAY)
- **Player Input:** WASD/Arrow keys for movement, Space for jump, ESC to exit
- **Shooting:** Left mouse button fires projectiles
- **Pause:** P key transitions to pause menu
- **Game Over Conditions:**
  - Player health ≤ 0 → END_SCREEN (loss)
  - (Commented) All enemies eliminated → END_SCREEN (win)

**Frame Operations:**
1. Process keyboard/mouse input
2. Update camera physics (gravity, movement)
3. Handle firing with ammo tracking
4. Render 3D world and enemies
5. Update enemy AI and attack detection
6. Render HUD text (health, score, ammo)
7. Render 2D crosshair overlay
8. Check game-over conditions

### 3. PAUSE_MENU (GameScreen::PAUSE_MENU)
- Pauses all game logic
- Button options: RESUME, MAIN MENU
- P key can also resume gameplay
- Cursor enabled for menu interaction

### 4. END_SCREEN (GameScreen::END_SCREEN)
- Displays final game result and score
- Button options: PLAY AGAIN, MAIN MENU
- Resets stats and respawns enemies on restart

---

## Key Features

### Camera & Player Control
- **Class:** `Camera` (Camera.h/Camera.cpp)
- Movement: Forward/Backward/Left/Right with physics
- Jump mechanic with gravity
- Mouse look with configurable sensitivity
- First-person perspective

### Enemy System
- **Class:** `EnemyManager` (Enemy.h/Enemy.cpp)
- Spawns 3 distinct enemies with colored cubes (Red, Green, Magenta)
- Spawned at: (-3, 1.5, -1), (3, 1.5, -1), (0, 1.5, -2)
- **AI Features:**
  - Position updates per frame
  - Attack range detection
  - Player damage dealing
  - Death on being shot

### Shooting Mechanics
- **Class:** `Shooter` (Shooter.h/Shooter.cpp)
- Fire method: `Shooter::fire(camera, world, enemies)`
- Raycast-based projectile from camera position
- Ammo counter: 0-30 (refills when depleted)
- Shooting disabled while reloading

### World Rendering
- **Class:** `World` (World.h/World.cpp)
- Generates static 3D environment
- Render method takes VAO, view-projection matrix, shader ID
- Collision detection with enemy firing

### Shader System
- **Class:** `Shader` (Shader.h/Shader.cpp)
- 3D Rendering: `basic.vert` + `basic.frag`
- 2D Crosshair: `crosshair.vert` + `crosshair.frag`
- Shaders located in `resources/`

### Text Rendering & HUD
- **Class:** `TextRenderer` (TextRenderer.h/TextRenderer.cpp)
- Method: `RenderHUD(health, score, ammo, width, height)`
- Displays real-time player stats on screen
- Uses font rendering from `stb_truetype.h`

### GUI System
- **File:** `GUI/main_gui.h/cpp`
- Functions:
  - `initializeGUI()`: Setup GUI system
  - `renderStartMenuScreen()`: Draw start menu
  - `handleStartMenuClick()`: Process button clicks (return: 0=START, 1=EXIT)
  - `renderPauseMenuScreen()`: Draw pause menu
  - `handlePauseMenuClick()`: Process pause menu clicks (return: 0=RESUME, 1=MAIN_MENU)
  - `renderEndMenuScreen()`: Draw end screen
  - `handleEndMenuClick()`: Process end screen clicks (return: 0=PLAY_AGAIN, 1=MAIN_MENU)
  - `guiClickCallback()`: Mouse click handler
  - `guiMouseCallback()`: Mouse movement handler for menus
  - `cleanupGUI()`: Release GUI resources

---

## File Structure

```
shooter_OOP_project-main/
├── include/                    # External libraries
│   ├── glad/                   # OpenGL loader
│   ├── GLFW/                   # Window management
│   ├── glm/                    # Math library
│   ├── KHR/                    # Khronos headers
│   └── stb_truetype.h          # Font rendering
├── lib/                        # Compiled libraries
├── resources/                  # Shader files
│   ├── basic.vert/frag         # 3D rendering shaders
│   └── crosshair.vert/frag     # 2D crosshair shaders
├── src/                        # Source code
│   ├── main.cpp                # Main game loop
│   ├── Camera.h/cpp            # Player camera control
│   ├── Enemy.h/cpp             # Enemy spawning & AI
│   ├── Shooter.h/cpp           # Projectile/raycast firing
│   ├── World.h/cpp             # World generation
│   ├── Shader.h/cpp            # Shader management
│   ├── TextRenderer.h/cpp      # HUD text rendering
│   ├── Item.h/cpp              # Item system (if used)
│   ├── glad.c                  # GLAD implementation
│   └── GUI/                    # Menu system
│       ├── main_gui.h/cpp
│       ├── start_Screen.h/cpp
│       ├── pause_Screen.h/cpp
│       ├── end_Screen.h/cpp
│       └── startScreen.cpp
└── readme.txt                  # This file
```

---

## Build Instructions

### Prerequisites
- Windows 10/11
- MinGW with g++ (C++17 support)
- GLFW3 DLL library
- OpenGL-compatible GPU

### Build Command
```bash
g++.exe -g -std=c++17 -I./include -L./lib ^
  ./src/main.cpp ./src/glad.c -lglfw3dll ^
  -o ./cutable.exe
```

### Compilation Flags
- `-g`: Debug symbols
- `-std=c++17`: C++ standard
- `-I./include`: Include directory paths
- `-L./lib`: Library directory paths
- `-lglfw3dll`: Link GLFW3 library

### Running the Game
```bash
./cutable.exe
```

---

## Controls

### Movement
| Key   | Action        |
|-------|---------------|
| W / ↑ | Move Forward  |
| S / ↓ | Move Backward |
| A / ← | Strafe Left   |
| D / → | Strafe Right  |
| Space | Jump          |
| ESC   | Exit Game     |

### Combat
| Input     | Action                      |
|-----------|-----------------------------|
| Mouse Move| Look Around (gameplay only) |
| Left Click| Fire Weapon                 |
| P         | Pause/Resume                |
### Menu Navigation
| Input     | Action                      |
|-----------|-----------------------------|
| Mouse Move| Hover over buttons          |
| Left Click| Select button               |

---

## Game Mechanics

### Ammo System
- Start with 30 ammo per round
- One ammo consumed per shot
- Ammo automatically refills to 30 when depleted
- Ammo counter displayed in HUD

### Health System
- Player starts with 100 HP
- Enemies deal damage based on proximity and attack timer
- Game ends when health reaches 0

### Score System
- Points awarded for each enemy eliminated
- Score persists across deaths
- Final score displayed on end screen

### Depth Testing & Rendering Order
```
1. 3D World Render (depth enabled)
2. Enemy Models (depth enabled)
3. Text/HUD (depth disabled, orthographic)
4. Crosshair Overlay (depth disabled, 2D quad rendering)
```

### Lighting & Materials
- Single light source (world-based)
- Colored enemy cubes for visual distinction
- Basic fragment shaders for color output

---

## Known Issues & TODOs

### Commented Features
- Enemy victory condition (all enemies dead) - currently commented out
- Enemy clearing function - needs implementation

### Potential Improvements
1. Add sound effects and background music
2. Implement weapon switching system
3. Add particle effects for gunfire/hits
4. Implement health/ammo pickups
5. Add level progression
6. Optimize enemy count scaling
7. Add difficulty settings

---

## Dependencies

| Library      | Version   | Purpose                     |
|--------------|-----------|-----------------------------|
| GLFW         | 3.x       | Window & input management   |
| GLAD         | Generated | OpenGL function loading     |
| GLM          | Latest    | 3D math (matrices, vectors) |
| stb_truetype | Latest    | Font rendering              |

---

## Author Notes

This project demonstrates core game development concepts:
- Object-Oriented Programming principles
- OpenGL rendering pipeline (3D + 2D blending)
- Game state machines
- Input handling and event callbacks
- 3D camera control and perspective
- Enemy AI and collision detection
- HUD text rendering

---
:)
