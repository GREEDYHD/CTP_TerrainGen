#include <vector>

// THIS CLASS IS A TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

#ifndef _PERLINNOISE_H
#define _PERLINNOISE_H

class PerlinNoise {
	// The permutation vector
	std::vector<int> p;
public:
	// Initialize with the reference values for the permutation vector
	PerlinNoise();
	// Generate a new permutation vector based on the value of seed
	PerlinNoise(unsigned int);
	// Get a noise value, for 2D images z can have any value
	double noise(double, double, double);

	double Noise(int);
	double CosInterpolate(float, float, float);
	double PerlinNoise2D(int, float, int, float, float);

private:
	double fade(double);
	double lerp(double, double, double);
	double grad(int, double, double, double);
};

#endif