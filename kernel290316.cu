// triangle.cpp: определяет точку входа для консольного приложения.
//
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <math.h>   
#include <stdlib.h> 
#include <random>
#include <iostream>
#include <omp.h>
#define EPSILON 0.000000001
#define INF 1e20
#define standartTest//baricentTest
#define bmp
//#define sampleInCenter
#define filter


struct Vec
{        // Usage: time ./smallpt 5000 && xv image.ppm 
	double x, y, z;                  // position, also color (r,g,b) 
	inline __host__ __device__ Vec (double x_ = 0, double y_ = 0, double z_ = 0)
	{
		x = x_;
		y = y_;
		z = z_;
	}
	inline __host__ __device__ Vec		operator+	  (const Vec &b) const
	{
		return Vec (x + b.x, y + b.y, z + b.z);
	}
	inline __host__ __device__ Vec		operator-	  (const Vec &b) const
	{
		return Vec (x - b.x, y - b.y, z - b.z);
	}
	inline __host__ __device__ Vec		operator*	  (double b)	 const
	{
		return Vec (x*b, y*b, z*b);
	}
	inline __host__ __device__ Vec		operator/	  (double b)	 const
	{
		return Vec (x/b, y/b, z/b);
	}
	inline __host__ __device__ Vec		inv			  ()			 const
	{
		return Vec (-x, -y, -z);
	}
	inline __host__ __device__ Vec		mult		  (const Vec &b) const
	{
		return Vec (x*b.x, y*b.y, z*b.z);
	}
	inline __host__ __device__ double	distance	  (const Vec &b) const
	{
		return  sqrt ((x - b.x)*(x - b.x) +
			(y - b.y)*(y - b.y) +
			(z - b.z)*(z - b.z));
	}
	inline __host__ __device__ Vec&		normalization ()
	{
		return *this = *this * (1 / sqrt (x*x + y*y + z*z));
	}
	inline __host__ __device__ double	dot			  (const Vec &b) const
	{
		return x*b.x + y*b.y + z*b.z;
	} // cross: 
	inline __host__ __device__ Vec		cross		  (const Vec&b)  const
	{
		return Vec (y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
	}
};
struct Ray
{
	Vec o, d; //o-начало d-направление
	__host__ __device__ Ray (Vec o_, Vec d_) : o (o_), d (d_) {}
};
struct triangle
{
	Vec a1, a2, a3, c, _normal;      // position, color 
	Vec o; double rad;//spher
	double difra = 1; double reflect;
	inline __host__ __device__ triangle (Vec a1_, Vec a2_, Vec a3_, Vec c_, double reflect_) :
		a1 (a1_), a2 (a2_), a3 (a3_), c (c_), reflect (reflect_)
	{
		_normal = (a2 - a1).cross(a3 - a1);

		makeMinimumBoundingSphere (a1, a2, a3);
		
	}
	void  makeMinimumBoundingSphere (const Vec &p1, const Vec &p2, const Vec &p3) {
		
		// Calculate relative distances
		float A = (p1  ).distance (p2);
		float B = (p2  ).distance (p3);
		float C = (p3 ).distance (p1);

		// Re-orient triangle (make A longest side)
		const Vec *a = &p3, *b = &p1, *c = &p2;
		if (B < C) swap (B, C), swap (b, c);
		if (A < B) swap (A, B), swap (a, b);

		// If obtuse, just use longest diameter, otherwise circumscribe
		if ((B*B) + (C*C) <= (A*A)) {
			rad = A / 2.f;
			o = (*b + *c)/ 2.f;
		}
		else {
			// http://en.wikipedia.org/wiki/Circumscribed_circle
			float  cos_a = (B*B + C*C - A*A) / (B*C * 2);
			rad = A / (sqrt (1 - cos_a*cos_a)*2.f);
			Vec alpha = *a - *c, beta = *b - *c;
			o = (beta * alpha.dot (alpha) - alpha * beta.dot (beta)).cross (alpha.cross (beta)) /
				(alpha.cross (beta).dot (alpha.cross (beta)) * 2.f) + *c;
		}
		
	}
	void swap (float & A, float & B)
	{
		float C = A;
		A = B;
		B = C;
	}
	void swap (const Vec * A, const Vec * B)
	{
		const Vec * C = A;
		A = B;
		B = C;
	}
	inline __host__ __device__ triangle (){}

	inline __host__ __device__ triangle moveX (const double step)const
	{
		triangle newT = *this;
		newT.a1.x += step;
		newT.a2.x += step;
		newT.a3.x += step;
		return newT;
	}
	inline __host__ __device__ triangle moveY (const double step)const
	{
		triangle newT = *this;
		newT.a1.y += step;
		newT.a2.y += step;
		newT.a3.y += step;
		return newT;
	}
	inline __host__ __device__ triangle moveZ (const double step)const
	{
		triangle newT = *this;
		newT.a1.z += step;
		newT.a2.z += step;
		newT.a3.z += step;
		return newT;
	}

	inline __host__ __device__ Vec normal ()const
	{
		return _normal;
	}

	bool intersectSpher (const Ray &r) const
	{ // returns distance, 0 if nohit 
		//Vec op = o - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
		//double t,  b = op.dot (r.d), det = b*b - op.dot (op) + rad*rad;
		//if (det + EPSILON <0) return false; else det = sqrt (det);
		//return (t = b - det)>EPSILON ? t : ((t = b + det) > EPSILON ? t : 0);
		//
		
		
		//a == 1; // because rdir must be normalized
		Vec k = r.o - o;
		double b = k.dot ( r.d);
		double c =k. dot ( k) - rad*rad;
		double d = b*b - c;

		if (d-EPSILON >= 0)
		{
			double sqrtfd = sqrt (d);
			// t, a == 1
			double t1 = -b + sqrtfd;
			double t2 = -b - sqrtfd;
			
		    double min_t = t1<t2 ? t1 : t2;//min (t1, t2);
			double max_t = t1>t2 ? t1 : t2; //max (t1, t2);
			
			double t = (min_t >= 0) ? min_t : max_t;
			//tResult = t;
			return (t > 0);
		}
		else
		{
			return true;
		}
		//
	}

#ifdef  standartTest
	inline __host__ __device__ bool		intersect (const Ray &r, Vec & hit) const
	{
		double k = 0;								//plane coefficient

		Vec pl = this->normal ().normalization();	//normal of flat for this object
		double D = -pl.dot (a1);					//point's projection on normal 
		double nu = pl.dot (r.o) + D;				//numerator 
		double den = pl.dot (r.d);					//denominator
		if (abs (den) - EPSILON<0) return false;
		k = -(nu) / (den);							
		hit = r.d*k + r.o;							//hit point
		//verifications
		Vec hitor = hit - r.o;						//verification that the
		double heor = hitor.dot (r.d) - EPSILON;	//point along the ray path

		bool t = PointInTriangle (hit, a1, a2, a3); //verification that the
		//////////////////////////////////////////////point in triangle

		if (t && heor > 0)return true;				//verification access
		else			return false;

	}
	inline __host__ __device__ bool		SameSide (const Vec p1, const Vec p2, const Vec a, const Vec b)const
	{
		Vec cp1 = (b - a).cross (p1 - a);
		Vec cp2 = (b - a).cross (p2 - a);
		double d = cp1.dot (cp2);
		if (d + EPSILON >= 0) return true;
		else return false;
	}
	inline __host__ __device__ bool		PointInTriangle (const Vec p, const Vec a, const Vec b, const Vec c)const
	{
		if (SameSide (p, a, b, c) && 
			SameSide (p, b, a, c) && 
			SameSide (p, c, a, b))  
			 return true;
		else return false;
	}
#endif
	
#ifdef  baricentTest

