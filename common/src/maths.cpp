
#include <cmath>

#include <common/maths.hpp>

float4 operator*(const mat4x4& m, float4 v)
{
	float4 retFloat = {};
	
	for (int i = 0; i < 4; i++)
		retFloat.e[i] = v.e[0] * m.c[0].e[i] + v.e[1] * m.c[1].e[i] + v.e[2] * m.c[2].e[i] + v.e[3] * m.c[3].e[i];

	return retFloat;
}

mat4x4 operator*(const mat4x4& a, const mat4x4& b)
{
	mat4x4 retMat = {};

	for (int line = 0; line < 4; line++)
		for (int column = 0; column < 4; column++)
			for (int i = 0; i < 4; i++)
				retMat.c[column].e[line] += a.c[i].e[line] * b.c[column].e[i];

	return retMat;
}

float3 operator/(float3 v, float a)
{
	return { v.x / a, v.y / a, v.z / a };
}

float3 operator-(float3 v)
{
	return { -v.x, -v.y, -v.z };
}

float4 operator*(float4 v, float a)
{
	return { v.x * a, v.y * a, v.z * a, v.w * a };
}

float4 operator+(float4 a, float4 b)
{
	return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

mat4x4 mat4::frustum(float left, float right, float bottom, float top, float near, float far)
{
	return {
			2 * near / (right - left),  0.f,                        0.f,                            -near * (right + left) / (right - left),
			0.f,                        2 * near / (top - bottom),  0.f,                            -near * (top + bottom) / (top - bottom),
			0.f,                        0.f,                        -(far + near) / (far - near),   2 * far * near / (near - far),
			0.f,                        0.f,                        -1.f,                           0.f,
	};
	//return identity();
}

mat4x4 mat4::perspective(float fovY, float aspect, float near, float far)
{
	/*
	float top = near * maths::tan((fovY / 180.f * 3.14159f) / 2);
	float bottom = -top;
	float right = top * aspect;
	float left = -right;

	return frustum(left, right, bottom, top, near, far);*/
	
	mat4x4 projection;
	float f = 1 / tan((fovY / 180.f * 3.14159f) / 2);

	projection = {
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, -(far + near) / (far - near), -1,
		0, 0, - (2 * far * near) / (far - near), 0
	};

	return projection;
}

mat4x4 mat4::rotateX(float angleRadians)
{
	return {
		1,                   0,                  0,                 0,
		0,                   cos(angleRadians),  sin(angleRadians), 0,
		0,                   -sin(angleRadians), cos(angleRadians), 0,
		0,                   0,                  0,                 1
	};
	//return identity();
}

mat4x4 mat4::rotateY(float angleRadians)
{
	return {
		cos(angleRadians), 0, -sin(angleRadians), 0,
		0, 1,  0, 0,
		sin(angleRadians), 0, cos(angleRadians), 0,
		0, 0, 0, 1
	};
	//return identity();
}

mat4x4 mat4::rotateZ(float angleRadians)
{
	return identity();
}

mat4x4 mat4::scale(float scale)
{
	if (scale == 0)
		scale = 0.0001f;

	return {
		scale, 0.f, 0.f, 0.f,
		0.f, scale, 0.f, 0.f,
		0.f, 0.f, scale, 0.f,
		0.0f, 0.0f, 0.f, 1.f,
	};
}

mat4x4 mat4::translate(float3 translate)
{
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		translate.x, translate.y, translate.z, 1
	};
}

float3 mat4::cross(float3 vec1, float3 vec2)
{
	float3 retVec = {};

	retVec.x = vec1.y * vec2.z - vec1.z * vec2.y;
	retVec.y = vec1.z * vec2.x - vec1.x * vec2.z;
	retVec.z = vec1.x * vec2.y - vec1.y * vec2.x;

	return retVec;
}
