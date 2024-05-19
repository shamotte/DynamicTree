#include "DualQuaternion.h"
#define _USE_MATH_DEFINES
#include <math.h>


DualQuaternion DualQuaternion::add(const DualQuaternion& other)
{
	return DualQuaternion(real + other.real, dual + other.dual);
}


DualQuaternion  DualQuaternion::multiply(const DualQuaternion& o) {

	

	return DualQuaternion();

}

DualQuaternion DualQuaternion::conjugate() const
{
	return DualQuaternion();
}

DualQuaternion DualQuaternion::conjugate()
{
	return DualQuaternion();

}

DualQuaternion::DualQuaternion(Quat _real, Quat _dual) :
	real(_real), dual(_dual)
{
	
}

DualQuaternion::DualQuaternion(float p1, float p2, float p3, float p4):
	real(p1,p2,p3,p4), dual(0,0,0,0)
{
}

DualQuaternion::DualQuaternion():
	real(), dual()
{
	
}



#pragma region Quat
Quat::Quat(float _w, float _x, float _y, float _z)
{
	w = _w;
	x = _x;
	y = _y;
	z = _z;
}


#define CONVERSION M_PI/360

Quat::Quat(float ax, float ay, float az)
{
	float rx = ax * CONVERSION, ry = ay * CONVERSION, rz = az * CONVERSION;




	Quat p = Quat(cos(rx), sin(rx), 0, 0) * Quat(cos(ry), 0, sin(ry), 0) * Quat(cos(rz), 0, 0, sin(rz));
	w = p.w;
	x = p.x;
	y = p.y;
	z = p.z;
}

Quat::Quat()
{
	w = 1;
	x = 0;
	z = 0;
	y = 0;
}

Quat Quat::add(const Quat& o)
{
	return Quat(w + o.w, x + o.x, y + o.y, z + o.z);

}

Quat Quat::multiply(const Quat& o)
{
	float pw, px, py, pz;

	pw = o.w * w - x * o.x - y * o.y - z * o.z;

	px = w * o.x + x * o.w + y * o.z - z * o.y;

	py = w * o.y + y * o.w + z * o.x - x * o.z;

	pz = w * o.z + z * o.w + x * o.y - y * o.x;
	return Quat(pw, px, py, pz);

}

Quat Quat::dot(const Quat& other)
{
	return Quat();
}

Quat Quat::cors(const Quat& other)
{
	return Quat();
}

Quat Quat::conjugate() const
{
	return Quat(w,-x,-y,-z);
}

Quat Quat::normalized() const
{
	float sum = x * x + w * w + z * z + y * y;

	sum = sqrtf(sum);

	return Quat(w / sum, x / sum, y / sum, z / sum);
}

Quat Quat::operator+(const Quat& other)
{
	return this->add(other);
}

Quat Quat::operator*(const Quat& other)
{
	return this->multiply(other);
}

#pragma endregion