	/* code rewritten to do tests on the sign of the determinant */
	/* the division is before the test of the sign of the det    */
	/* and one CROSS has been moved out from the if-else if-else */
	inline __host__ __device__ bool intersect (const Ray &r, Vec & hit) const
	{
		Vec edge1, edge2, tvec, pvec, qvec;
		double det, inv_det;
		double u, v, t;
		/* find vectors for two edges sharing vert0 */
		edge1 = a2 - a1;
		edge2 = a3 - a1;

		/* begin calculating determinant - also used to calculate U parameter */
		pvec = r.d%edge2;

		/* if determinant is near zero, ray lies in plane of triangle */
		det = edge1.dot (pvec);

		/* calculate distance from vert0 to ray origin */
		tvec = r.o - a1;
		inv_det = 1.0 / det;

		qvec = tvec%edge1;

		if (det > EPSILON)
		{
			u = tvec.dot (pvec);
			if (u - EPSILON < 0.0 || u > det - EPSILON)
				return false;

			/* calculate V parameter and test bounds */
			v = r.d.dot (qvec);
			if (v - EPSILON < 0.0 || u + v > det - EPSILON)
				return false;

		}
		else if (det < -EPSILON)
		{
			/* calculate U parameter and test bounds */
			u = tvec.dot (pvec);
			if (u + EPSILON > 0.0 || u - EPSILON < det)
				return false;

			/* calculate V parameter and test bounds */
			v = r.d.dot (qvec);
			if (v + EPSILON > 0.0 || u + v - EPSILON < det)
				return false;
		}
		else return false;  /* ray is parallell to the plane of the triangle */

		t = edge2.dot (qvec)*inv_det;
		u = inv_det;
		v = inv_det;
		hit = r.o + r.d*t;
		return true;
	}
#endif
};
struct camera
{
	Ray cameraLocation;
	Vec cameraXangle;
	Vec cameraYangle;
	camera (Ray cameraLocation_, Vec cameraXangle_) :
		cameraLocation (cameraLocation_),
		cameraXangle (cameraXangle_)
	{
		cameraYangle = (cameraXangle_.cross (cameraLocation_.d)).normalization ()*.5135;
	}
};
struct imgSettings
{
	unsigned int w;//width
	unsigned int h;//hight
	unsigned int samps;//sampels per pixel
	imgSettings (unsigned int w_, unsigned int h_, unsigned int samps_) :w (w_), h (h_), samps (samps_){}
};
struct world
{
	triangle*objects;
	Vec	*lights;
	unsigned int objCount;
	unsigned int lightsCount;
	world ();
	world (const unsigned int objCount_, const unsigned int lightsCount_,
		const triangle*objects_, const Vec	*lights_) :
		objCount (objCount_),
		lightsCount (lightsCount_)
	{
		objects = (triangle *)malloc (objCount_*sizeof(triangle));
		lights = (Vec *)malloc (lightsCount_*sizeof(Vec));
		for (unsigned int i = 0; i < objCount; ++i)
			objects[i] = objects_[i];
		for (unsigned int i = 0; i < lightsCount; ++i)
			lights[i] = lights_[i];
	}

};

 //world CUDA_WORLD();
triangle * CUDA_objects;
//enum Refl_t { DIFF, SPEC, REFR };  // material types, used in distanceiance() 
#pragma pack (push)
#pragma pack(1)
struct bmpinfo
{

