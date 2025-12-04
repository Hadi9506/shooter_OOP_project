## Project Overview
A 3D First-Person Shooter (FPS) game written in C++ using OpenGL, GLFW, GLM and GLAD. The project includes a menu system, gameplay with simple enemy AI, HUD rendering, a weapon system (with magazine/reserve logic), tracer effects, and pause/end screens.

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

### Main Game Loop (`main.cpp`)
Purpose: initializes the renderer, handles the main game loop, manages screen states (menu, gameplay, pause), and dispatches rendering and update calls each frame.

Key global variables (as present in `main.cpp`):
- `playerHealth` : current player HP (0–100)
- `score` : player score
- `currentAmmo` : bullets in the currently loaded magazine
- `reserveMags` : number of full spare magazines
- `partialMagAmmo` : leftover bullets from an ejected mag
- `TracerManager tracerManager` : global tracer/visual bullet-effect manager
- `WeaponSystem ws` : global weapon system handling switching/reloading
- `currentGameScreen` : active screen enum (START_MENU, GAMEPLAY, PAUSE_MENU, END_SCREEN)
- `pauseKeyPressed` : simple debounce flag for the P key

Screen resolution constants:
- `SCR_WIDTH` = 1200
- `SCR_HEIGHT` = 800

### Graphics Initialization
1. GLFW window creation (1200×800)
2. OpenGL 3.3 Core Profile
3. GLAD loader initialization
4. Depth testing enabled
5. VSync via `glfwSwapInterval(1)`

### VAO Setup
- Cube VAO: vertex + index buffers for 3D world cubes
- Crosshair VAO: simple 2D VAO with 4 vertices used for drawing two crosshair lines

---

## Game States

### START_MENU (`GameScreen::START_MENU`)
- Renders the start menu UI via the GUI system
- Buttons include PLAY, SETTINGS, EXIT
- When PLAY is chosen the game transitions to `GAMEPLAY`, cursor is disabled and several player stats are reset in code: `playerHealth = 100`, `score = 0`, and `currentAmmo` is set from code (in `main.cpp` the start path sets `currentAmmo = 31`).
- The code spawns multiple enemies at startup (see implementation: a loop spawns 7 enemies).

### GAMEPLAY (`GameScreen::GAMEPLAY`)
- Player input: WASD / Arrow keys for movement, Space to jump, ESC to exit
- Shooting: Left mouse button (and Enter) call `Shooter::fire(...)`
- Weapon switching: keys `1`, `2`, `3` to switch weapon slots (handled by `WeaponSystem`)
- Reload: `R` triggers a reload via the `WeaponSystem` (reload helper and magazine logic are implemented)
- Pause: `P` opens pause menu

Frame update sequence (high-level):
1. Process input
2. Update camera physics
3. Update tracer effects
4. Render 3D world and enemies (depth testing enabled)
5. Render HUD (depth disabled)
6. Render 2D crosshair overlay
7. Check player health / game-over conditions

### PAUSE_MENU (`GameScreen::PAUSE_MENU`)
- Pauses gameplay updates and allows menu interaction
- Buttons: RESUME, MAIN MENU
- `P` can also resume gameplay

### END_SCREEN (`GameScreen::END_SCREEN`)
- Shows final score and result
- Options: PLAY AGAIN, MAIN MENU

---

## Key Features

### Camera & Player Control
- Class: `Camera` (`Camera.h/cpp`)
- First-person camera: mouse look, configurable sensitivity, physics-based movement and jump

### Enemy System
- Class: `EnemyManager` (`Enemy.h/cpp`)
- The current code spawns multiple enemies in a loop (7 in the provided `main.cpp` snippet) at distinct positions and updates their positions and attacks each frame

### Shooting & Weapon System
- Class: `Shooter` (`Shooter.h/cpp`) — handles firing via raycasts and spawning tracer/bullet effects
- `WeaponSystem` (`WeaponSystem.h/cpp`) — handles weapon switching and reloading. Keys `1`/`2`/`3` switch weapons, `R` reloads, and `Shooter::fire` is used to shoot.
- Global variables used for ammo state: `currentAmmo`, `reserveMags`, `partialMagAmmo` plus an inline reload helper in code that refills the current magazine using partial ammo first then reserve mags.

### Tracers & Visuals
- `TracerManager` collects and renders tracer effects; updated and rendered each frame using the 3D shader. This provides visual bullet traces for fired shots.

### World Rendering
- Class: `World` (`World.h/cpp`) — procedural/static generation of cube-based world geometry; `render` accepts VAO, view-projection, and shader id.

### Shader System
- Class: `Shader` (`Shader.h/cpp`)
- 3D rendering uses `resources/basic.vert` and `resources/basic.frag`
- 2D crosshair uses `resources/crosshair.vert` and `resources/crosshair.frag`

### Text Renderer & HUD
- Class: `TextRenderer` (`TextRenderer.h/cpp`)
- Method: `RenderHUD(health, score, currentAmmo, reserveMags, width, height)` — displays health, score and ammo/mag info on-screen

