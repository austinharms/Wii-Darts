#ifndef VECTOR3FLOAT_H_
#define VECTOR3FLOAT_H_
struct Vector3f
{
public:
	Vector3f(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3f(float val) {
		x = val;
		y = val;
		z = val;
	}

	float x;
	float y;
	float z;
};
#endif // !VECTOR3FLOAT_H_