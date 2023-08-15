#ifndef DARTS_TRANSFORM_H_
#define DARTS_TRANSFORM_H_
#include "Core.h"
#include <ogc/gu.h>

namespace darts {
	class Transform {
	public:
		Transform();

		Transform(const Transform& other);

		// returns the Transforms matrix
		Mtx& GetMatrix();

		// returns the Transforms matrix
		const Mtx& GetMatrix() const;

		// move the Transform by vector amount
		void Translate(const guVector& vector);

		// rotate the Transform by vector radians
		// Note: the rotation order is XYZ
		void Rotate(const guVector& vector);

		// scale the Transform by vector amount
		void Scale(const guVector& vector);

		// set the Transform to identity matrix
		void Reset();

		// set the Transform position
		void SetPosition(const guVector& vector);

		guVector GetPosition() const;

		guVector GetForward() const;

		guVector GetUp() const;

		guVector GetRight() const;

		// set the Transform to "look at" the target with the specified up vector
		void LookAt(const guVector& target, const guVector& up);

		Transform operator* (const Transform& other) const;

		Transform& operator=(const Transform& other);

		static void Mul(const Transform& a, const Transform& b, Transform& out);

	private:
		Mtx m_matrix;
	};
}
#endif // !DARTS_TRANSFORM_H_
