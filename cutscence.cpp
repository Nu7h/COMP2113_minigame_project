#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

//to run g++ cutscence.cpp -o cut

#include <cstdlib>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Wait for given seconds
void waitSeconds(double seconds) {
    this_thread::sleep_for(chrono::milliseconds((int)(seconds * 1000)));
}

// Print the whole scene file
void playScene(const string& filename, double wait) {
    
    clearScreen();
    
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

void typeLine(const string& filename, int targetLine, double delaySeconds) {
    ifstream file(filename);

    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;
    int currentLine = 1;

    // Find the line you want
    while (getline(file, line)) {
        if (currentLine == targetLine) {

            // Type this line character by character
            for (size_t i = 0; i < line.size(); i++) {
                char c = line[i];

                if (c == '^' && i + 1 < line.size()) {
                    char next = line[i + 1];

                    if (next == 'c') cout << "\033[36m";
                    else if (next == 'p') cout << "\033[0;35m";
                    else if (next == 'R') cout << "\033[31m";
                    else if (next == 'y') cout << "\033[33m";
                    else if (next == 'g') cout << "\033[32m";
                    else if (next == 'b') cout << "\033[34m";
                    else if (next == 'r') cout << "\033[0m";
                    else cout << '^' << next;

                    i++; // skip next char
                }
                else {
                    cout << c;
                    cout.flush();
                    this_thread::sleep_for(
                        chrono::milliseconds((int)(delaySeconds * 1000))
                    );
                }
            }

            cout << endl;
            cout << "\033[0m"; // reset color
            return; // stop after printing one line
        }

        currentLine++;
    }

    cout << "Error: Line " << targetLine << " not found." << endl;
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
        clearScreen();
        spacescene(filename,i);
        waitSeconds(waittime);
        cout << "\033[2J\033[H"; // clear screen + move cursor to top
    }
}

void dialogue(const string& filename, const string& textfilename, double delaySeconds, double wait, int line) {
    
    ifstream file(filename);
    ifstream textfile(textfilename);

    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    if (!textfile) {
        cout << "Error: Cannot open file " << textfilename << endl;
        return;
    }

    playScene(filename,wait);
    cout << "" << endl;
    typeLine(textfilename,line,delaySeconds);
}

int main() {
    //opening pan
    panscene("room/1 scene.txt",20,0.1);

    //show scene
    playScene("room/1 scene.txt",2);
    playScene("room/2 scene.txt",2);
    playScene("room/1 scene.txt",2);

    //elevator and charactor blink
    playScene("room/1 scene cyan.txt",0.5);
    playScene("room/1 scene.txt",0.5);
    playScene("room/1 scene cyan.txt",0.5);
    playScene("room/1 scene.txt",0.5);
    playScene("room/1 scene cyan.txt",0.5);
    playScene("room/2 scene.txt",2);
    playScene("room/2 scene cyan.txt",0.5);
    playScene("room/2 scene.txt",0.5);
    playScene("room/2 scene cyan.txt",0.5);
    playScene("room/2 scene.txt",0.5);
    playScene("room/2 scene cyan.txt",2);

    //elevator goes up
    playScene("room/3 scene frame 1.txt",1);
    playScene("room/3 scene frame 2.txt",0.15);
    playScene("room/3 scene frame 3.txt",0.15);
    playScene("room/3 scene frame 4.txt",0.15);
    playScene("room/3 scene frame 5.txt",0.15);
    playScene("room/3 scene frame 6.txt",0.15);
    playScene("room/3 scene frame 7.txt",0.15);
    playScene("room/3 scene frame 8.txt",0.15);
    playScene("room/3 scene frame 9.txt",0.15);
    playScene("room/3 scene frame 10.txt",0.15);
    playScene("room/3 scene frame 11.txt",1);

    //light goes off
    playScene("room/4 scene.txt",1);
    playScene("room/4 scene black.txt",1);
    playScene("room/4 scene.txt",0.5);
    playScene("room/4 scene black.txt",0.5);
    playScene("room/4 scene.txt",0.25);
    playScene("room/4 scene black.txt",0.25);
    playScene("room/4 scene.txt",0.25);
    playScene("room/4 scene black.txt",1);
    playScene("room/4 scene load.txt",2);

    //wizard show up
    dialogue("room/5 scene.txt","room/5 scene dialogue.txt",0.02,0.5,1);
    waitSeconds(1.5);
    dialogue("room/5 scene.txt","room/5 scene dialogue.txt",0.02,0.5,2);
    waitSeconds(1.5);
    dialogue("room/5 scene.txt","room/5 scene dialogue.txt",0.02,0.5,3);
    waitSeconds(1.5);

    //got sword and shield
    playScene("room/6 scene.txt",3);

    return 0;
}