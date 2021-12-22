#pragma once

class Wave
{
	int mLength;
	int mLevels;
	int *mData;
	int *mOffset;
	
	void generateSquare(float pw);
	void generateSine();
	void generateNoise();
	void generateSaw();
	void generateTriangle(float step);
	
	void generateMipMap();
	void generateMipMapLevel(int level);
	
public:
	static const int waveAmplitude = 32768;

	Wave();
	~Wave();
	
	void init(int length);
	
	int getLength(int level) const;
	int getLevels() const;
	const int* getData(int level) const;
	
	void generate(int index);
};
