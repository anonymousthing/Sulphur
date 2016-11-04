#include "PerlinGenerator.h"

#include "Platform.h" //Used to seed by time.
#include "RAIIHelper.h"

#include <random>

static bool returnTrue(void *input) {
	return true;
}

PerlinGenerator::PerlinGenerator() {
	registerParameter("xScale", (void**)&xScale, returnTrue);
	registerParameter("yScale", (void**)&yScale, returnTrue);

	xScale = yScale = 75.0f;
}

PerlinGenerator::~PerlinGenerator() {

}

//Perlin Generator :D
static void initPerlin(int seed, int *p) {
	std::uniform_real_distribution<double> random;
	std::default_random_engine engine;

	if (seed == -1) {
		engine.seed(Platform::GetElapsedTime());
	} else {
		engine.seed(seed);
	}

	for (int i = 0; i < 256; i++)
		p[i + 256] = p[i] = (int)(random(engine) * 255.0);
}

template<typename T>
static T lerp(T a, T b, T weight) {
	return ((T)1 - weight) * a + weight * b;
}

//These two functions are come from Ken Perlin's paper.
static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
static double grad(int hash, double x, double y, double z) {
	int h = hash & 15;

	double u = h < 8 ? x : y;

	double v;
	if (h == 12 || h == 14) {
		v = x;
	} else if (h < 4) {
		v = y;
	} else {
		v = z;
	}

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

//This function is a cleaned-up version of Ken Perlin's formula.
static double noise(int *p, double x, double y, double z) {
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;
	int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
	int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = fade(x), v = fade(y), w = fade(z);

	double a = lerp(grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z), u);
	double b = lerp(grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z), u);
	double c = lerp(grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1), u);
	double d = lerp(grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1), u);

	return lerp(lerp(a, b, v), lerp(c, d, v), w);
}

void PerlinGenerator::_generate(float *buffer) const  {
	//buffer is width*height floats long.
	//We scale noise by xScale & yScale.
	//We seed our random number generator with seed.
	Buffer<int, 512> p;
	initPerlin(seed, p);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			buffer[y * width + x] = ((float)noise(p, x / xScale, y / yScale, 1) + 1.0f) / 2.0f;
		}
	}
}
