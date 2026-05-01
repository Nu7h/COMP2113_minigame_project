#ifndef OUTRO_H
#define OUTRO_H

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <unistd.h>
#include "Cutscene.h"
// Main public functions (these are what other files should use)
void playScene(const std::string& filename, double wait);
void typeScene(const std::string& filename, double delaySeconds);
void typeLine(const std::string& filename, int targetLine, double delaySeconds);
void dialogue(const std::string& sceneFile, const std::string& textFile,
              double delaySeconds, double wait, int line);
void outro();

// Utility functions (optional to expose — you can hide them if you want)
void clearScreen();
void waitSeconds(double seconds);

// Internal helpers (usually better NOT exposed, but included here if needed)
void processChar(char c, std::ifstream& file, bool useDelay, double delaySeconds);
void processCharInline(char c, const std::string& line, size_t& i,
                       bool useDelay, double delaySeconds);

#endif