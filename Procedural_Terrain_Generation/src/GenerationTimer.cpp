#include "GenerationTimer.h"

GenerationTimer::GenerationTimer(Map& terrain): terrain(terrain) {

}

bool GenerationTimer::measureDiamondSquare(int testsNumber) {
	std::vector<double> measurments;
	LARGE_INTEGER startTime, endTime, diffTime, frequency;
	QueryPerformanceFrequency(&frequency);
	for (int i = 0; i < testsNumber; i++) {
		QueryPerformanceCounter(&startTime);
		terrain.generate();
		QueryPerformanceCounter(&endTime);
		diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
		double time = ((double)diffTime.QuadPart * 1000) / frequency.QuadPart;
		measurments.push_back(time);
	}
	return saveResults(measurments, ("Diamond-test-" + std::to_string(terrain.getSize()) + ".txt").c_str());
}

bool GenerationTimer::measurePerlin(int testsNumber, float baseFrequancy, int octaves, float persistance, float amplitude) {
	std::vector<double> measurments;
	LARGE_INTEGER startTime, endTime, diffTime, frequency;
	QueryPerformanceFrequency(&frequency);
	for (int i = 0; i < testsNumber; i++) {
		std::cout << i << std::endl;
		QueryPerformanceCounter(&startTime);
		terrain.generateWithPerlin(baseFrequancy, octaves, persistance, amplitude);
		QueryPerformanceCounter(&endTime);
		diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
		double time = ((double)diffTime.QuadPart * 1000) / frequency.QuadPart;
		measurments.push_back(time);
	}
	return saveResults(measurments, ("Perlin-tests-" + std::to_string(terrain.getSize()) + ".txt").c_str());
}

bool GenerationTimer::saveResults(std::vector<double> results, std::string fileName) {
	std::ofstream oFile;
	oFile.open(MEASUREMENTS_DIR + fileName);
	int mapSize = terrain.getSize();
	if (oFile.is_open()) {
		for (int i = 0; i < results.size(); i++) {
			oFile << mapSize << " " << results[i] << "\n";
		}
		return true;
	}
	else {
		return false;
	}
}