#ifndef VECTOR3FLOAT_H_
#define VECTOR3FLOAT_H_
#include <math.h>

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

	void operator=(Vector3f const& other) {
		x = other.x;
		y = other.y;
		z = other.z;
	}

	Vector3f operator+(Vector3f const& other) const {
		return Vector3f(x + other.x, y + other.y, z + other.z);
	}

	Vector3f& operator+=(Vector3f const& other) const {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3f& operator-=(Vector3f const& other) const {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vector3f& operator*=(Vector3f const& other) const {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	Vector3f& operator/=(Vector3f const& other) const {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Vector3f operator-(Vector3f const& other) const {
		return Vector3f(x - other.x, y - other.y, z - other.z);
	}

	Vector3f operator*(Vector3f const& other) const {
		return Vector3f(x * other.x, y * other.y, z * other.z);
	}

	Vector3f operator/(Vector3f const& other) const {
		return Vector3f(x / other.x, y / other.y, z / other.z);
	}

	float magnitude() const {
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}

	Vector3f normalize() const {
		float mag = magnitude();
		if (mag == 1 || mag == 0)
			return Vector3f(this->x, this->y, this->z);
		float magInv = 1.0f / mag;
		return Vector3f(this->x * magInv, this->y * magInv, this->z * magInv);
	}

	bool isZero() const {
		return !x && !y && !z;
	}

	bool isOne() const {
		return x == 1 && y == 1 && z == 1;
	}
};
#endif // !VECTOR3FLOAT_H_