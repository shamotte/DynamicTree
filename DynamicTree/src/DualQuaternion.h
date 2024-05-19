#pragma once

class Quat {

public:
	float w;
	float x, y, z;



	Quat(float _w, float _x, float _y, float _z);

	Quat(float rx, float ry, float rz);
	Quat();



	Quat add(const Quat& other);
	Quat multiply(const Quat& other);
	Quat dot(const Quat& other);
	Quat cors(const Quat& other);

	Quat conjugate() const;
	Quat normalized() const;
	

	Quat operator+(const Quat& other);
	Quat operator*(const Quat& other);




};




class DualQuaternion {
public:
	Quat real;
	Quat dual;



	DualQuaternion add(const DualQuaternion& other);

	DualQuaternion multiply(const DualQuaternion& other);



	DualQuaternion conjugate() const;
	DualQuaternion conjugate();

	DualQuaternion(Quat _real, Quat _dual);
	DualQuaternion(float p1, float  p2, float p3, float p4);
	DualQuaternion();

};