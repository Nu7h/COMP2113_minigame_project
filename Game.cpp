#include "Game.h"
#include <chrono>
#include <thread>
#include <unistd.h> 
#include <cstdlib>
#include <random>
#include <vector>
#include <utility>
#include <fstream> 

namespace {

std::mt19937& gameRng(){
    static std::mt19937 gen{std::random_device{}()};
    return gen;
}


void placeSlimeRandom(Slime& slime, const Map& map, int avoidX, int avoidY, const std::vector<Slime>& existingSlimes){
    std::vector<std::pair<int, int>> cells;
    cells.reserve(static_cast<std::size_t>(map.getWidth() * map.getHeight()));
    for(int yy = 0; yy < map.getHeight(); ++yy){
        for(int xx = 0; xx < map.getWidth(); ++xx){
            if(!map.isWalkable(xx, yy) || (xx == avoidX && yy == avoidY))
                continue;

            bool occupied = false;
            for (const auto& other : existingSlimes){
                if(other.x == xx && other.y == yy){
                    occupied = true;
                    break;
                }
            }
            if(!occupied){
                cells.emplace_back(xx, yy);
            }
        }
    }
    if(cells.empty())
        return;
    std::uniform_int_distribution<std::size_t> dist(0, cells.size() - 1);
    const auto& pick = cells[dist(gameRng())];
    slime.x = pick.first;
    slime.y = pick.second;
}

void spawnSlimes_Count(std::vector<Slime>& slimes, const Map& map, int px, int py, Difficulty difficulty, int count){
    slimes.clear();
    int slimeSpeed = 10;
    if( difficulty == Difficulty::NORMAL){
        slimeSpeed = 8;
    }else if( difficulty == Difficulty::HARD){
        slimeSpeed = 6;
    }
    for (int i = 0; i < count; i++){
        Slime s;
        s.setmaxmoveCooldown(slimeSpeed);
        placeSlimeRandom(s, map, px, py, slimes);
        slimes.push_back(s);
    }
}

void spawnSlimes_Difficulty(std::vector<Slime>& slimes, const Map& map, int px, int py, Difficulty difficulty){
    int numberSlimes = 1;
    if( difficulty == Difficulty::NORMAL){
        numberSlimes = 2;
    }else if( difficulty == Difficulty::HARD){
        numberSlimes= 3;
    }
    spawnSlimes_Count(slimes, map, px, py, difficulty, numberSlimes);
}

} // namespace

static const int KEY_UP    = 1000;
static const int KEY_DOWN  = 1001;
static const int KEY_RIGHT = 1002;
static const int KEY_LEFT  = 1003;

void Game::run(){
    using clock = std::chrono::steady_clock;
    using ms    = std::chrono::milliseconds;
    auto next = clock::now();

    while(true){
        input();
        update();
        render();

        next += ms(50);
        std::this_thread::sleep_until(next);
    }
}

static int readKey() {
    unsigned char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;  // -1 = no key

    if (c == '\033') {
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\033';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\033';
        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
        return '\033';
    }
    return (int)c;
}

bool Game::hasSaveFile() const {
    std::ifstream f("save.dat");
    return f.good();
}

void Game::resetGameSession() {
    player = Player();
    currentRoom = "room/room0.txt";
    map.loadFromFile(currentRoom);

    slimes.clear();

    if (boss != nullptr) {
        delete boss;
        boss = nullptr;
    }

    isAttacking = false;
    attackTimer = 0;
    attackCD = 0;
    attackX = 0;
    attackY = 0;

    transition = false;
    numRoomSaves = 0;
}

void Game::startNewGame(Difficulty selectedDifficulty) {
    resetGameSession();
    difficulty = selectedDifficulty;

    spawnSlimes_Difficulty(slimes, map, player.getX(), player.getY(), difficulty);

    int rNum = 0;
    if (sscanf(currentRoom.c_str(), "room/room%d.txt", &rNum) == 1) {
        roomSaves[0].roomNumber = rNum;
        roomSaves[0].slimesLeft = static_cast<int>(slimes.size());
        numRoomSaves = 1;
    }

    state = GameState::PLAYING;
}

