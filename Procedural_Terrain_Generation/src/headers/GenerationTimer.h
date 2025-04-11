#pragma once
#include <iostream>
#include <Map.h>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>
#include "HydraulicErosion.h"

class GenerationTimer {
private:
	Map& terrain;
	HydraulicErosion& hydraulicEroder;
public:
	GenerationTimer(Map& terrain, HydraulicErosion& hydraulicEroder);
	bool measureDiamondSquare(int testsNumber);
	bool measurePerlin(int testsNumber, float baseFrequency, int octaves, float persistance, float amplitude);
	bool measureHydraulicErosion(int testsNumber, int iterations, float volume, float dt, float density, float friction);
	bool saveResults(std::vector<double> results, std::string fileName);
};