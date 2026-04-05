#include "Game.h"
#include <chrono>
#include <thread>
#include <unistd.h> 
#include <cstdlib>
#include <random>
#include <vector>
#include <utility>

namespace {

std::mt19937& gameRng(){
    static std::mt19937 gen{std::random_device{}()};
    return gen;
}

void placeSlimeRandom(Slime& slime, const Map& map, int avoidX, int avoidY){
    std::vector<std::pair<int, int>> cells;
    cells.reserve(static_cast<std::size_t>(map.getWidth() * map.getHeight()));
    for(int yy = 0; yy < map.getHeight(); ++yy){
        for(int xx = 0; xx < map.getWidth(); ++xx){
            if(map.isWalkable(xx, yy) && (xx != avoidX || yy != avoidY))
                cells.emplace_back(xx, yy);
        }
    }
    if(cells.empty())
        return;
    std::uniform_int_distribution<std::size_t> dist(0, cells.size() - 1);
    const auto& pick = cells[dist(gameRng())];
    slime.x = pick.first;
    slime.y = pick.second;
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
    }
}

void Game::inputMenu(){
    int ch = readKey();
    switch(ch){
        case 'w':
        case KEY_UP:
            menuSelection = (menuSelection - 1 + menuOptionCount) % menuOptionCount;
            break;
        case 's':
        case KEY_DOWN:
            menuSelection = (menuSelection + 1) % menuOptionCount;
            break;
        case '\n':
        case ' ':
            if(menuSelection < 3){
                difficulty = static_cast<Difficulty>(menuSelection);
                placeSlimeRandom(slime, map, player.getX(), player.getY());
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
        case 'q': exit(0);
        case 'p': state = GameState::PAUSED; break;

        case 'w': dx =  0; dy = -1; moved = true; break;
        case 's': dx =  0; dy =  1; moved = true; break;
        case 'a': dx = -1; dy =  0; moved = true; break;
        case 'd': dx =  1; dy =  0; moved = true; break;
        case 'e': transition = true; break;

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
    
    if(moved){
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

    placeSlimeRandom(slime, map, player.getX(), player.getY());

    return true;
}

void Game::inputPaused(){
    int ch = readKey();
    switch(ch){
        case 'p': state = GameState::PLAYING; break;
        case 'q': exit(0);
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
    }
}

void Game::updateMenu()    { /* nothing to tick */ }
void Game::updatePaused()  { /* freeze evrytng  */ }
void Game::updateGameOver(){ /* nothing to tick */ }
void Game::updateWin()     { /* nothing to tick */ }

void Game::updatePlaying(){
    if(isAttacking){
        attackTimer--;
        if(attackTimer <= 0){
            isAttacking = false;
        }
    }

    if(attackCD > 0){
        attackCD--;
    }

    slime.approach(player, map);
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
    }  
}

void Game::renderMenu(){
    renderer.drawMenu(menuSelection);
}

void Game::renderPlaying(){
    renderer.drawGame(map, player, slime, isAttacking, attackX, attackY);
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

Game::Game(){
    if(!map.loadFromFile(currentRoom)){
        exit(1);
    }
}

