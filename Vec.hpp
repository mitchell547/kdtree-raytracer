struct float3;
#pragma once
//#include "float3.h"


struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm 
	double x, y, z;                  // position, also color (r,g,b) 
	inline   Vec (double x_ = 0, double y_ = 0, double z_ = 0) {
		x = x_;
		y = y_;
		z = z_;
	}
	Vec(const float3&);
	Vec& operator=(const float3&);
	
	// for compability
	/*inline Vec& operator= (const float3& b) {
		x = b.v[0];
		y = b.v[1];
		z = b.v[2];
		return *this;
	}*/

	inline   Vec		operator+	  (const Vec &b) const {
		return Vec (x + b.x, y + b.y, z + b.z);
	}
	inline   Vec		operator-	  (const Vec &b) const {
		return Vec (x - b.x, y - b.y, z - b.z);
	}
	inline   Vec		operator*	  (double b)	 const {
		return Vec (x*b, y*b, z*b);
	}
	inline friend   Vec		operator/	  (const Vec &a, double b)	 {
		return Vec (a.x/b, a.y/b, a.z/b);
	}
	inline friend  Vec		operator/	  (double a, const Vec &b)	 {
		return Vec (a/b.x, a/b.y, a/b.z);
	}
	inline   Vec&		operator+=	  (const Vec &b) {
		x = x + b.x; y = y + b.y; z = z + b.z;
		return *this;
	}
	inline   Vec		inv			  ()			 const {
		return Vec (-x, -y, -z);
	}
	inline   Vec		mult		  (const Vec &b) const {
		return Vec (x*b.x, y*b.y, z*b.z);
	}
	inline   double	distance	  (const Vec &b) const {
		return  sqrt ((x - b.x)*(x - b.x) +
			(y - b.y)*(y - b.y) +
			(z - b.z)*(z - b.z));
	}
	inline   Vec&		normalization () {
		return *this = *this * (1 / sqrt (x*x + y*y + z*z));
	}
	inline   double	dot			  (const Vec &b) const {
		return x*b.x + y*b.y + z*b.z;
	} // cross: 
	inline   Vec		cross		  (const Vec&b)  const {
		return Vec (y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
	}
};

template<typename T> struct Vec3
{
	T x, y, z;                  
	inline     Vec3<T> (T x_, T y_, T z_) : x (x_), y (y_), z (z_){}

	inline     Vec3<T> () : x (0), y (0), z (0){	};

	inline     Vec3<T>		operator+	(const Vec3<T> &b)	const {
		return Vec3<T> (this->x + b.x, this->y + b.y, this->z + b.z);
	}

	inline     Vec3<T>		operator-	(const Vec3<T> &b)	const {
		return Vec3<T> (this->x - b.x, this->y - b.y, this->z - b.z);
	}

	inline     Vec3<T>		operator*	(T b)				const {
		return Vec3<T> (x*b, y*b, z*b);
	}

	inline     Vec3<T>		operator/	(T b)				const {
		T p = 1.f/ b;
		return Vec3<T> (x * p, y * p, z * p);
	}

	inline     Vec3<T>		inv ()					const {
		return Vec3<T> (-x, -y, -z);
	}

	inline     Vec3<T>		mult (const Vec3<T> &b)	const {
		return Vec3<T> (x*b.x, y*b.y, z*b.z);
	}
	
	inline     T				distance (const Vec3<T> &b)	const {
		return  sqrt ((x - b.x)*(x - b.x) +
			(y - b.y)*(y - b.y) +
			(z - b.z)*(z - b.z));
	}

	inline     Vec3<T>&		normalization () {
		return *this = *this * (1.f/ sqrt (x*x + y*y + z*z));
	}

	inline     T				dot (const Vec3<T> &b)	const {
		return fma (this->x,b.x , fma (this->y, b.y, this->z*b.z));
	}

	inline     Vec3<T>		cross		(const Vec3<T>&b)	const {
		return Vec3<T> (this->y*b.z - this->z*b.y, this->z*b.x - this->x*b.z, this->x*b.y - this->y*b.x);
	}
};