	unsigned char signature1 = 'B';//1 byte
	unsigned char signature2 = 'M';//1 byte
	unsigned int fileSize;//4 byte
	unsigned int reserved = 0;//4 byte
	unsigned int offset = 14 + 40;//4 byte
	unsigned int headerSize = 40;//4 byte
	unsigned int width;//4 byte
	unsigned int height;//4 byte
	unsigned short colorPlanes = 1;//2 byte
	unsigned short bpp = 24;//2 byte
	unsigned int compression = 0;//4 byte
	unsigned int imgSize;//4 byte
	unsigned int resolutionHor = 2795;//4 byte
	unsigned int resolutionVer = 2795;//4 byte
	unsigned int pltColors = 0;//4 byte
	unsigned int impColors = 0;//4 byte
	bmpinfo (unsigned int _width, unsigned int _height)
	{
		width = _width;
		height = _height;
		fileSize = 14 + 40 + height * width * 3;
		imgSize = height *  width * 4;

	}
};
#pragma pack(pop)

Vec lights[] = { Vec (7, 70, 25),
				Vec (60, 7, 7) };
triangle obj[] = {
	triangle (Vec (37, 25, -4), Vec (39, 25, -4), Vec (39, 27, -4), Vec (.0, .0, .50), 0),//back

	triangle (Vec (5, 5, 5), Vec (6, 5, 5), Vec (6, 6, 5), Vec (0.6, 0.6, 0.6), 0.91),
	triangle (Vec (6, 5, 5), Vec (7, 5, 5), Vec (7, 6, 5), Vec (0.6, 0.6, 0.6), 0.92),
	triangle (Vec (7, 5, 5), Vec (8, 5, 5), Vec (8, 6, 5), Vec (0.6, 0.6, 0.6), 0.93),
	triangle (Vec (8, 5, 5), Vec (9, 5, 5), Vec (9, 6, 5), Vec (0.6, 0.6, 0.6), 0.94),
	triangle (Vec (9, 5, 5), Vec (10, 5, 5), Vec (10, 6, 5), Vec (0.6, 0.6, 0.6), 0.95),
	triangle (Vec (10, 5, 5), Vec (11, 5, 5), Vec (11, 6, 5), Vec (0.6, 0.6, 0.6), 0.96),
	triangle (Vec (11, 5, 5), Vec (12, 5, 5), Vec (12, 6, 5), Vec (0.6, 0.6, 0.6), 0.97),
	triangle (Vec (12, 5, 5), Vec (13, 5, 5), Vec (13, 6, 5), Vec (0.6, 0.6, 0.6), 0.98),
	triangle (Vec (13, 5, 5), Vec (14, 5, 5), Vec (14, 6, 5), Vec (0.6, 0.6, 0.6), 0.99),
	triangle (Vec (14, 5, 5), Vec (15, 5, 5), Vec (15, 6, 5), Vec (0.6, 0.6, 0.6), 0.90),

	triangle (Vec (6, 5, 5), Vec (6, 6, 5), Vec (7, 6, 5), Vec (0.6, 0.6, 0.6), 0.89),
	triangle (Vec (7, 5, 5), Vec (7, 6, 5), Vec (8, 6, 5), Vec (0.6, 0.6, 0.6), 0.88),
	triangle (Vec (8, 5, 5), Vec (8, 6, 5), Vec (9, 6, 5), Vec (0.6, 0.6, 0.6), 0.87),
	triangle (Vec (9, 5, 5), Vec (9, 6, 5), Vec (10, 6, 5), Vec (0.6, 0.6, 0.6), 0.86),
	triangle (Vec (10, 5, 5), Vec (10, 6, 5), Vec (11, 6, 5), Vec (0.6, 0.6, 0.6), 0.85),
	triangle (Vec (11, 5, 5), Vec (11, 6, 5), Vec (12, 6, 5), Vec (0.6, 0.6, 0.6), 0.84),
	triangle (Vec (12, 5, 5), Vec (12, 6, 5), Vec (13, 6, 5), Vec (0.6, 0.6, 0.6), 0.83),
	triangle (Vec (13, 5, 5), Vec (13, 6, 5), Vec (14, 6, 5), Vec (0.6, 0.6, 0.6), 0.82),
	triangle (Vec (14, 5, 5), Vec (14, 6, 5), Vec (15, 6, 5), Vec (0.6, 0.6, 0.6), 0.81),

	triangle (Vec (6, 6, 5), Vec (7, 6, 5), Vec (7, 7, 5), Vec (0.6, 0.6, 0.6), 0.89),
	triangle (Vec (7, 6, 5), Vec (8, 6, 5), Vec (8, 7, 5), Vec (0.6, 0.6, 0.6), 0.88),
	triangle (Vec (8, 6, 5), Vec (9, 6, 5), Vec (9, 7, 5), Vec (0.6, 0.6, 0.6), 0.87),
	triangle (Vec (9, 6, 5), Vec (10, 6, 5), Vec (10, 7, 5), Vec (0.6, 0.6, 0.6), 0.86),
	triangle (Vec (10, 6, 5), Vec (11, 6, 5), Vec (11, 7, 5), Vec (0.6, 0.6, 0.6), 0.85),
	triangle (Vec (11, 6, 5), Vec (12, 6, 5), Vec (12, 7, 5), Vec (0.6, 0.6, 0.6), 0.84),
	triangle (Vec (12, 6, 5), Vec (13, 6, 5), Vec (13, 7, 5), Vec (0.6, 0.6, 0.6), 0.83),
	triangle (Vec (13, 6, 5), Vec (14, 6, 5), Vec (14, 7, 5), Vec (0.6, 0.6, 0.6), 0.82),
	triangle (Vec (14, 6, 5), Vec (15, 6, 5), Vec (15, 7, 5), Vec (0.6, 0.6, 0.6), 0.81),

	triangle (Vec (7, 6, 5), Vec (7, 7, 5), Vec (8, 7, 5), Vec (0.6, 0.6, 0.6), 0.79),
	triangle (Vec (8, 6, 5), Vec (8, 7, 5), Vec (9, 7, 5), Vec (0.6, 0.6, 0.6), 0.78),
	triangle (Vec (9, 6, 5), Vec (9, 7, 5), Vec (10, 7, 5), Vec (0.6, 0.6, 0.6), 0.76),
	triangle (Vec (10, 6, 5), Vec (10, 7, 5), Vec (11, 7, 5), Vec (0.6, 0.6, 0.6), 0.75),
	triangle (Vec (11, 6, 5), Vec (11, 7, 5), Vec (12, 7, 5), Vec (0.6, 0.6, 0.6), 0.74),
	triangle (Vec (12, 6, 5), Vec (12, 7, 5), Vec (13, 7, 5), Vec (0.6, 0.6, 0.6), 0.73),
	triangle (Vec (13, 6, 5), Vec (13, 7, 5), Vec (14, 7, 5), Vec (0.6, 0.6, 0.6), 0.72),
	triangle (Vec (14, 6, 5), Vec (14, 7, 5), Vec (15, 7, 5), Vec (0.6, 0.6, 0.6), 0.71),

	triangle (Vec (7, 7, 5), Vec (8, 7, 5), Vec (8, 8, 5), Vec (0.6, 0.6, 0.6), 0.79),
	triangle (Vec (8, 7, 5), Vec (9, 7, 5), Vec (9, 8, 5), Vec (0.6, 0.6, 0.6), 0.78),
	triangle (Vec (9, 7, 5), Vec (10, 7, 5), Vec (10, 8, 5), Vec (0.6, 0.6, 0.6), 0.76),
	triangle (Vec (10, 7, 5), Vec (11, 7, 5), Vec (11, 8, 5), Vec (0.6, 0.6, 0.6), 0.75),
	triangle (Vec (11, 7, 5), Vec (12, 7, 5), Vec (12, 8, 5), Vec (0.6, 0.6, 0.6), 0.74),
	triangle (Vec (12, 7, 5), Vec (13, 7, 5), Vec (13, 8, 5), Vec (0.6, 0.6, 0.6), 0.73),
	triangle (Vec (13, 7, 5), Vec (14, 7, 5), Vec (14, 8, 5), Vec (0.6, 0.6, 0.6), 0.72),
	triangle (Vec (14, 7, 5), Vec (15, 7, 5), Vec (15, 8, 5), Vec (0.6, 0.6, 0.6), 0.71),

	triangle (Vec (8, 7, 5), Vec (8, 8, 5), Vec (9, 8, 5), Vec (0.6, 0.6, 0.6), 0.69),
	triangle (Vec (9, 7, 5), Vec (9, 8, 5), Vec (10, 8, 5), Vec (0.6, 0.6, 0.6), 0.68),
	triangle (Vec (10, 7, 5), Vec (10, 8, 5), Vec (11, 8, 5), Vec (0.6, 0.6, 0.6), 0.67),
	triangle (Vec (11, 7, 5), Vec (11, 8, 5), Vec (12, 8, 5), Vec (0.6, 0.6, 0.6), 0.66),
	triangle (Vec (12, 7, 5), Vec (12, 8, 5), Vec (13, 8, 5), Vec (0.6, 0.6, 0.6), 0.65),
	triangle (Vec (13, 7, 5), Vec (13, 8, 5), Vec (14, 8, 5), Vec (0.6, 0.6, 0.6), 0.64),
	triangle (Vec (14, 7, 5), Vec (14, 8, 5), Vec (15, 8, 5), Vec (0.6, 0.6, 0.6), 0.63),

	triangle (Vec (8, 8, 5), Vec (9, 8, 5), Vec (9, 9, 5), Vec (0.6, 0.6, 0.6), 0.69),
	triangle (Vec (9, 8, 5), Vec (10, 8, 5), Vec (10, 9, 5), Vec (0.6, 0.6, 0.6), 0.68),
	triangle (Vec (10, 8, 5), Vec (11, 8, 5), Vec (11, 9, 5), Vec (0.6, 0.6, 0.6), 0.67),
	triangle (Vec (11, 8, 5), Vec (12, 8, 5), Vec (12, 9, 5), Vec (0.6, 0.6, 0.6), 0.66),
	triangle (Vec (12, 8, 5), Vec (13, 8, 5), Vec (13, 9, 5), Vec (0.6, 0.6, 0.6), 0.65),
	triangle (Vec (13, 8, 5), Vec (14, 8, 5), Vec (14, 9, 5), Vec (0.6, 0.6, 0.6), 0.64),
	triangle (Vec (14, 8, 5), Vec (15, 8, 5), Vec (15, 9, 5), Vec (0.6, 0.6, 0.6), 0.63),

	triangle (Vec (9, 8, 5), Vec (9, 9, 5), Vec (10, 9, 5), Vec (0.6, 0.6, 0.6), 0.58),
	triangle (Vec (10, 8, 5), Vec (10, 9, 5), Vec (11, 9, 5), Vec (0.6, 0.6, 0.6), 0.57),
	triangle (Vec (11, 8, 5), Vec (11, 9, 5), Vec (12, 9, 5), Vec (0.6, 0.6, 0.6), 0.56),
	triangle (Vec (12, 8, 5), Vec (12, 9, 5), Vec (13, 9, 5), Vec (0.6, 0.6, 0.6), 0.55),
	triangle (Vec (13, 8, 5), Vec (13, 9, 5), Vec (14, 9, 5), Vec (0.6, 0.6, 0.6), 0.54),
	triangle (Vec (14, 8, 5), Vec (14, 9, 5), Vec (15, 9, 5), Vec (0.6, 0.6, 0.6), 0.53),

	triangle (Vec (9, 9, 5), Vec (10, 9, 5), Vec (10, 10, 5), Vec (0.6, 0.6, 0.6), 0.58),
	triangle (Vec (10, 9, 5), Vec (11, 9, 5), Vec (11, 10, 5), Vec (0.6, 0.6, 0.6), 0.57),
	triangle (Vec (11, 9, 5), Vec (12, 9, 5), Vec (12, 10, 5), Vec (0.6, 0.6, 0.6), 0.56),
	triangle (Vec (12, 9, 5), Vec (13, 9, 5), Vec (13, 10, 5), Vec (0.6, 0.6, 0.6), 0.55),
	triangle (Vec (13, 9, 5), Vec (14, 9, 5), Vec (14, 10, 5), Vec (0.6, 0.6, 0.6), 0.54),
	triangle (Vec (14, 9, 5), Vec (15, 9, 5), Vec (15, 10, 5), Vec (0.6, 0.6, 0.6), 0.53),

	triangle (Vec (10, 9, 5), Vec (10, 10, 5), Vec (11, 10, 5), Vec (0.6, 0.6, 0.6), 0.46),
	triangle (Vec (11, 9, 5), Vec (11, 10, 5), Vec (12, 10, 5), Vec (0.6, 0.6, 0.6), 0.47),
	triangle (Vec (12, 9, 5), Vec (12, 10, 5), Vec (13, 10, 5), Vec (0.6, 0.6, 0.6), 0.48),
	triangle (Vec (13, 9, 5), Vec (13, 10, 5), Vec (14, 10, 5), Vec (0.6, 0.6, 0.6), 0.49),
	triangle (Vec (14, 9, 5), Vec (14, 10, 5), Vec (15, 10, 5), Vec (0.6, 0.6, 0.6), 0.40),

	triangle (Vec (10, 10, 5), Vec (11, 10, 5), Vec (11, 11, 5), Vec (0.6, 0.6, 0.6), 0.46),
	triangle (Vec (11, 10, 5), Vec (12, 10, 5), Vec (12, 11, 5), Vec (0.6, 0.6, 0.6), 0.47),
	triangle (Vec (12, 10, 5), Vec (13, 10, 5), Vec (13, 11, 5), Vec (0.6, 0.6, 0.6), 0.48),
	triangle (Vec (13, 10, 5), Vec (14, 10, 5), Vec (14, 11, 5), Vec (0.6, 0.6, 0.6), 0.49),
	triangle (Vec (14, 10, 5), Vec (15, 10, 5), Vec (15, 11, 5), Vec (0.6, 0.6, 0.6), 0.41),

	triangle (Vec (11, 10, 5), Vec (11, 11, 5), Vec (12, 11, 5), Vec (0.6, 0.6, 0.6), 0.37),
	triangle (Vec (12, 10, 5), Vec (12, 11, 5), Vec (13, 11, 5), Vec (0.6, 0.6, 0.6), 0.38),
	triangle (Vec (13, 10, 5), Vec (13, 11, 5), Vec (14, 11, 5), Vec (0.6, 0.6, 0.6), 0.39),
	triangle (Vec (14, 10, 5), Vec (14, 11, 5), Vec (15, 11, 5), Vec (0.6, 0.6, 0.6), 0.30),

	triangle (Vec (11, 11, 5), Vec (12, 11, 5), Vec (12, 12, 5), Vec (0.6, 0.6, 0.6), 0.37),
	triangle (Vec (12, 11, 5), Vec (13, 11, 5), Vec (13, 12, 5), Vec (0.6, 0.6, 0.6), 0.38),
	triangle (Vec (13, 11, 5), Vec (14, 11, 5), Vec (14, 12, 5), Vec (0.6, 0.6, 0.6), 0.39),
	triangle (Vec (14, 11, 5), Vec (15, 11, 5), Vec (15, 12, 5), Vec (0.6, 0.6, 0.6), 0.31),

	triangle (Vec (12, 11, 5), Vec (12, 12, 5), Vec (13, 12, 5), Vec (0.6, 0.6, 0.6), 0.28),
	triangle (Vec (13, 11, 5), Vec (13, 12, 5), Vec (14, 12, 5), Vec (0.6, 0.6, 0.6), 0.29),
	triangle (Vec (14, 11, 5), Vec (14, 12, 5), Vec (15, 12, 5), Vec (0.6, 0.6, 0.6), 0.20),

	triangle (Vec (12, 12, 5), Vec (13, 12, 5), Vec (13, 13, 5), Vec (0.6, 0.6, 0.6), 0.28),
	triangle (Vec (13, 12, 5), Vec (14, 12, 5), Vec (14, 13, 5), Vec (0.6, 0.6, 0.6), 0.29),
	triangle (Vec (14, 12, 5), Vec (15, 12, 5), Vec (15, 13, 5), Vec (0.6, 0.6, 0.6), 0.21),

	triangle (Vec (13, 12, 5), Vec (13, 13, 5), Vec (14, 13, 5), Vec (0.6, 0.6, 0.6), 0.19),
	triangle (Vec (14, 12, 5), Vec (14, 13, 5), Vec (15, 13, 5), Vec (0.6, 0.6, 0.6), 0.10),

	triangle (Vec (14, 13, 5), Vec (14, 14, 5), Vec (15, 14, 5), Vec (0.6, 0.6, 0.6), 0.05),

	triangle (Vec (13, 13, 5), Vec (14, 13, 5), Vec (14, 14, 5), Vec (0.6, 0.6, 0.6), 0.19),
	triangle (Vec (14, 13, 5), Vec (15, 13, 5), Vec (15, 14, 5), Vec (0.6, 0.6, 0.6), 0.11),

	triangle (Vec (14, 14, 5), Vec (15, 14, 5), Vec (15, 15, 5), Vec (0.6, 0.6, 0.6), 0.04),



	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0),//down
	//triangle(Vec(5,15,5	),Vec(15,15,5	),Vec(15,15,15	),Vec(.50,.15,.0),0),//up
	//triangle(Vec(5,15,5	),Vec(5,15,15	),Vec(15,15,15	),Vec(.50,.0,.15),0),//up																	 
	//triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.10, .10, .10), 0.85),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.10, .10, .10), 0.85),//back
	//triangle(Vec(5,5,15	),Vec(15,5,15	),Vec(15,15,15	),Vec(.0,.10,.50),0),//front
	//triangle(Vec(5,5,15	),Vec(5,15,15	),Vec(15,15,15	),Vec(.10,.0,.50),0),//front																	 
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.50, .50, .0), 0.6),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .50), 0),//left
	//triangle(Vec(15,5,5	),Vec(15,5,15	),Vec(15,15,15	),Vec(.10,.50,.0),0.60),//right
	//triangle(Vec(15,5,5	),Vec(15,15,5	),Vec(15,15,15	),Vec(.0,.50,.10),0.60),//right

	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15),//down
	triangle (Vec (5, 15, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.50, .15, .0), 0).moveX (15),//up
	triangle (Vec (5, 15, 5), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.50, .0, .15), 0).moveX (15),//up																	 
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15),//back
	triangle (Vec (5, 5, 15), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.0, .10, .50), 0).moveX (15),//front
	triangle (Vec (5, 5, 15), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.10, .0, .50), 0).moveX (15),//front																	 
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15),//left
	triangle (Vec (15, 5, 5), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.10, .50, .0), 0).moveX (15),//right
	triangle (Vec (15, 5, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.0, .50, .10), 0).moveX (15),//right

	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveZ (-15),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveZ (-15),//down
	triangle (Vec (5, 15, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.50, .15, .0), 0).moveZ (-15),//up
	triangle (Vec (5, 15, 5), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.50, .0, .15), 0).moveZ (-15),//up																	 	   
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveZ (-15),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveZ (-15),//back
	triangle (Vec (5, 5, 15), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.0, .10, .50), 0).moveZ (-15),//front
	triangle (Vec (5, 5, 15), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.10, .0, .50), 0).moveZ (-15),//front																	 	  
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveZ (-15),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveZ (-15),//left
	triangle (Vec (15, 5, 5), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.10, .50, .0), 0).moveZ (-15),//right
	triangle (Vec (15, 5, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.0, .50, .10), 0).moveZ (-15),//right

	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15).moveZ (-15),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15).moveZ (-15),//down
	triangle (Vec (5, 15, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.50, .15, .0), 0).moveX (15).moveZ (-15),//up
	triangle (Vec (5, 15, 5), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.50, .0, .15), 0).moveX (15).moveZ (-15),//up																	 
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15).moveZ (-15),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15).moveZ (-15),//back
	triangle (Vec (5, 5, 15), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.0, .10, .50), 0).moveX (15).moveZ (-15),//front
	triangle (Vec (5, 5, 15), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.10, .0, .50), 0).moveX (15).moveZ (-15),//front																	 
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15).moveZ (-15),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15).moveZ (-15),//left
	triangle (Vec (15, 5, 5), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.10, .50, .0), 0).moveX (15).moveZ (-15),//right
	triangle (Vec (15, 5, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.0, .50, .10), 0).moveX (15).moveZ (-15),//right

	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveY (15),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveY (15),//down
	triangle (Vec (5, 15, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.50, .15, .0), 0).moveY (15),//up
	triangle (Vec (5, 15, 5), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.50, .0, .15), 0).moveY (15),//up																	 
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveY (15),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveY (15),//back
	triangle (Vec (5, 5, 15), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.0, .10, .50), 0).moveY (15),//front
	triangle (Vec (5, 5, 15), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.10, .0, .50), 0).moveY (15),//front																	 
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveY (15),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveY (15),//left
	triangle (Vec (15, 5, 5), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.10, .50, .0), 0).moveY (15),//right
	triangle (Vec (15, 5, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.0, .50, .10), 0).moveY (15),//right

	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15).moveY (15),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15).moveY (15),//down
	triangle (Vec (5, 15, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.50, .15, .0), 0).moveX (15).moveY (15),//up
	triangle (Vec (5, 15, 5), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.50, .0, .15), 0).moveX (15).moveY (15),//up																	 
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15).moveY (15),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15).moveY (15),//back
	triangle (Vec (5, 5, 15), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.0, .10, .50), 0).moveX (15).moveY (15),//front
	triangle (Vec (5, 5, 15), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.10, .0, .50), 0).moveX (15).moveY (15),//front																	 
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15).moveY (15),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15).moveY (15),//left
	triangle (Vec (15, 5, 5), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.10, .50, .0), 0).moveX (15).moveY (15),//right
	triangle (Vec (15, 5, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.0, .50, .10), 0).moveX (15).moveY (15),//right

	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveZ (-15).moveY (15),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveZ (-15).moveY (15),//down
	triangle (Vec (5, 15, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.50, .15, .0), 0).moveZ (-15).moveY (15),//up
	triangle (Vec (5, 15, 5), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.50, .0, .15), 0).moveZ (-15).moveY (15),//up																	 	   
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveZ (-15).moveY (15),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveZ (-15).moveY (15),//back
	triangle (Vec (5, 5, 15), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.0, .10, .50), 0).moveZ (-15).moveY (15),//front
	triangle (Vec (5, 5, 15), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.10, .0, .50), 0).moveZ (-15).moveY (15),//front																	 	  
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveZ (-15).moveY (15),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveZ (-15).moveY (15),//left
	triangle (Vec (15, 5, 5), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.10, .50, .0), 0).moveZ (-15).moveY (15),//right
	triangle (Vec (15, 5, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.0, .50, .10), 0).moveZ (-15).moveY (15),//right

	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15).moveZ (-15).moveY (15),//down
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (15, 5, 15), Vec (.50, .0, .0), 0).moveX (15).moveZ (-15).moveY (15),//down
	triangle (Vec (5, 15, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.50, .15, .0), 0).moveX (15).moveZ (-15).moveY (15),//up
	triangle (Vec (5, 15, 5), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.50, .0, .15), 0).moveX (15).moveZ (-15).moveY (15),//up																	 
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15).moveZ (-15).moveY (15),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.0, .0, .50), 0).moveX (15).moveZ (-15).moveY (15),//back
	triangle (Vec (5, 5, 15), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.0, .10, .50), 0).moveX (15).moveZ (-15).moveY (15),//front
	triangle (Vec (5, 5, 15), Vec (5, 15, 15), Vec (15, 15, 15), Vec (.10, .0, .50), 0).moveX (15).moveZ (-15).moveY (15),//front																	 
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15).moveZ (-15).moveY (15),//left
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (5, 15, 15), Vec (.0, .50, .0), 0).moveX (15).moveZ (-15).moveY (15),//left
	triangle (Vec (15, 5, 5), Vec (15, 5, 15), Vec (15, 15, 15), Vec (.10, .50, .0), 0).moveX (15).moveZ (-15).moveY (15),//right
	triangle (Vec (15, 5, 5), Vec (15, 15, 5), Vec (15, 15, 15), Vec (.0, .50, .10), 0).moveX (15).moveZ (-15).moveY (15)//right

};


