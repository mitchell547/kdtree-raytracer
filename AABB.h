#include "Vec.hpp"
#include "Ray.hpp"
#include "Triangle.hpp"
#include <algorithm>

/*struct AABB {
	Vec min;
	Vec max;
	AABB() : min(Vec(0, 0, 0)), max(Vec(0, 0, 0)) {}
	AABB(Vec _min, Vec _max) : min(_min), max(_max) { }
};
*/

struct AABB {
	float3 min;
	float3 max;
	//AABB() : min(float3(0, 0, 0)), max(float3(0, 0, 0)) {}
	//AABB(float3 _min, float3 _max) : min(_min), max(_max) { }
	AABB() {min = float3(0, 0, 0); max = float3(0, 0, 0);};
	AABB(float3 _min, float3 _max) { min = _min; max = _max; };
};

float3 getSizes(const AABB & box) {
	return box.max - box.min;
}

float getSurfaceArea(const AABB & box) {
	float3 size = box.max - box.min;
	return (size.v[X] * size.v[Y] + size.v[Y] * size.v[Z] + size.v[X] * size.v[Z]) * 2;
}

float getSurfaceArea(const float3 & size) {
	return (size.v[X] * size.v[Y] + size.v[Y] * size.v[Z] + size.v[X] * size.v[Z]) * 2;
}

/*AABB getTriangleAABB(triangle ABC) {
	float minx = std::min(ABC.a1.x, std::min(ABC.a2.x, ABC.a3.x));
	float miny = std::min(ABC.a1.y, std::min(ABC.a2.y, ABC.a3.y));
	float minz = std::min(ABC.a1.z, std::min(ABC.a2.z, ABC.a3.z));

	float maxx = std::max(ABC.a1.x, std::max(ABC.a2.x, ABC.a3.x));
	float maxy = std::max(ABC.a1.y, std::max(ABC.a2.y, ABC.a3.y));
	float maxz = std::max(ABC.a1.z, std::max(ABC.a2.z, ABC.a3.z));

	return AABB(Vec(minx, miny, minz), Vec(maxx, maxy, maxz));
}
*/

AABB getTriangleAABB(triangle & ABC) {
	float minx = min(ABC.p[0].v[0], min(ABC.p[1].v[0], ABC.p[2].v[0]));
	float miny = min(ABC.p[0].v[1], min(ABC.p[1].v[1], ABC.p[2].v[1]));
	float minz = min(ABC.p[0].v[2], min(ABC.p[1].v[2], ABC.p[2].v[2]));

	float maxx = max(ABC.p[0].v[0], max(ABC.p[1].v[0], ABC.p[2].v[0]));
	float maxy = max(ABC.p[0].v[1], max(ABC.p[1].v[1], ABC.p[2].v[1]));
	float maxz = max(ABC.p[0].v[2], max(ABC.p[1].v[2], ABC.p[2].v[2]));

	return AABB(float3(minx, miny, minz), float3(maxx, maxy, maxz));
}

void addAABB(AABB & a, AABB & b) {
	/*a.min.x = std::min(a.min.x, b.min.x);
	a.min.y = std::min(a.min.y, b.min.y);
	a.min.z = std::min(a.min.z, b.min.z);

	a.max.x = std::max(a.max.x, b.max.x);
	a.max.y = std::max(a.max.y, b.max.y);
	a.max.z = std::max(a.max.z, b.max.z);
	*/
	for (int i = 0; i < 3; ++i) {
		a.min.v[i] = min(a.min.v[i], b.min.v[i]);
		a.max.v[i] = max(a.max.v[i], b.max.v[i]);
	}
}

/*bool RayAABBIntersect(Ray ray, AABB box, float & tmin, float & tmax) {
	Vec inv_dir = 1.0 / ray.d;
	float lo = inv_dir.x*(box.min.x - ray.o.x);
	float hi = inv_dir.x*(box.max.x - ray.o.x);
	tmin = std::min(lo, hi);
	tmax = std::max(lo, hi);

	lo = inv_dir.y*(box.min.y - ray.o.y);
	hi = inv_dir.y*(box.max.y - ray.o.y);
	tmin = std::max(tmin, std::min(lo, hi));
	tmax = std::min(tmax, std::max(lo, hi));
 
	lo = inv_dir.z*(box.min.z - ray.o.z);
	hi = inv_dir.z*(box.max.z - ray.o.z);
	tmin = std::max(tmin, std::min(lo, hi));
	tmax = std::min(tmax, std::max(lo, hi));
 
	return (tmin <= tmax) && (tmax > 0.f);
}
*/

bool RayAABBIntersect(const Ray & ray, const AABB & box, float & tmin, float & tmax) {
	float3 inv_dir = 1.0 / ray.d;
	float lo, hi;
	tmin = -INF;
	tmax = INF;
	for (int i = 0; i < 3; ++i) {
		lo = inv_dir.v[i]*(box.min.v[i] - ray.o.v[i]);
		hi = inv_dir.v[i]*(box.max.v[i] - ray.o.v[i]);
		tmin = max(tmin, min(lo, hi));
		tmax = min(tmax, max(lo, hi));
	}
 
	return (tmin <= tmax) && (tmax > 0.f);
}