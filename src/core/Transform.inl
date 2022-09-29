#include <ogc/gu.h>
#include "Transform.h"

namespace wiidarts {
	Transform::Transform() {
		guMtxIdentity(_matrix);
	}

	Transform::Transform(const Transform& other)
	{
		guMtxCopy(((Transform*)&other)->_matrix, _matrix);
	}

	Mtx& Transform::getMatrix() {
		return _matrix;
	}

	const Mtx& Transform::getMatrix() const {
		return _matrix;
	}

	void Transform::translate(const guVector& vector)
	{
		guMtxTransApply(_matrix, _matrix, vector.x, vector.y, vector.z);
	}

	void Transform::rotate(const guVector& vector)
	{
		guVector pos = getPosition();
		setPosition({ 0,0,0 });
		Mtx temp;
		if (vector.x) {
			guVector axis{ 1, 0, 0 };
			guMtxRotAxisRad(temp, &axis, vector.x);
			guMtxConcat(temp, _matrix, _matrix);
		}

		if (vector.y) {
			guVector axis{ 0, 1, 0 };
			guMtxRotAxisRad(temp, &axis, vector.y);
			guMtxConcat(temp, _matrix, _matrix);
		}

		if (vector.z) {
			guVector axis{ 0, 0, 1 };
			guMtxRotAxisRad(temp, &axis, vector.z);
			guMtxConcat(temp, _matrix, _matrix);
		}

		setPosition(pos);
	}

	void Transform::scale(const guVector& vector)
	{
		guMtxApplyScale(_matrix, _matrix, vector.x, vector.y, vector.z);
	}

	void Transform::reset()
	{
		guMtxIdentity(_matrix);
	}

	void Transform::setPosition(const guVector& position)
	{
		_matrix[0][3] = position.x;
		_matrix[1][3] = position.y;
		_matrix[2][3] = position.z;
	}

	guVector Transform::getPosition() const
	{
		return { _matrix[0][3], _matrix[1][3], _matrix[2][3] };
	}

	void Transform::lookAt(const guVector& target, const guVector& up)
	{
		guVector pos = getPosition();
		guLookAt(_matrix, &pos, (guVector*)&up, (guVector*)&target);
	}

	Transform Transform::operator*(const Transform& other) const
	{
		Transform t;
		guMtxConcat(((Transform*)&other)->_matrix, ((Transform*)this)->_matrix, t._matrix);
		return t;
	}

	Transform& Transform::operator=(const Transform& other)
	{
		guMtxCopy(((Transform*)&other)->_matrix, _matrix);
		return *this;
	}
}