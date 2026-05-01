#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <unistd.h>

// Utility
void clearScreen();
void waitSeconds(double seconds);

// Internal helpers (you can keep these public or hide later if refactoring)
void processChar(char c, std::ifstream& file, bool useDelay, double delaySeconds);
void processCharInline(char c, const std::string& line, size_t& i, bool useDelay, double delaySeconds);

// Scene display
void playScene(const std::string& filename, double wait);
void typeScene(const std::string& filename, double delaySeconds);
void typeLine(const std::string& filename, int targetLine, double delaySeconds);

// Effects
void spacescene(const std::string& filename, int space);
void panscene(const std::string& filename, int panspace, double waittime);

// Dialogue system
void dialogue(const std::string& sceneFile, const std::string& textFile,
              double delaySeconds, double wait, int line);

// High-level sequence
void intro();

void outro();

#endif // CUTSCENE_H