void Game::input(){
    switch(state){
        case GameState::START_MENU:     inputStartMenu();    break;
        case GameState::DIFFICULTY_MENU: inputDifficultyMenu(); break;
        case GameState::PLAYING:  inputPlaying(); break;
        case GameState::PAUSED:   inputPaused();  break;
        case GameState::GAME_OVER:inputGameOver();break;
        case GameState::WIN:      inputWin();     break;
        case GameState::SAVING:    inputSaving();   break;
    }
}

void Game::inputStartMenu() {
    int ch = readKey();

    switch (ch) {
    case 'w': case 'W':
    case KEY_UP:
        startMenuSelection =
            (startMenuSelection - 1 + startMenuOptionCount) % startMenuOptionCount;
        break;

    case 's': case 'S':
    case KEY_DOWN:
        startMenuSelection =
            (startMenuSelection + 1) % startMenuOptionCount;
        break;

    case '\n':
    case ' ':
        if (startMenuSelection == 0) {
            difficultyMenuSelection = 1;  // default to Normal
            state = GameState::DIFFICULTY_MENU;
        } else if (startMenuSelection == 1) {
            if (hasSaveFile()) {
                if (loadGame()) {
                    state = GameState::PLAYING;
                }
            }
        } else if (startMenuSelection == 2) {
            exit(0);
        }
        break;
    }
}

void Game::inputDifficultyMenu() {
    int ch = readKey();

    switch (ch) {
    case 'w': case 'W':
    case KEY_UP:
        difficultyMenuSelection =
            (difficultyMenuSelection - 1 + difficultyMenuOptionCount) % difficultyMenuOptionCount;
        break;

    case 's': case 'S':
    case KEY_DOWN:
        difficultyMenuSelection =
            (difficultyMenuSelection + 1) % difficultyMenuOptionCount;
        break;

    case '\n':
    case ' ':
        if (difficultyMenuSelection == 0) {
            startNewGame(Difficulty::EASY);
        } else if (difficultyMenuSelection == 1) {
            startNewGame(Difficulty::NORMAL);
        } else if (difficultyMenuSelection == 2) {
            startNewGame(Difficulty::HARD);
        } else if (difficultyMenuSelection == 3) {
            state = GameState::START_MENU;
        }
        break;
    }
}

void Game::inputPlaying(){
    int ch = readKey();

    int dx = 0, dy = 0;
    bool moved = false;

    switch(ch){
        case 'q': case 'Q': state = GameState::SAVING; break;
        case 'p': case 'P': state = GameState::PAUSED; break;
        case 'w': case 'W': dx =  0; dy = -1; moved = true; break;
        case 's': case 'S': dx =  0; dy =  1; moved = true; break;
        case 'a': case 'A': dx = -1; dy =  0; moved = true; break;
        case 'd': case 'D': dx =  1; dy =  0; moved = true; break;
        case 'e': case 'E': transition = true; break;
        case 'c': case 'C':
            player.activateShield();
            break;
        case ' ':
            if(!isAttacking && attackCD <= 0){
                int ax = player.getLastX() + player.getX();
                int ay = player.getLastY() + player.getY();

                if(map.isWalkable(ax, ay)){
                    isAttacking = true;
                    attackTimer = 5;
                    attackCD = 10;
                    attackX = ax;
                    attackY = ay;
                }
            }
            break;
    }
    
    if(moved && !player.isShielding){
    player.move(dx, dy, map);
    handleRoomTransition();
    }

}

void Game::handleRoomTransition(){
    int x = player.getX();
    int y = player.getY();
    int w = map.getWidth();
    int h = map.getHeight();

    if(y == 0){
        if(x <= (w/2) + 1 && x >= (w/2) - 2 && (transition || autotrans)) tryTransition('N');
    } else if(x == w - 1){
        if(y <= (h/2) && y >= (h/2) - 1 && (transition || autotrans)) tryTransition('E');
    } else if(y == h - 1){
        if(x <= (w/2) + 1 && x >= (w/2) - 2 && (transition || autotrans)) tryTransition('S');
    } else if(x == 0){
        if(y <= (h/2) && y >= (h/2) - 1 && (transition || autotrans)) tryTransition('W');
    }

    transition = false;
}

