#pragma once
#include <iostream>
#include <Map.h>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>

class GenerationTimer {
private:
	Map& terrain;
public:
	GenerationTimer(Map& terrain);
	bool measureDiamondSquare(int testsNumber);
	bool measurePerlin(int testsNumber, float baseFrequency, int octaves, float persistance, float amplitude);
	bool saveResults(std::vector<double> results, std::string fileName);
};