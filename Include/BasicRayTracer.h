#include <stdio.h>
#include <math.h>   
#include <stdlib.h> 
#include <omp.h>
#include "Include/Geometry/Vec.hpp"
#include "Include/Geometry/Ray.hpp"
#include "Include/Geometry/Triangle.hpp"
//#include "BMP.hpp"
#include "Include/raytracer_misc.h"
#include "Include/World.hpp"
#define INF 1e20

#pragma once

// Main rendering method
// Основной метод рендеринга сцены (трассировка всех лучей и получение цветов всех пикселей)
void SimpleRender (const  world & wrld, const camera & cam, Vec c[], const imgSettings & img);

// Single ray tracing
// Трассировка конкретного луча по сцене (поиск пересечений, освещённость, отражения)
Vec RayTrace (const  world  & wrld,const Ray & ray,unsigned int deep);

// Check light source visibility
// Проверка видимости источника света из точки пересечения
inline   bool Visible (const  world & wrld, const Vec & hit, const Vec & light, const int & id);

/*bool helpOrient (triangle t, Vec v1, Vec v2) {
	Vec normalization = t.normal ();
	double x1 = normalization.dot (v1 - t.p[0]);
	double x2 = normalization.dot (v2 - t.p[0]);
	return x1*x2 + EPSILON >= 0;
}
*/

/*inline   Vec Shade (const Vec & hit, const Vec & light) {
	//double ka = 0.1; //ambient coefficient
	return Vec (0.3, 0.3, 0.3);
}
*/

float facingRatio(const triangle & tr, const Ray & ray) {
	return max(0, tr._normal.norm().dot((float3()-ray.d).norm()));
}

float3 smoothNormal(const float3 v_n[3], const float3 & bari) {
	return (1 - bari.v[0] - bari.v[1]) * v_n[0] + bari.v[0] * v_n[1] + bari.v[1] * v_n[2];
}


/*
Return new reflected ray
Получение отражённого луча
*/
inline   Ray reflect (const Ray & r,const  triangle & obj,const Vec & hit, const Vec & bari) {
	//Vec normal = obj.normal ();
	float3 normal = smoothNormal(obj.v_n, bari);
	Vec iV = hit - Vec(r.o);//inputVector
	normal = normal.normalization ();
	Vec rV = iV - ((normal*(iV.dot (normal))) * 2);//reflectVector
	rV.normalization();
	Ray reflect (hit + rV*0.01, rV);//new reflect ray
	return reflect;
}

// Поиск пересечений луча с треугольниками сцены
inline   bool findIntersection (const triangle objects[], const unsigned int objCount,
	const Ray &r, double &t, int &id, float3 & hit, float3 & bari)
{
	float3 localHit, uv;
	double  d; 
	t = INF;
	for (unsigned int i = objCount; i--;)
	{
	#ifndef MOLLER_TRUMBORE_INTERSECT
		bool isIntersection = objects[i].intersect (r, localHit); 
	#else
		bool isIntersection = objects[i].mollerTrumboreIntersect(r, localHit, uv); 
	#endif
			
		if (isIntersection)
		{
			d = localHit.distance (r.o); 
			if (d < t)
			{
				t = d;
				id = i;
				hit = localHit;
				bari = uv;
			}
		}
		
	}
	return t < INF;
}

/*inline   bool intersectHelper (const triangle * objects, const unsigned int objCount,
	const Ray &r, double &t, int &id, float3 & hit)
{
	bool  isIntersect = 0;
	intersect (objects, objCount, r, t, id, hit, &isIntersect);
	return isIntersect;
}*/


/*
Returns true if the light source is visible
*/
// For standard raytracing
inline   bool Visible (const  world & wrld, const Vec & hit, const Vec & light, const int & id)
{
	double distToLight = hit.distance (light);
	float3 hit1;
	Vec sub = light - hit;
	//Vec sub = hit - light;
	sub.normalization();
	Ray r (hit + sub * 0.001, sub);
	//Ray r (hit, sub);
	double distance;
	float3 bari;
	int id1 = -1;
	bool isIntersection = findIntersection (wrld.objects, wrld.objCount, r, distance, id1, hit1, bari);
	if (!isIntersection)
		return true;
	else {
		if (id == id1) return true;
		if (distToLight + EPSILON < distance) return true;
		return false;
	}
	/*if (isIntersection)
	{
		if (id == id1) return true;
		return false;
	}
	else return true;
	*/
}

