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
            else if (next == 'p') {
                cout << "\033[0;35m";//purple
            }
            else if (next == 'R') {
                cout << "\033[31m"; //red
            }
            else if (next == 'y') {
                cout << "\033[33m"; //yellow
            }
            else if (next == 'g') {
                cout << "\033[32m"; //green
            }
            else if (next == 'b') {
                cout << "\033[34m"; //blue
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

void typeScene(const string& filename, double delaySeconds) {
    ifstream file(filename);

    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    char c;

    while (file.get(c)) {

        // Handle your ^ commands (color, etc.)
        if (c == '^') {
            char next;
            if (file.get(next)) {
                if (next == 'c') {
                    cout << "\033[36m"; // cyan
                }
                else if (next == 'p') {
                    cout << "\033[0;35m";//purple
                }
                else if (next == 'R') {
                    cout << "\033[31m"; //red
                }
                else if (next == 'y') {
                    cout << "\033[33m"; //yellow
                }
                else if (next == 'g') {
                    cout << "\033[32m"; //green
                }
                else if (next == 'b') {
                    cout << "\033[34m"; //blue
                }
                else if (next == 'r') {
                    cout << "\033[0m"; // reset
                }
                else {
                    cout << '^' << next;
                }
            }
        }
        else {
            cout << c;
            cout.flush(); // force immediate output
            this_thread::sleep_for(
                chrono::milliseconds((int)(delaySeconds * 1000))
            );
        }
    }

    cout << "\033[0m"; // reset color at end
}

void spacescene(const string& filename, int space) {
    ifstream file(filename);

    if (!file) {
    cout << "Error: Cannot open file " << filename << endl;
    return;
    }
    
    string line;

    while (std::getline(file,line)) {
        for ( int i=0; i < space; i++) {
            cout << " ";
        }

        cout << line << endl;
    }
}

void panscene(const string& filename, int panspace, double waittime) {
        ifstream file(filename);

    if (!file) {
    cout << "Error: Cannot open file " << filename << endl;
    return;
    }

    for (int i = panspace; i>=0; i--) {
        playScene("room/enter.txt");
        spacescene(filename,i);
        waitSeconds(waittime);
        cout << "\033[2J\033[H"; // clear screen + move cursor to top
    }
}


int main() {
    //opening pan
    panscene("room/1 scene.txt",20,0.1);
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

    playScene("room/enter.txt");
    playScene("room/2 scene.txt");
    waitSeconds(2);

    playScene("room/enter.txt");
    playScene("room/2 scene cyan.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/2 scene.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/2 scene cyan.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/2 scene.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/2 scene cyan.txt");
    waitSeconds(2);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 1.txt");
    waitSeconds(1);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 2.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 3.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 4.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 5.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 6.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 7.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 8.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 9.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 10.txt");
    waitSeconds(0.15);

    playScene("room/enter.txt");
    playScene("room/3 scene frame 11.txt");
    waitSeconds(1);

    playScene("room/enter.txt");
    playScene("room/4 scene.txt");
    waitSeconds(1);

    playScene("room/enter.txt");
    playScene("room/4 scene black.txt");
    waitSeconds(1);

    playScene("room/enter.txt");
    playScene("room/4 scene.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/4 scene black.txt");
    waitSeconds(0.5);

    playScene("room/enter.txt");
    playScene("room/4 scene.txt");
    waitSeconds(0.25);

    playScene("room/enter.txt");
    playScene("room/4 scene black.txt");
    waitSeconds(0.25);

    playScene("room/enter.txt");
    playScene("room/4 scene.txt");
    waitSeconds(0.25);

    playScene("room/enter.txt");
    playScene("room/4 scene black.txt");
    waitSeconds(1);

    playScene("room/enter.txt");
    playScene("room/4 scene load.txt");
    waitSeconds(2);

    playScene("room/enter.txt");
    playScene("room/5 scene.txt");
    waitSeconds(2);
    cout << "" << endl;
    typeScene("room/5 scene dialogue.txt",0.02);
    waitSeconds(5);
    playScene("room/enter.txt");
    playScene("room/6 scene.txt");

    return 0;
}