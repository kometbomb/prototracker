#pragma once

class Random
{
	static const unsigned long int N = 624;
	static const unsigned long int M = 397;
	static const unsigned long int MATRIX_A = 0x9908b0dfUL;   /* constant vector a */
	static const unsigned long int UPPER_MASK = 0x80000000UL; /* most significant w-r bits */
	static const unsigned long int LOWER_MASK = 0x7fffffffUL; /* least significant r bits */

	unsigned long mt[N]; /* the array for the state vector  */
	int mti; /* mti==N+1 means mt[N] is not initialized */
public:
	Random();
	void seed(unsigned long s);
	unsigned int rndu();
	int rnd(int min_val, int max_val);
	float rndf();
	long double rndl();
};
