#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

// Print the whole scene file
void playScene(const string& filename) {
    ifstream file(filename);

    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    char c;

    while (file.get(c)) {

        // Detect '^' commands
        if (c == '^') {
            char next;
            file.get(next);

            if (next == 'c') {
                cout << "\033[36m"; // cyan
            }
            else if (next == 'r') {
                cout << "\033[0m"; // reset
            }
            else {
                // not a valid command, print normally
                cout << '^' << next;
            }
        }
        else {
            cout << c;
        }
    }

    cout << "\033[0m"; // safety reset
}

// Wait for given seconds
void waitSeconds(double seconds) {
    this_thread::sleep_for(chrono::milliseconds((int)(seconds * 1000)));
}

int main() {
    //opening pan
    playScene("room/1 scene.txt");
    waitSeconds(3);

    playScene("room/enter.txt");
    playScene("room/2 scene.txt");
    waitSeconds(3);

    playScene("room/enter.txt");
    playScene("room/1 scene.txt");
    waitSeconds(3);

    playScene("room/enter.txt");
    playScene("room/1 scene cyan.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/1 scene.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/1 scene cyan.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/1 scene.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/1 scene cyan.txt");
    waitSeconds(0.5);

    return 0;
}