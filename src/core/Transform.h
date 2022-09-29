#include <ogc/gu.h>

#ifndef WIIDARTS_TRANSFORM_H_
#define WIIDARTS_TRANSFORM_H_
namespace wiidarts {
	class Transform {
	public:
		inline Transform();

		inline Transform(const Transform& other);

		// returns the Transforms matrix
		inline Mtx& getMatrix();

		// returns the Transforms matrix
		inline const Mtx& getMatrix() const;

		// move the Transform by vector amount
		inline void translate(const guVector& vector);

		// rotate the Transform by vector radians
		// Note: the rotation order is XYZ
		inline void rotate(const guVector& vector);

		// scale the Transform by vector amount
		inline void scale(const guVector& vector);

		// set the Transform to identity matrix
		inline void reset();

		// set the Transform position
		inline void setPosition(const guVector& vector);

		inline guVector getPosition() const;

		// set the Transform to "look at" the target with the specified up vector
		inline void lookAt(const guVector& target, const guVector& up);

		inline Transform operator* (const Transform& other) const;

		inline Transform& operator=(const Transform& other);

	private:
		Mtx _matrix;
	};
}

#include "Transform.inl"
#endif // !WIIDARTS_TRANSFORM_H_