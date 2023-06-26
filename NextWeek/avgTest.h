#pragma once

class Accumulator 
{
	long double acc = 0;
	int counter = 0;

public:
	void add(double value) {
		acc += value;
		counter++;
	}

	double getAverage() {
		return acc / counter;
	}
};