### GUI System
- `GUI/main_gui.h/cpp` — handles menu rendering, mouse callbacks and button handling
- Functions provided by the GUI: `initializeGUI`, `renderStartMenuScreen`, `handleStartMenuClick`, `renderPauseMenuScreen`, `handlePauseMenuClick`, `renderEndMenuScreen`, `handleEndMenuClick`, `guiClickCallback`, `guiMouseCallback`, and `cleanupGUI`

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
├── lib/                        # Compiled libraries (GLFW DLLs, etc.)
├── resources/                  # Shader files
│   ├── basic.vert
│   ├── basic.frag
│   ├── crosshair.vert
│   └── crosshair.frag
├── src/                        # Source code
│   ├── main.cpp                # Main game loop (current version includes WeaponSystem, TracerManager, reload helper)
│   ├── Camera.h/cpp
│   ├── Enemy.h/cpp
│   ├── Shooter.h/cpp
│   ├── World.h/cpp
│   ├── Shader.h/cpp
│   ├── TextRenderer.h/cpp
│   ├── WeaponSystem.h         # Weapon switching / reload management
│   ├── Bullet.h/cpp
│   ├── tracer.cpp/h           # Tracer manager and effects
│   ├── glad.c                  # GLAD implementation
│   └── GUI/
│       ├── main_gui.h/cpp
│       ├── start_Screen.h/cpp
│       ├── pause_Screen.h/cpp
│       └── end_Screen.h/cpp
└── readme.txt                  # This file
```

---

## Build Instructions

### Prerequisites
- Windows 10/11
- MinGW / g++ with C++17 support (or other compatible compiler)
- GLFW3 DLL available on your PATH or in the executable directory
- OpenGL-capable GPU and drivers

### Example Build Command (MinGW / PowerShell)
```powershell
g++.exe -g -std=c++17 -I./include -L./lib \\
  ./src/main.cpp ./src/glad.c -lglfw3dll \\
  -o ./cutable.exe
```

### Compilation Flags
- `-g` : debug symbols
- `-std=c++17` : standard
- `-I./include` : header includes
- `-L./lib` : library path
- `-lglfw3dll` : link GLFW (DLL import)

### Running the Game
```powershell
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
| Input      | Action                                                               |
|------------|----------------------------------------------------------------------|
| Mouse Move | Look Around (gameplay only)                                          |
| Left Click | Fire Weapon                                                          |
| Enter      | Alternate fire trigger (also calls `Shooter::fire` in provided code) |
| 1, 2, 3    | Switch weapon slots (handled by `WeaponSystem`)                      |
| R          | Reload current weapon (uses `WeaponSystem::reloadCurrent` in code)   |
| P          | Pause / resume                                                       |

### Menu Navigation
| Input      | Action                      |
|------------|-----------------------------|
| Mouse Move | Hover over buttons          |
| Left Click | Select button               |

---

## Game Mechanics

### Ammo System (updated)
- The code maintains magazine-aware ammo state via three variables:
  - `currentAmmo` : bullets in the currently loaded magazine
  - `reserveMags` : number of full spare magazines available
  - `partialMagAmmo` : any leftover bullets from an ejected mag
- A small reload helper in `main.cpp` demonstrates the approach:
  1. Use `partialMagAmmo` first to top up the current magazine
  2. If still needed, consume one full magazine from `reserveMags` and fill the remainder
  3. If that mag wasn't fully used, leftover bullets become `partialMagAmmo`
- `WeaponSystem` exposes `switchWeapon(int)` and `reloadCurrent()` in the current code; `R` triggers reload, keys `1`/`2`/`3` switch weapons.

### Health System
- Player starts with 100 HP
- Enemies damage the player based on proximity and attack logic
- If `playerHealth <= 0` the game returns to the menu / game over flow

### Score System
- Eliminating enemies increments `score`
- Score is displayed on the end screen

### Rendering Order
1. 3D world and enemy models (depth enabled)
2. Tracer rendering
3. HUD / text (depth disabled)
4. Crosshair overlay (depth disabled; drawn with a 2D shader)

---

## Known Issues & TODOs

### Noted / Code observations
- The code sets `currentAmmo = 31` when transitioning from the START_MENU to GAMEPLAY; the top-level variable is initialized to `30`. This may be intentional or an off-by-one artifact — consider standardizing the starting magazine size.
- Enemy victory (all enemies defeated → win) is currently commented out in places; consider re-enabling or implementing an explicit win condition and an enemy clearing function.

### Potential Improvements
1. Add sound effects and background music
2. Polished weapon switching GUI and per-weapon ammo counts
3. Particle effects for gunfire and hits
4. Health/ammo pickup items
5. Level progression and difficulty scaling
6. Optimize enemy count and spawn timing

---

## Dependencies

| Library      | Version   | Purpose                     |
|--------------|-----------|-----------------------------|
| GLFW         | 3.x       | Window & input management   |
| GLAD         | generated | OpenGL function loading     |
| GLM          | header-only | Math (vectors, matrices) |
| stb_truetype | header-only | Bitmap / font rendering   |

---

## Author Notes

This project demonstrates practical game dev concepts using C++ and OpenGL:
- Object-Oriented structure for game systems
- A simple weapon / reload model with magazine and reserve logic
- Tracer visual effects for fired shots
- Game state management (menu, gameplay, pause, end)
- HUD rendering and simple GUI interactions

---
:)