bool Game::tryTransition(char dir){
    std::string next = map.getNeighbor(dir);
    if(next == "0") return false;

    std::string next_path = "room/" + next;
    if(!map.loadFromFile(next_path)) return false;

    currentRoom = next_path;

    // Clean up old boss if exists
    if (boss != nullptr) {
        delete boss;
        boss = nullptr;
    }

    int w = map.getWidth();
    int h = map.getHeight();

    if(dir == 'N') player.y = h - 1;
    else if(dir == 'E') player.x = 0;
    else if(dir == 'S') player.y = 0;
    else if(dir == 'W') player.x = w - 1;

    int rNum = 0;
    if (sscanf(currentRoom.c_str(), "room/room%d.txt", &rNum) == 1) {
        // Special handling for room 3 - spawn boss instead of slimes
        if (rNum == 3) {
            slimes.clear();
            boss = new Boss();
            boss->x = w / 2;
            boss->y = h / 2;
        } else {
            // Regular slime spawning for other rooms
            int savedSlimes = -1;
            for(int i = 0; i < numRoomSaves; i++) {
                if(roomSaves[i].roomNumber == rNum) {
                    savedSlimes = roomSaves[i].slimesLeft;
                    break;
                }
            }

            if (savedSlimes == -1) {
                spawnSlimes_Difficulty(slimes, map, player.getX(), player.getY(), difficulty);
                if (numRoomSaves < 100) {
                    roomSaves[numRoomSaves].roomNumber = rNum;
                    roomSaves[numRoomSaves].slimesLeft = slimes.size();
                    numRoomSaves++;
                }
            } else {
                spawnSlimes_Count(slimes, map, player.getX(), player.getY(), difficulty, savedSlimes);
            }
        }
    } else {
        spawnSlimes_Difficulty(slimes, map, player.getX(), player.getY(), difficulty);
    }

    return true;
}

void Game::inputPaused(){
    int ch = readKey();
    switch(ch){
        case 'p': case 'P': state = GameState::PLAYING; break;
        case 'q': case 'Q': state = GameState::SAVING; break;
    }
}

void Game::inputGameOver(){
    int ch = readKey();
    if(ch == ' ' || ch == '\n'){
        player = Player();
        currentRoom = "room/room0.txt";
        map.loadFromFile(currentRoom);
        slimes.clear();
        if (boss != nullptr) {
            delete boss;
            boss = nullptr;
        }
        numRoomSaves = 0;
        isAttacking   = false;
        attackTimer   = 0;
        attackCD = 0;
        startMenuSelection  = 0;
        state = GameState::START_MENU;
    }
}

void Game::inputWin(){
    int ch = readKey();
    if(ch == '\n' || ch == ' '){
        player = Player();
        currentRoom = "room/room0.txt";
        map.loadFromFile(currentRoom);
        slimes.clear();
        if (boss != nullptr) {
            delete boss;
            boss = nullptr;
        }
        numRoomSaves = 0;
        startMenuSelection = 0;
        state = GameState::START_MENU;
    }
}

// ===============
//     UPDATE
// ===============

void Game::update(){
    switch(state){
        case GameState::START_MENU:     updateStartMenu();     break;
        case GameState::DIFFICULTY_MENU: updateDifficultyMenu(); break;
        case GameState::PLAYING:  updatePlaying();  break;
        case GameState::PAUSED:   updatePaused();   break;
        case GameState::GAME_OVER:updateGameOver(); break;
        case GameState::WIN:      updateWin();      break;
        case GameState::SAVING:    break;
    }
}

bool Game::loadGame() {
    std::ifstream f("save.dat");
    if (!f) return false;
    int px, py, hp, diff;
    f >> currentRoom >> px >> py >> hp >> diff;
    difficulty = static_cast<Difficulty>(diff);
    map.loadFromFile(currentRoom);
    player.x = px;
    player.y = py;
    player.hp = hp;
    
    if (f >> numRoomSaves) {
        for (int i = 0; i < numRoomSaves; ++i) {
            f >> roomSaves[i].roomNumber >> roomSaves[i].slimesLeft;
        }
    } else {
        numRoomSaves = 0;
    }
    
    int rNum = 0;
    int spawned = 0;
    if (sscanf(currentRoom.c_str(), "room/room%d.txt", &rNum) == 1) {
        for(int i = 0; i < numRoomSaves; i++) {
            if(roomSaves[i].roomNumber == rNum) {
                spawnSlimes_Count(slimes, map, px, py, difficulty, roomSaves[i].slimesLeft);
                spawned = 1;
                break;
            }
        }
    }
    if (!spawned) {
        spawnSlimes_Difficulty(slimes, map, px, py, difficulty);
    }
    return true;
}
void Game::saveGame() {
    std::ofstream f("save.dat");
    f << currentRoom << "\n"
      << player.x << "\n"
      << player.y << "\n"
      << player.hp << "\n"
      << static_cast<int>(difficulty) << "\n"
      << numRoomSaves << "\n";
    for (int i = 0; i < numRoomSaves; ++i) {
        f << roomSaves[i].roomNumber << " " << roomSaves[i].slimesLeft << "\n";
    }
}
void Game::updateStartMenu() {}
void Game::updateDifficultyMenu() {}
void Game::updatePaused()  { /* freeze evrytng  */ }
void Game::inputSaving() {
    int ch = readKey();
    if (ch == 'y' || ch == 'Y') { saveGame(); exit(0); }
    if (ch == 'n' || ch == 'N') { exit(0); }
}

