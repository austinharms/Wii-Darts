#ifndef AVG_FLOAT_H_
#define AVG_FLOAT_H_
#include <stdint.h>
#include <malloc.h>

class Avgf
{
public:
	Avgf(uint16_t sampleCount, float initialValue = 0) {
		_samples = (float*)malloc(sampleCount * sizeof(float));
		for (uint16_t i = 0; i < sampleCount; i++)
			_samples[i] = initialValue;
		_sum = initialValue * sampleCount;
		_sampleIndex = 0;
	}

	~Avgf() {
		free(_samples);
	}

	void add(float sample) {
		_sum -= _samples[_sampleIndex];
		_samples[_sampleIndex] = sample;
		_sum += sample;
		if (++_sampleIndex == _sampleCount)
			_sampleIndex = 0;
	}

	float value() const {
		return _sum / _sampleCount;
	}

private:
	uint16_t _sampleCount;
	uint16_t _sampleIndex;
	float* _samples;
	float _sum;
};
#endif // !AVG_FLOAT_H_
