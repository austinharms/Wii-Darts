#include "engine/Transform.h"

Transform::Transform() {
	guMtxIdentity(m_matrix);
}

Transform::Transform(const Transform& other)
{
	guMtxCopy(((Transform*)&other)->m_matrix, m_matrix);
}

Mtx& Transform::GetMatrix() {
	return m_matrix;
}

const Mtx& Transform::GetMatrix() const {
	return m_matrix;
}

void Transform::Translate(const guVector& vector)
{
	guMtxTransApply(m_matrix, m_matrix, vector.x, vector.y, vector.z);
}

void Transform::Rotate(const guVector& vector)
{
	guVector pos = GetPosition();
	SetPosition({ 0,0,0 });
	Mtx temp;
	if (vector.x) {
		guVector axis{ 1, 0, 0 };
		guMtxRotAxisRad(temp, &axis, vector.x);
		guMtxConcat(temp, m_matrix, m_matrix);
	}

	if (vector.y) {
		guVector axis{ 0, 1, 0 };
		guMtxRotAxisRad(temp, &axis, vector.y);
		guMtxConcat(temp, m_matrix, m_matrix);
	}

	if (vector.z) {
		guVector axis{ 0, 0, 1 };
		guMtxRotAxisRad(temp, &axis, vector.z);
		guMtxConcat(temp, m_matrix, m_matrix);
	}

	SetPosition(pos);
}

void Transform::Scale(const guVector& vector)
{
	guMtxApplyScale(m_matrix, m_matrix, vector.x, vector.y, vector.z);
}

void Transform::Reset()
{
	guMtxIdentity(m_matrix);
}

void Transform::SetPosition(const guVector& position)
{
	m_matrix[0][3] = position.x;
	m_matrix[1][3] = position.y;
	m_matrix[2][3] = position.z;
}

void Transform::Rotate(const guVector& axis, float amount)
{
	guMtxRotAxisRad(m_matrix, (guVector*)&axis, amount);
}

guVector Transform::GetPosition() const
{
	return { m_matrix[0][3], m_matrix[1][3], m_matrix[2][3] };
}

guVector Transform::GetForward() const
{
	return { m_matrix[2][0], m_matrix[2][1], m_matrix[2][2] };
}

guVector Transform::GetUp() const
{
	return { m_matrix[1][0], m_matrix[1][1], m_matrix[1][2] };
}

guVector Transform::GetRight() const
{
	return { m_matrix[0][0], m_matrix[0][1], m_matrix[0][2] };
}

void Transform::LookAt(const guVector& target, const guVector& up)
{
	guVector pos = GetPosition();
	guVector dir;
	guVector right;
	guVector upDir;
	guVecSub(&pos, (guVector*)&target, &dir);

	guVecNormalize(&dir);
	guVecCross((guVector*)&up, &dir, &right);
	guVecNormalize(&right);
	guVecCross(&dir, &right, &upDir);
	//guVecScale(&upDir, &upDir, -1);
	guVecNormalize(&upDir);
	m_matrix[0][0] = right.x;
	m_matrix[0][1] = right.y;
	m_matrix[0][2] = right.z;
	m_matrix[0][3] = pos.x;
	m_matrix[1][0] = upDir.x;
	m_matrix[1][1] = upDir.y;
	m_matrix[1][2] = upDir.z;
	m_matrix[1][3] = pos.y;
	m_matrix[2][0] = dir.x;
	m_matrix[2][1] = dir.y;
	m_matrix[2][2] = dir.z;
	m_matrix[2][3] = pos.z;

	//Reset();
	////guVector upPos = pos;
	////guVecAdd((guVector*)&up, &upPos, &upPos);
	//guLookAt(m_matrix, &pos, &const_cast<guVector&>(up), (guVector*)&target);
}

Transform Transform::operator*(const Transform& other) const
{
	Transform t;
	guMtxConcat(((Transform*)&other)->m_matrix, ((Transform*)this)->m_matrix, t.m_matrix);
	return t;
}

Transform& Transform::operator=(const Transform& other)
{
	guMtxCopy(((Transform*)&other)->m_matrix, m_matrix);
	return *this;
}

void Transform::Mul(const Transform& a, const Transform& b, Transform& out)
{
	guMtxConcat(const_cast<MtxP>(a.m_matrix), const_cast<MtxP>(b.m_matrix), out.m_matrix);
}
