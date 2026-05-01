#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Map.h"
#include "GameState.h"
#include "Render.h"
#include "Enemy.h"
#include "Difficulty.h"
#include "Cutscene.h"
#include "Outro.h"
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h> 
#include <cstdlib>
#include <random>
#include <utility>
#include <fstream> 

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    //--- core loop ---
    void input();
    void update();
    void render();

    // --- per-state input ---
    void inputStartMenu();
    void inputDifficultyMenu();
    void inputPlaying();
    void inputPaused();
    void inputGameOver();
    void inputWin();

    // --- per-state update ---
    void updateStartMenu();
    void updateDifficultyMenu();
    void updatePlaying();
    void updatePaused();
    void updateGameOver();
    void updateWin();

    // --- per-state render ---
    void renderStartMenu();
    void renderDifficultyMenu();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();
    void renderWin();

    // --- save game ---
    void inputSaving();
    void updateSaving();
    void renderSaving();

    void startNewGame(Difficulty selectedDifficulty);
    void resetGameSession();
    bool hasSaveFile() const;
    void saveGame();
    bool loadGame();

    // --- state ---
    GameState state = GameState::START_MENU;

    // --- gameplay ---
    std::vector<Slime> slimes;
    Boss* boss = nullptr;  // nullptr when not in boss room
    Player player;  
    Map map;
    std::vector<Heart> hearts;

    bool isAttacking = false;
    int attackTimer = 0;
    int attackCD = 0;
    int attackX = 0;
    int attackY = 0;
    int dmgTimer = 0;
    int slimeDmgTimer = 0;
    
    // --- render ---
    Render renderer;

    // --- menu ---
    static constexpr int startMenuOptionCount = 3; // New/ Continue/ Quit
    static constexpr int difficultyMenuOptionCount = 5; // Easy/ Normal/ Hard / Back / Explore
    int startMenuSelection = 0;
    int difficultyMenuSelection = 1; // Default to Normal
    Difficulty difficulty = Difficulty::NORMAL;

    // --- room ---
    std::string currentRoom = "room/room0.txt";
    bool autotrans = true; // set true if want autotransition w/o pressing E and false otherwise.
    bool transition = false;
    bool roomLocked = false;

    bool tryTransition(char dir);
    void handleRoomTransition(int dx, int dy);

    struct RoomSaveData {
        int roomNumber;
        int slimesLeft;
    };
    RoomSaveData roomSaves[100];
    int numRoomSaves = 0;


};

#endif