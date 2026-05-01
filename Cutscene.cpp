#include "Cutscene.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

//run through g++ cutscene.cpp -o cutscene
// ./cutscene

void clearScreen() {
    cout << "\033[2J\033[H" << flush;
}

void waitSeconds(double seconds) {
    this_thread::sleep_for(chrono::duration<double>(seconds));
}

// For playScene / typeScene
void processChar(char c, ifstream& file, bool useDelay, double delaySeconds) {
    if (c == '^') {
        char next;
        if (file.get(next)) {
            switch (next) {
                case 'c': cout << "\033[36m"; break;
                case 'p': cout << "\033[0;35m"; break;
                case 'R': cout << "\033[31m"; break;
                case 'y': cout << "\033[33m"; break;
                case 'g': cout << "\033[32m"; break;
                case 'b': cout << "\033[34m"; break;
                case 'r': cout << "\033[0m";  break;
                default:  cout << '^' << next; break;
            }
        }
    } else {
        cout << c;

        if (useDelay) {
            cout.flush();
            this_thread::sleep_for(chrono::duration<double>(delaySeconds));
        }
    }
}

// For typeLine
void processCharInline(char c, const string& line, size_t& i, bool useDelay, double delaySeconds) {
    if (c == '^' && i + 1 < line.size()) {
        char next = line[i + 1];

        switch (next) {
            case 'c': cout << "\033[36m"; break;
            case 'p': cout << "\033[0;35m"; break;
            case 'R': cout << "\033[31m"; break;
            case 'y': cout << "\033[33m"; break;
            case 'g': cout << "\033[32m"; break;
            case 'b': cout << "\033[34m"; break;
            case 'r': cout << "\033[0m";  break;
            default:  cout << '^' << next; break;
        }

        i++; // skip next char
    } else {
        cout << c;

        if (useDelay) {
            cout.flush();
            this_thread::sleep_for(chrono::duration<double>(delaySeconds));
        }
    }
}


void playScene(const string& filename, double wait) {
    clearScreen();

    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    char c;
    while (file.get(c)) {
        processChar(c, file, false, 0);
    }

    cout << "\033[0m" << flush; // reset
    waitSeconds(wait);
}

void typeScene(const string& filename, double delaySeconds) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    char c;
    while (file.get(c)) {
        processChar(c, file, true, delaySeconds);
    }

    cout << "\033[0m" << flush;
}

void typeLine(const string& filename, int targetLine, double delaySeconds) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;
    int currentLine = 1;

    while (getline(file, line)) {
        if (currentLine == targetLine) {
            for (size_t i = 0; i < line.size(); i++) {
                processCharInline(line[i], line, i, true, delaySeconds);
            }

            cout << endl << "\033[0m";
            return;
        }
        currentLine++;
    }

    cout << "Error: Line not found.\n";
}


void spacescene(const string& filename, int space) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        for (int i = 0; i < space; i++) cout << " ";
        cout << line << endl;
    }
}

void panscene(const string& filename, int panspace, double waittime) {
    for (int i = panspace; i >= 0; i--) {
        clearScreen();
        spacescene(filename, i);
        waitSeconds(waittime);
    }
}

void dialogue(const string& sceneFile, const string& textFile,
              double delaySeconds, double wait, int line) {

    playScene(sceneFile, wait);
    cout << endl;
    typeLine(textFile, line, delaySeconds);
}


void intro() {

    // opening pan
    panscene("room/1 scene.txt", 20, 0.1);

    // show scene
    playScene("room/1 scene.txt", 2);

    // elevator + blink
    playScene("room/1 scene cyan.txt", 0.5);
    playScene("room/1 scene.txt", 0.5);
    playScene("room/1 scene cyan.txt", 0.5);
    playScene("room/1 scene.txt", 0.5);
    playScene("room/1 scene cyan.txt", 0.5);
    playScene("room/2 scene.txt", 2);

    // elevator animation
    for (int i = 1; i <= 11; i++) {
        string file = "room/3 scene frame " + to_string(i) + ".txt";
        playScene(file, (i == 1 || i == 11) ? 1 : 0.15);
    }

    // flicker lights
    playScene("room/4 scene.txt", 1);
    playScene("room/4 scene black.txt", 1);
    playScene("room/4 scene.txt", 0.5);
    playScene("room/4 scene black.txt", 0.5);
    playScene("room/4 scene.txt", 0.25);
    playScene("room/4 scene black.txt", 0.25);
    playScene("room/4 scene.txt", 0.25);
    playScene("room/4 scene black.txt", 1);
    playScene("room/4 scene load.txt", 2);

    // dialogue
    for (int i = 1; i <= 6; i++) {
        dialogue("room/5 scene.txt", "room/5 scene dialogue.txt", 0.02, 0, i);
        waitSeconds(2);
    }

    // final scene
    playScene("room/6 scene.txt", 3);

    for (int i = 1; i <= 2; i++) {
        dialogue("room/5 scene.txt", "room/7 scene dialogue.txt", 0.02, 0, i);
        waitSeconds(2);
    }

}