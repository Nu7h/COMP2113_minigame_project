#include "Outro.h"

using namespace std;

//run through g++ cutscene.cpp -o cutscene
// ./cutscene

// ---- Skip state ----
static bool skipCutscene = false;

// Non-blocking check: sets skipCutscene if 'S' is pressed
static void checkSkipKey() {
    unsigned char c = 0;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 's' || c == 'S') {
            skipCutscene = true;
        }
    }
}

void clearScreen() {
    cout << "\033[2J\033[H" << flush;
}

// Waits in small slices so it can detect the skip key mid-wait
void waitSeconds(double seconds) {
    const double slice = 0.05;
    double elapsed = 0.0;
    while (elapsed < seconds && !skipCutscene) {
        this_thread::sleep_for(chrono::duration<double>(slice));
        elapsed += slice;
        checkSkipKey();
    }
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
            checkSkipKey();
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
            checkSkipKey();
        }
    }
}


void playScene(const string& filename, double wait) {
    if (skipCutscene) return;

    clearScreen();

    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    char c;
    while (file.get(c) && !skipCutscene) {
        processChar(c, file, false, 0);
    }

    cout << "\033[0m" << flush; // reset
    waitSeconds(wait);
}

void typeScene(const string& filename, double delaySeconds) {
    if (skipCutscene) return;

    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    char c;
    while (file.get(c) && !skipCutscene) {
        processChar(c, file, true, delaySeconds);
    }

    cout << "\033[0m" << flush;
}

void typeLine(const string& filename, int targetLine, double delaySeconds) {
    if (skipCutscene) return;

    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;
    int currentLine = 1;

    while (getline(file, line)) {
        if (currentLine == targetLine) {
            for (size_t i = 0; i < line.size() && !skipCutscene; i++) {
                processCharInline(line[i], line, i, true, delaySeconds);
            }

            cout << endl << "\033[0m";
            return;
        }
        currentLine++;
    }

    cout << "Error: Line not found.\n";
}

void dialogue(const string& sceneFile, const string& textFile,
              double delaySeconds, double wait, int line) {
    if (skipCutscene) return;

    playScene(sceneFile, wait);
    cout << endl;
    typeLine(textFile, line, delaySeconds);
}


void outro() {
    skipCutscene = false;  // reset flag at the start of each intro

    // Show skip hint
    clearScreen();
    cout << "\033[33m  Press S at any time to skip the outro...\033[0m" << flush;
    waitSeconds(1.5);

    // dialogue
    for (int i = 1; i <= 2 && !skipCutscene; i++) {
        dialogue("room/5 scene.txt", "outro/1 outro.txt", 0.02, 0, i);
        waitSeconds(2);
    }

    // Clear screen after skip so game starts clean
    if (skipCutscene) {
        clearScreen();
    }
}