#include "GenerationTimer.h"

GenerationTimer::GenerationTimer(Map& terrain, HydraulicErosion& hydraulicEroder): terrain(terrain), hydraulicEroder(hydraulicEroder) {

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

bool GenerationTimer::measureHydraulicErosion(int testsNumber, int iterations, float volume, float dt, float density, float friction) {
	std::vector<double> measurments;
	LARGE_INTEGER startTime, endTime, diffTime, frequency;
	QueryPerformanceFrequency(&frequency);
	for (int i = 0; i < testsNumber; i++) {
		std::cout << i << std::endl;
		QueryPerformanceCounter(&startTime);
		//terrain.generateWithPerlin(baseFrequancy, octaves, persistance, amplitude);
		for (int q = 0; q < iterations; q++) {
			for (int i = 0; i < terrain.getSize() - 1; i++) {
				for (int j = 0; j < terrain.getSize() - 1; j++) {
					hydraulicEroder.Erode(q, volume, dt, density, friction, glm::ivec2{ i,j });
				}
			}
		}
		QueryPerformanceCounter(&endTime);
		diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
		double time = ((double)diffTime.QuadPart * 1000) / frequency.QuadPart;
		measurments.push_back(time);
	}
	return saveResults(measurments, ("Erosion-tests-" + std::to_string(terrain.getSize()) + ".txt").c_str());
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