float3 calculateLighting(const  world & wrld, const Vec & hit, const int & id, const float3 & bari) {
	triangle tri = wrld.objects[id];
	float3 color = tri.c;
	float3 diffuse, specular;
	//float3 _bari;
	unsigned int lC = wrld.lightsCount;
	for (unsigned int i = 0; i < lC; ++i)
	{//проверим освещенность
		bool isVisible = Visible (wrld, hit, wrld.lights[i], id);
		if (isVisible)
		{
			float3 light_dir = (wrld.lights[i] - hit).normalization();
			double distancei = wrld.lights[i].distance (hit);
			//double cos = abs ((light.dot (tr.normal ().normalization ())) / (distancei));
			//float3 face_normal = wrld.objects[id]._normal;
			float3 face_normal = smoothNormal(wrld.objects[id].v_n, bari);
			//color = color + color*(1 / (distancei*distancei));
			//color = color + 0.2*color*max(0, face_normal.dot(light_dir));
			diffuse = (diffuse + color * max(0, face_normal.dot(light_dir))) *  wrld.objects[id].diffuse;


			
		}
		else
		{
			//color = color*0.2;
		}
	}
	return diffuse + specular;
}

/*
Recursively trace the input ray with a light source and reflection
*/
Vec RayTrace (const  world  & wrld,const Ray & ray,unsigned int deep) {
	float3 color (0, 0, 0);
	int id = 0;
	float3 hit, bari;// найдем полигон
	double distance ;
	
	//bool isIntersection = intersectHelper (wrld.objects, wrld.objCount, ray, distance, id, hit);
	bool isIntersection = findIntersection (wrld.objects, wrld.objCount, ray, distance, id, hit, bari);
	
	if (!isIntersection)
		return color;
	triangle tr = wrld.objects[id];
	//if (!tr.PointInTriangle(hit, tr.p[0], tr.p[1], tr.p[2]))
	//	assert(false);

	//float3 hitNormal = (1 - bari.v[0] - bari.v[1]) * tr.v_n[0] + bari.v[0] * tr.v_n[1] + bari.v[1] * tr.v_n[2]; // smoothed normal
	//float3 hitNormal = smoothNormal(tr.v_n, bari);
	//float facing = max(0, hitNormal.norm().dot((float3()-ray.d).norm()));
	//color = float3(facing, facing, facing);
	color = tr.c * 0.2;	// ambient
	if (color.v[X] < 0.01 && color.v[Y] < 0.01 && color.v[Z] < 0.02)
		int aaa = 1;
	color += calculateLighting(wrld, hit, id, bari);
	

	if (tr.reflect > 0 && deep > 0)//найдем отражение
	{
		Ray reflRay = reflect (ray, tr, hit, bari);
		color = color*(1.0 - tr.reflect) + RayTrace (wrld, reflRay, deep-1)*tr.reflect;
	}
	return color;
}

void SimpleRender (const  world & wrld, const camera & cam, Vec c[], const imgSettings & img) {
	Vec r; 
	int i = 0;
	#pragma omp parallel for schedule(dynamic, 2) private(r, i)       
	for (int y = img.h - 1; y >= 0; --y) { // Loop over image rows 
		fprintf (stderr, "\rRemain (%d spp) %d lines ", 1, y);
		for (int x = img.w - 1, i = y * img.w + x; x >= 0; --x, --i) {  // Loop cols 
			//i = y * img.w + x;	// use this with OpenMP
			
			#ifndef sampling
				double sx = 0.5, sy = 0.5;
				double k = 1;				
			#else
				// Семплирование для антиалиасинга
				double k = .25;
				for (unsigned int sy = 0; sy < 2; ++sy)     // 2x2 subpixel rows 
					for (unsigned int sx = 0; sx < 2; ++sx) // 2x2 subpixel cols	
			#endif
				{
					r = Vec ();
					//double dx = 0, dy = 0;
					Vec d = cam.cameraXangle*(((sx + .5) / 2 + x) / img.w - .5) +
						cam.cameraYangle*(((sy + .5) / 2 + y) / img.h - .5) +
						cam.cameraLocation.d;

					r += RayTrace (wrld, Ray (cam.cameraLocation.o , d.normalization ()), REFLECTION_DEPTH);//

					c[i] += Vec (clamp (r.x), clamp (r.y), clamp (r.z))*k;

				}
			//--i;
		}
	}
}