//struct Sphere {
//	double distance;       // distanceius 
//	Vec p, e, c;      // position, emission, color 
//	Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive) 
//	Sphere(double distance_, Vec p_, Vec e_, Vec c_, Refl_t refl_) :
//		distance(distance_), p(p_), e(e_), c(c_), refl(refl_) {}
//	double intersect(const Ray &r) const 
//	{ // returns distance, 0 if nohit 
//		Vec op = p - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
//		double t, eps = 1e-4, b = op.dot(r.d), det = b*b - op.dot(op) + distance*distance;
//		if (det<0) return 0; else det = sqrt(det);
//		return (t = b - det)>eps ? t : ((t = b + det) > eps ? t : 0);
//	}
//};
//
//Sphere spheres[] = {//Scene: distanceius, position, emission, color, material 
//	Sphere(1e5, Vec(1e5 + 1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left 
//	Sphere(1e5, Vec(-1e5 + 99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght 
//	Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back 
//	Sphere(1e5, Vec(50,40.8,-1e5 + 170), Vec(),Vec(),           DIFF),//Frnt 
//	Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm 
//	Sphere(1e5, Vec(50,-1e5 + 81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top 
//	Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr 
//	Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas 
//	Sphere(600, Vec(50,681.6 - .27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite 
//};

inline double	clamp  (double x) { return x < 0 ? 0 : x>1 ? 1 : x; }
inline int		toInt  (double x) { return int  (pow (clamp (x), 1 / 2.2) * 255 + .5); }
inline char		toChar (double x) { return char (pow (clamp (x), 1 / 2.2) * 255); }



