#include "DualQuaternion.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>

#define BIAS 0.000000000001

#pragma region Dual

DualQuaternion DualQuaternion::add(const DualQuaternion& other) const
{
	return DualQuaternion(real + other.real, dual + other.dual);
}


DualQuaternion  DualQuaternion::multiply(const DualQuaternion& o) const
{
	Quat a = (real * o.dual);
	Quat b = (dual * o.real);
	//std::cout << "----------\n" << a << b << "-----------------------\n";;


	return DualQuaternion(real * o.real,a + b);

}

DualQuaternion DualQuaternion::scale(const float value) const
{
	return DualQuaternion(real * value, dual * value);
}

DualQuaternion DualQuaternion::operator+(const DualQuaternion& other) const
{
	return add(other);
}

DualQuaternion DualQuaternion::operator*(const DualQuaternion& other) const
{
	return multiply(other);
}

DualQuaternion DualQuaternion::operator*(float scalar) const
{
	return scale(scalar);
}

DualQuaternion DualQuaternion::translation(float x, float y, float z)
{
	return DualQuaternion(Quat(1,0,0,0),Quat(0,x,y,z));
}

DualQuaternion DualQuaternion::rotation(float x, float y, float z)
{
	return DualQuaternion(Quat(x, y, z), Quat(0, 0, 0, 0));
}

Vertex DualQuaternion::toVertex()
{
	return Vertex(dual.x, dual.y, dual.z, 1);
}

float DualQuaternion::magnitue() const
{
	return sqrt(real.magnitudeSqr() + dual.magnitudeSqr());
}

DualQuaternion DualQuaternion::conjugate() const
{
	return DualQuaternion(real.conjugate(),dual);
}


DualQuaternion::DualQuaternion(Quat _real, Quat _dual) :
	real(_real), dual(_dual)
{
	
}

DualQuaternion::DualQuaternion(float p1, float p2, float p3, float p4):
	real(p1,p2,p3,p4), dual(0,0,0,0)
{
}

DualQuaternion::DualQuaternion(float x, float y, float z):
	real(1,0,0,0), dual(0,x,y,z)
{

}

DualQuaternion::DualQuaternion():
	real(), dual()
{
	
}


#pragma endregion


	#pragma region Quat
std::ostream& operator<<(std::ostream& os, const Quat& q)
{
	os  << std::fixed << std::setprecision(3) << q.w << " " << q.x << " " << q.y << " " << q.z << "\n";
	return os;
}



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

Quat Quat::add(const Quat& o) const
{
	return Quat(w + o.w, x + o.x, y + o.y, z + o.z);

}

Quat Quat::multiply(const Quat& o) const
{
	float pw, px, py, pz;

	pw = o.w * w - x * o.x - y * o.y - z * o.z;

	px = w * o.x + x * o.w + y * o.z - z * o.y;

	py = w * o.y + y * o.w + z * o.x - x * o.z;

	pz = w * o.z + z * o.w + x * o.y - y * o.x;
	return Quat(pw, px, py, pz);

}

Quat Quat::scale(float f) const
{
	return Quat(w*f,x*f,y*f,z*f);
}

float Quat::dot(const Quat& o) const 
{

	return w * o.w + x * o.x + y * o.y + z * o.z;
}

Quat Quat::cors(const Quat& other) const
{
	ASSERT(false);
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

float Quat::magnitude() const
{
	return sqrt(w * w + x * x + y * y + z * z);
}

float Quat::magnitudeSqr() const
{
	return w * w + x * x + y * y + z * z;
}


Quat Quat::lerp(const Quat& a, const Quat& b, float t) 
{
	//((a * t) + (b * (1 - t))).normalized();
	return (a * (1 - t) + b * t).normalized();
}

Quat Quat::slerp(const Quat& a, const Quat& b, float t) 
{
	float cosw = a.dot(b);
	if (cosw  - BIAS <= 0 ) {
		return Quat(a);
	}
	double sinw = sqrt(1 - (cosw * cosw));
	double w = atan2(sinw, cosw);



	return a* (sin((1 - t) * w) / sinw) + b * (sin(t * w) / sinw);

}



Quat Quat::operator+(const Quat& other) const
{
	return this->add(other);
}



Quat Quat::operator*(const Quat& other) const
{
	return this->multiply(other);
}

Quat Quat::operator*(float scalar) const
{
	return scale(scalar);
}

#pragma endregion



	
	#pragma region Vert

Vertex Quat::toPont() const
{
	return Vertex(x, y, z, 1);
}

Vertex::Vertex(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

Vertex::Vertex()
{
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}


#pragma endregion