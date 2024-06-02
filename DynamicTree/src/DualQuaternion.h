#pragma once
#include <iostream>
#include "Utils.h"

struct Vertex {
	float x, y, z, w;

	Vertex(float _x, float _y, float _z, float _w);
	Vertex();

};

class Quat {

public:
	float w;
	float x, y, z;



	Quat(float _w, float _x, float _y, float _z);

	Quat(float rx, float ry, float rz);
	Quat();



	Quat add(const Quat& other) const;
	Quat multiply(const Quat& other) const;

	Quat scale(float f) const;

	float dot(const Quat& other) const;
	Quat cors(const Quat& other) const;

	Quat conjugate() const;
	Quat normalized() const;
	float magnitude() const;
	float magnitudeSqr() const;


	static Quat lerp(const Quat& a, const Quat& b, float t);
	static Quat slerp(const Quat& a, const Quat& b, float t);




	Vertex toPont() const;
	

	Quat operator+(const Quat& other) const;
	Quat operator*(const Quat& other) const;
	Quat operator*( float scalar) const;







};

std::ostream& operator<<(std::ostream& os, const Quat& q);





class DualQuaternion {
public:
	Quat real;
	Quat dual;



	DualQuaternion add(const DualQuaternion& other) const;

	DualQuaternion multiply(const DualQuaternion& other) const;
	DualQuaternion scale(const float value) const;


	DualQuaternion operator+(const DualQuaternion& other) const;
	DualQuaternion operator*(const DualQuaternion& other) const;
	DualQuaternion operator*(float scalar) const;

	static DualQuaternion translation(float x, float y, float z);
	static DualQuaternion rotation(float x, float y, float z);
	Vertex toVertex();

	

	float magnitue() const;



	DualQuaternion conjugate() const;

	DualQuaternion(Quat _real, Quat _dual);
	DualQuaternion(float p1, float  p2, float p3, float p4);
	DualQuaternion(float x, float  y, float z);
	DualQuaternion();

};