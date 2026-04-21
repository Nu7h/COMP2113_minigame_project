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

void Game::input(){
    switch(state){
        case GameState::MENU:     inputMenu();    break;
        case GameState::PLAYING:  inputPlaying(); break;
        case GameState::PAUSED:   inputPaused();  break;
        case GameState::GAME_OVER:inputGameOver();break;
        case GameState::WIN:      inputWin();     break;
        case GameState::SAVING:    inputSaving();   break;
        case GameState::CONTINUE:  inputContinue(); break;
    }
}

void Game::inputMenu(){
    int ch = readKey();
    switch(ch){
        case 'w': case 'W':
        case KEY_UP:
            menuSelection = (menuSelection - 1 + menuOptionCount) % menuOptionCount;
            break;
        case 's': case 'S':
        case KEY_DOWN:
            menuSelection = (menuSelection + 1) % menuOptionCount;
            break;
        case '\n':
        case ' ':
            if(menuSelection < 3){
                difficulty = static_cast<Difficulty>(menuSelection);
                spawnSlimes_Difficulty(slimes, map, player.getX(), player.getY(), difficulty);
                numRoomSaves = 0;
                int rNum = 0;
                if (sscanf(currentRoom.c_str(), "room/room%d.txt", &rNum) == 1) {
                    roomSaves[numRoomSaves].roomNumber = rNum;
                    roomSaves[numRoomSaves].slimesLeft = slimes.size();
                    numRoomSaves++;
                }
                state = GameState::PLAYING;
            } else exit(0);
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

    int w = map.getWidth();
    int h = map.getHeight();

    if(dir == 'N') player.y = h - 1;
    else if(dir == 'E') player.x = 0;
    else if(dir == 'S') player.y = 0;
    else if(dir == 'W') player.x = w - 1;

    int rNum = 0;
    if (sscanf(currentRoom.c_str(), "room/room%d.txt", &rNum) == 1) {
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
        isAttacking   = false;
        attackTimer   = 0;
        attackCD = 0;
        menuSelection  = 0;
        state = GameState::MENU;
    }
}

void Game::inputWin(){
    int ch = readKey();
    if(ch == '\n' || ch == ' '){
        menuSelection = 0;
        state = GameState::MENU;
    }
}

// ===============
//     UPDATE
// ===============

void Game::update(){
    switch(state){
        case GameState::MENU:     updateMenu();     break;
        case GameState::PLAYING:  updatePlaying();  break;
        case GameState::PAUSED:   updatePaused();   break;
        case GameState::GAME_OVER:updateGameOver(); break;
        case GameState::WIN:      updateWin();      break;
        case GameState::SAVING:    break;
        case GameState::CONTINUE:  break;
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
void Game::updateMenu()    { /* nothing to tick */ }
void Game::updatePaused()  { /* freeze evrytng  */ }
void Game::inputSaving() {
    int ch = readKey();
    if (ch == 'y' || ch == 'Y') { saveGame(); exit(0); }
    if (ch == 'n' || ch == 'N') { exit(0); }
}
void Game::inputContinue() {
    int ch = readKey();
    if (ch == 'y' || ch == 'Y') {
        if (loadGame()) state = GameState::PLAYING;
    } else if (ch == 'n' || ch == 'N') {
        std::remove("save.dat");
        state = GameState::MENU;
    }
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

    for(auto& slime : slimes){
        slime.approach(player, map, slimes);
        slime.updateProjectiles(map, player);
    }

    if (isAttacking) {
        for (auto e = slimes.begin(); e != slimes.end(); ) {
            if (e->x == attackX && e->y == attackY) {
                e->hp -= 10; // 5 hits to kill (100/20)
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
    if (player.hp <= 0){
        state = GameState::GAME_OVER;
    }
}   

// ===============
//     RENDER
// ===============

void Game::render(){
    switch(state){
        case GameState::MENU:     renderMenu();     break;
        case GameState::PLAYING:  renderPlaying();  break;
        case GameState::PAUSED:   renderPaused();   break;
        case GameState::GAME_OVER:renderGameOver(); break;
        case GameState::WIN:      renderWin();      break;
        case GameState::SAVING:    renderSaving();   break;
        case GameState::CONTINUE:  renderContinue(); break;
    }  
}

void Game::renderMenu(){
    renderer.drawMenu(menuSelection);
}

void Game::renderPlaying(){
    renderer.drawGame(map, player, slimes, isAttacking, attackX, attackY);
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

void Game::renderContinue(){
    renderer.drawContinuePrompt();
}

Game::Game() {
    if (!map.loadFromFile(currentRoom)) exit(1);
    std::ifstream f("save.dat");
    if (f.good()) state = GameState::CONTINUE; // show continue prompt first
    else state = GameState::MENU;
}