bool helpOrient (triangle t, Vec v1, Vec v2)
{

	Vec normalization = t.normal ();
	double x1 = normalization.dot (v1 - t.a1);
	double x2 = normalization.dot (v2 - t.a1);
	return x1*x2 + EPSILON >= 0;
}
/*
Returns true if input ray intersect some object
*/


inline __host__ __device__ void intersect (const triangle * objects, const unsigned int objCount,
	const Ray &r, double &t, int &id, Vec & hit, bool * isIntersect)
{
	Vec lockalHit;
	double  d; 
	t = INF;
	for (unsigned int i = objCount; i--;)
	{
		//bool isSpherIntersection = objects[i].intersectSpher (r);
		//if (isSpherIntersection)
		{
			bool isIntersection = objects[i].intersect (r, lockalHit);
			if (isIntersection)
			{
				d = lockalHit.distance (r.o);
				if (d < t)
				{
					t = d;
					id = i;
					hit = lockalHit;
				}
			}
		}
	}
	*isIntersect =  t < INF;
}
inline __host__ __device__ bool intersectHelper (const triangle * objects, const unsigned int objCount,
	const Ray &r, double &t, int &id, Vec & hit)
{
	bool  isIntersect = 0;
	intersect (objects, objCount, r, t, id, hit, &isIntersect);
	return isIntersect;
}
/*
Returns true if the light source is visible
*/
inline __host__ __device__ bool Visible (const  world & wrld, const Vec & hit, const Vec & light, const int & id)
{
	double distToLight = hit.distance (light);
	Vec hit1;
	Vec sub = light - hit;
	Ray r (hit, sub);
	double distanse;
	int id1 = -1;
	bool isIntersection = intersectHelper (wrld.objects, wrld.objCount, r, distanse, id1, hit1);
	if (isIntersection)
	{
		if (id == id1)return true;
		return false;
	}
	else return true;

}
inline __host__ __device__ Vec Shade (const Vec & hit, const Vec & light)
{
	//double ka = 0.1; //ambient coefficient
	return Vec (0.3, 0.3, 0.3);
}
/*
Return new reflected ray
*/
inline __host__ __device__ Ray reflect (const Ray & r,const  triangle & obj,const Vec & hit)
{
	Vec normal = obj.normal ();
	Vec iV = hit - r.o;//inputVector
	normal = normal.normalization ();
	Vec rV = iV - ((normal*(iV.dot (normal))) * 2);//reflectVector
	Ray reflect (hit, rV.normalization ());//new reflect ray
	return reflect;
}
/*
Recursively trace the input ray with a light source and reflection
*/
__host__ __device__ Vec RayTrace (const  world  & wrld,const Ray & ray,unsigned int deep)
{
	Vec color (0, 0, 0);
	int id = 0;
	Vec hit;// найдем полигон
	double distanse ;
	bool isIntersection = intersectHelper (wrld.objects, wrld.objCount, ray, distanse, id, hit);
	
	if (!isIntersection)
		return color;
	triangle tr = wrld.objects[id];
	color = tr.c;

	unsigned int lC = wrld.lightsCount;
	for (unsigned int i = 0; i < lC; ++i)
	{//проверим освещенность
		bool isVisible = Visible (wrld, hit, wrld.lights[i], id);
		if (isVisible)
		{
			Vec light = wrld.lights[i] - hit;
			double distancei = wrld.lights[i].distance (hit);
			double cos = abs ((light.dot (tr.normal ().normalization ())) / (distancei));
			color = color + color*(1 / (distancei*distancei));
		}
		else
		{
			color = color - color*0.8;
		}
	}

	//	

	if (tr.reflect > 0 && deep > 0)//найдем отражение
	{
		Ray reflRay = reflect (ray, tr, hit);
		color = color*(1.0 - tr.reflect) + RayTrace (wrld, reflRay, deep--)*tr.reflect;
	}
	return color;
}
//Vec distanceiance(const Ray &r, int depth, unsigned short *Xi) 
//{
//	
//	double t;                               // distance to intersection 
//	int id = 0;                               // id of intersected object 
//	if (!intersect(r, t, id)) return Vec(); // if miss, return black 
//	const Sphere &obj = spheres[id];        // the hit object 
//	Vec x = r.o + r.d*t, n = (x - obj.p).normalization(), nl = n.dot(r.d) < 0 ? n : n*-1, f = obj.c;
//	double p = f.x > f.y && f.x>f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl 
//	if (++depth > 5) 
//		if (erand48(Xi) < p) f = f*(1 / p); 
//	else return obj.e; //R.R. 
//	if (obj.refl == DIFF) 
//	{                  // Ideal DIFFUSE reflection 
//		double r1 = 2 * 3.14159265358979323846 *erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
//		Vec w = nl, u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).normalization(), v = w%u;
//		Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1 - r2)).normalization();
//		return obj.e + f.mult(distanceiance(Ray(x, d), depth, Xi));
//	}
//	else if (obj.refl == SPEC)            // Ideal SPECULAR reflection 
//		return obj.e + f.mult(distanceiance(Ray(x, r.d - n * 2 * n.dot(r.d)), depth, Xi));
//	Ray reflRay(x, r.d - n * 2 * n.dot(r.d));     // Ideal dielectric REFRACTION 
//	bool into = n.dot(nl) > 0;                // Ray from outside going in? 
//	double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.d.dot(nl), cos2t;
//	if ((cos2t = 1 - nnt*nnt*(1 - ddn*ddn)) < 0)    // Total internal reflection 
//		return obj.e + f.mult(distanceiance(reflRay, depth, Xi));
//	Vec tdir = (r.d*nnt - n*((into ? 1 : -1)*(ddn*nnt + sqrt(cos2t)))).normalization();
//	double a = nt - nc, b = nt + nc, R0 = a*a / (b*b), c = 1 - (into ? -ddn : tdir.dot(n));
//	double Re = R0 + (1 - R0)*c*c*c*c*c, Tr = 1 - Re, P = .25 + .5*Re, RP = Re / P, TP = Tr / (1 - P);
//	return obj.e + f.mult(depth > 2 ? (erand48(Xi) < P ?   // Russian roulette 
//		distanceiance(reflRay, depth, Xi)*RP : distanceiance(Ray(x, tdir), depth, Xi)*TP) :
//		distanceiance(reflRay, depth, Xi)*Re + distanceiance(Ray(x, tdir), depth, Xi)*Tr);
//}
inline void fileOutput (Vec *c, const unsigned  int w, const unsigned int h)
{
#ifdef ppm
	FILE *f = fopen ("image.ppm", "w");         // Write image to PPM file. 
	fprintf (f, "P3\n%d %d\n%d\n", w, h, 255);
	for (unsigned int i = 0; i < w*h; ++i)
		fprintf (f, "%d %d %d ", toInt (c[i].x), toInt (c[i].y), toInt (c[i].z));
#endif

#ifdef bmp
	bmpinfo first (w, h);
	FILE *f = fopen ("temp1.bmp", "wb");  // 

	fwrite (&first, sizeof(bmpinfo), 1, f);
	int step = (w * 3 + 3) / 4 * 4;
	unsigned char *data = new unsigned char[step*h];

	for (int i = h - 1; i >= 0; i--)
	{
		unsigned char *p = data + step * (h - i);

		for (int j = 0; j < w; j++)
		{
			*p++ = toChar (c[i*w + j].z);
			*p++ = toChar (c[i*w + j].y);
			*p++ = toChar (c[i*w + j].x);
		}
	}

	fwrite (data, step*h, 1, f);
	delete[] data;


	//int n = sizeof(first);
	//char * b = new char[n];
	//memcpy (b, &first, n);
	//for (int i = 0; i < n; i++)fprintf (f, "%c", b[i]);
	////unsigned char x = 0;

	//for (int i = h - 1; i >= 0; i--)
	//{
	//	for (int j = 0; j < w; j++)
	//	{

	//		fprintf (f, "%c%c%c", toChar (c[i*w + j].x), toChar (c[i*w + j].z), toChar (c[i*w + j].y));//rbg
	//	}
	//}

	fclose (f);
#endif
}