void Game::updateGameOver(){ /* nothing to tick */ }
void Game::updateWin()     { /* nothing to tick */ }
void Game::updatePlaying(){
    player.updateIFrames();
    player.updateShield();
    if(isAttacking){
        attackTimer--;
        if(attackTimer <= 0){
            isAttacking = false;
        }
    }

    if(attackCD > 0){
        attackCD--;
    }

    // Update boss if in boss room
    if (boss != nullptr) {
        boss->updateBoss(player, map);
        boss->updateBossProjectiles(map, player);
        
        // Handle player attack on boss - check hitbox for all body parts
        if (isAttacking && boss->isHitByAttack(attackX, attackY)) {
            if(dmgTimer == 0){
                boss->hp -= 50;
                dmgTimer = 5;
            }
            if (boss->hp <= 0) {
                delete boss;
                boss = nullptr;
                state = GameState::WIN;
            }
        }
    } else {
        // Normal slime handling when not in boss room
        for(auto& slime : slimes){
            slime.approach(player, map, slimes);
            slime.updateProjectiles(map, player);
        }

        if (isAttacking) {
            for (auto e = slimes.begin(); e != slimes.end(); ) {
                if (e->x == attackX && e->y == attackY) {
                    e->hp -= 10; 
                    if (e->hp <= 0) {
                        e = slimes.erase(e);
                        int rNum = 0;
                        if (sscanf(currentRoom.c_str(), "room/room%d.txt", &rNum) == 1) {
                            for(int i = 0; i < numRoomSaves; i++) {
                                if(roomSaves[i].roomNumber == rNum) {
                                    roomSaves[i].slimesLeft--;
                                    break;
                                }
                            }
                        }
                    }
                    else ++e;
                } else ++e;
            }
        }
    }
    if(dmgTimer > 0){
        dmgTimer--;
    }
    if (player.hp <= 0){
        state = GameState::GAME_OVER;
    }
}   


// ===============
//     RENDER
// ===============

void Game::render(){
    switch(state){
        case GameState::START_MENU:     renderStartMenu();     break;
        case GameState::DIFFICULTY_MENU: renderDifficultyMenu(); break;
        case GameState::PLAYING:  renderPlaying();  break;
        case GameState::PAUSED:   renderPaused();   break;
        case GameState::GAME_OVER:renderGameOver(); break;
        case GameState::WIN:      renderWin();      break;
        case GameState::SAVING:    renderSaving();   break;
    }  
}

void Game::renderStartMenu() {
    renderer.drawStartMenu(startMenuSelection, hasSaveFile());
}

void Game::renderDifficultyMenu() {
    renderer.drawDifficultyMenu(difficultyMenuSelection);
}

void Game::renderPlaying(){
    renderer.drawGame(map, player, slimes, boss, isAttacking, attackX, attackY);
}

void Game::renderPaused(){
    renderer.drawPauseOverlay();
}

void Game::renderGameOver(){
    renderer.drawGameOver();
}

void Game::renderWin(){
    renderer.drawWin();
}

void Game::renderSaving(){
    renderer.drawSavePrompt();
}

Game::Game() {
    if (!map.loadFromFile(currentRoom)) exit(1);
    state = GameState::START_MENU;
}

Game::~Game() {
    if (boss != nullptr) {
        delete boss;
        boss = nullptr;
    }
}

