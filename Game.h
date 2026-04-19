#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Map.h"
#include "GameState.h"
#include "Render.h"
#include "Enemy.h"
#include "Difficulty.h"
#include <string>
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    //--- core loop ---
    void input();
    void update();
    void render();

    // --- per-state input ---
    void inputMenu();
    void inputPlaying();
    void inputPaused();
    void inputGameOver();
    void inputWin();

    // --- per-state update ---
    void updateMenu();
    void updatePlaying();
    void updatePaused();
    void updateGameOver();
    void updateWin();

    // --- per-state render ---
    void renderMenu();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();
    void renderWin();

    // --- save game ---
    void inputSaving();
    void inputContinue();
    void updateSaving();
    void updateContinue();
    void renderSaving();
    void renderContinue();

    void saveGame();
    bool loadGame();

    // --- state ---
    GameState state = GameState::MENU;

    // --- gameplay ---
    std::vector<Slime> slimes;
    Player player;  
    Map map;

    bool isAttacking = false;
    int attackTimer = 0;
    int attackCD = 0;
    int attackX = 0;
    int attackY = 0;

    // --- render ---
    Render renderer;

    // --- menu ---
    static constexpr int menuOptionCount = 4;
    int menuSelection = 0;
    Difficulty difficulty = Difficulty::NORMAL;

    // --- room ---
    std::string currentRoom = "room/room0.txt";
    bool autotrans = true; // set true if want autotransition w/o pressing E and false otherwise.
    bool transition = false;

    bool tryTransition(char dir);
    void handleRoomTransition();

    struct RoomSaveData {
        int roomNumber;
        int slimesLeft;
    };
    RoomSaveData roomSaves[100];
    int numRoomSaves = 0;


};

#endif