Ray * RenderRayHelper (const camera & cam, const imgSettings & img)
{
	Vec r;
	int kFilter = 4;
#ifdef sampleInCenter
	kFilter = 1;
#endif
	int RaysCount = img.h*img.w*kFilter;
	Ray * allRays = (Ray *)malloc (sizeof(Ray)*RaysCount);
	int iii = 0;
//#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP 
	for (unsigned int y = 0; y < img.h; ++y) // Loop over image rows 
	{
		//fprintf (stderr, "\rRendering (%d spp) %5.2f%%", img.samps * 4, 100.*y / (img.h - 1));
		for (unsigned int x = 0; x < img.w; ++x)   // Loop cols 
		{
			int i = (img.h - y - 1)*img.w + x;
#ifdef sampleInCenter
			double sx = 0.5, sy = 0.5;
			double k = 1;
#else
			double k = .25;
			for (unsigned int sy = 0; sy < 2; ++sy) // 2x2 subpixel rows 
			for (unsigned int sx = 0; sx < 2; ++sx) // 2x2 subpixel cols	
#endif
			{
				r = Vec ();
				for (unsigned int sa = 0; sa < img.samps; ++sa)//quality of image
				{
#ifdef filter
					double r1 = 2 * (double)rand () / (double)RAND_MAX, dx = r1 < 1 ? sqrt (r1) - 1 : 1 - sqrt (2 - r1);
					double r2 = 2 * (double)rand () / (double)RAND_MAX, dy = r2 < 1 ? sqrt (r2) - 1 : 1 - sqrt (2 - r2);
#else
					double dx = 0, dy = 0;
#endif
					Vec d = cam.cameraXangle*(((sx + .5 + dx) / 2 + x) / img.w - .5) +
							cam.cameraYangle*(((sy + .5 + dy) / 2 + y) / img.h - .5) +
							cam.cameraLocation.d;
					allRays[i] = Ray (cam.cameraLocation.o + d * 140, d.normalization ());
					i++;
				}
			}

		}
	}
	return allRays;
}
void renderNEW (const Ray * allRays, const  world & wrld, const imgSettings & img, Vec *c)
{
	Vec r;
	for (int i = 0; i < img.h*img.w; i++)
	{
		fprintf (stderr, "\rRendering (%d spp) %5.2f%%", img.samps * 4, (100.*i )/ (img.h*img.w));
#ifdef sampleInCenter
		for (unsigned int sa = 0; sa < img.samps; ++sa)//quality of image
		{
			r = r + RayTrace (wrld, allRays[i], 5)*(1. / img.samps);
		}
#else
		for (unsigned int sa = 0; sa < img.samps; ++sa)//quality of image
		{
			r = r + RayTrace (wrld, allRays[i*4], 5)*(1. / img.samps);
			r = r + RayTrace (wrld, allRays[i*4+1], 5)*(1. / img.samps);
			r = r + RayTrace (wrld, allRays[i*4+2], 5)*(1. / img.samps);
			r = r + RayTrace (wrld, allRays[i*4+3], 5)*(1. / img.samps);
		}
		r = r*0.25;
#endif
		

		c[i] = c[i] + Vec (clamp (r.x), clamp (r.y), clamp (r.z));
	}
}
inline void Render (const  world & wrld, const camera & cam, Vec *c, const imgSettings & img)
{
	Vec r; int i = 0;
#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP 
	for (unsigned int y = 0; y < img.h; ++y) // Loop over image rows 
	{
		fprintf (stderr, "\rRendering (%d spp) %5.2f%%", img.samps * 4, 100.*y / (img.h - 1));
		for (unsigned int x = 0 /*,Xi[3] = { 0, 0, y*y*y }*/; x < img.w; ++x)   // Loop cols 
		{
			//int i = (img.h - y - 1)*img.w + x;
#ifdef sampleInCenter
			double sx = 0.5, sy = 0.5;
			double k = 1;				
#else
			double k = .25;
			for (unsigned int sy = 0; sy < 2; ++sy)     // 2x2 subpixel rows 
			for (unsigned int sx = 0; sx < 2; ++sx) // 2x2 subpixel cols	
#endif
			{
				r = Vec ();
				for (unsigned int sa = 0; sa < img.samps; ++sa)//quality of image
				{
#ifdef filter
					double r1 = 2 * (double)rand () / (double)RAND_MAX, dx = r1 < 1 ? sqrt (r1) - 1 : 1 - sqrt (2 - r1);
					double r2 = 2 * (double)rand () / (double)RAND_MAX, dy = r2 < 1 ? sqrt (r2) - 1 : 1 - sqrt (2 - r2);
#else
					double dx = 0, dy = 0;
#endif
					Vec d = cam.cameraXangle*(((sx + .5 + dx) / 2 + x) / img.w - .5) +
						cam.cameraYangle*(((sy + .5 + dy) / 2 + y) / img.h - .5) +
						cam.cameraLocation.d;
					r = r + RayTrace (wrld, Ray (cam.cameraLocation.o + d * 140, d.normalization ()), 5)*(1. / img.samps);
				}
				c[i] = c[i] + Vec (clamp (r.x), clamp (r.y), clamp (r.z))*k;
				
			}
			i++;
		}
	}
}
int main (int argc, char *argv[])
{
	//int vec = sizeof(Vec);
	//int tr = sizeof(triangle);
	int w = 1024, h = 768, samps = argc == 2 ? atoi (argv[1]) / 4 : 1; // # samples 

	//Ray cam (Vec (85, 45, 170), Vec (-0.45, -0.04, -1).normalization ()); // cam pos, dir 
	camera cam (Ray (Vec (85, 45, 170), Vec (-0.45, -0.04, -1).normalization ()), Vec (w*.5135 / h));
	Vec  r, *c = new Vec[w*h];//cx = Vec (w*.5135 / h), cy = (cx%cam.d).normalization ()*.5135,
	imgSettings img = imgSettings (w, h, samps);
	/*cudaDeviceProp  prop ;
	 cudaGetDeviceProperties (&prop,0);
	 dim3 GRID= dim3 (1024, 1024, 1024);
	 dim3 THREADS= dim3 (512, 512, 64);
	 cudaThreadSynchronize ();*/

	int objCount = sizeof(obj) / sizeof(triangle);


	//memccpy (&crazy [ count], obj, objCount, sizeof(triangle));
	//int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = sizeof(lights) / sizeof(Vec);
	world wrld = world (objCount, lightsCount, obj,lights);
	
	
	
	//выделяем память для device копий для OBJECTS
	int size = objCount*sizeof(triangle);
	cudaMalloc ((void**)&CUDA_objects, size);
	// копируем ввод на device
	cudaMemcpy (CUDA_objects, &obj, size, cudaMemcpyHostToDevice);
	//Ray * allRays = RenderRayHelper (cam, img);
	double start = omp_get_wtime ();
	
	//renderNEW (allRays, wrld, img, c);
	Render/* <<<GRID, THREADS >>> */ (wrld, cam, c, img);
	double end = omp_get_wtime ();

	//free (allRays);
	cudaFree (CUDA_objects);
	//cudaThreadSynchronize ();
	fprintf (stderr, "\rtime %5.2f%%", end-start);

	fileOutput (c, w, h);
}