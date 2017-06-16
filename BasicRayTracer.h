#include <stdio.h>
#include <math.h>   
#include <stdlib.h> 
#include <omp.h>
#include "Vec.hpp"
#include "Ray.hpp"
#include "Triangle.hpp"
#include "BMP.hpp"
#include "raytracer_misc.h"
#include "World.hpp"
#define INF 1e20

#pragma once

// Основной метод рендеринга сцены (трассировка всех лучей и получение цветов всех пикселей)
void SimpleRender (const  world & wrld, const camera & cam, Vec c[], const imgSettings & img);

// Трассировка конкретного луча по сцене (поиск пересечений, освещённость, отражения)
Vec RayTrace (const  world  & wrld,const Ray & ray,unsigned int deep);

// Проверка видимости источника света из точки пересечения
inline   bool Visible (const  world & wrld, const Vec & hit, const Vec & light, const int & id);

/*
Return new reflected ray
Получение отражённого луча
*/
inline   Ray reflect (const Ray & r,const  triangle & obj,const Vec & hit) {
	Vec normal = obj.normal ();
	Vec iV = hit - Vec(r.o);//inputVector
	normal = normal.normalization ();
	Vec rV = iV - ((normal*(iV.dot (normal))) * 2);//reflectVector
	Ray reflect (hit, rV.normalization ());//new reflect ray
	return reflect;
}

// Поиск пересечений луча с треугольниками сцены
inline   void intersect (const triangle * objects, const unsigned int objCount,
	const Ray &r, double &t, int &id, float3 & hit, bool * isIntersect)
{
	float3 localHit;
	double  d; 
	t = INF;
	for (unsigned int i = objCount; i--;)
	{
		//bool isSpherIntersection = objects[i].intersectSpher (r);
		//if (isSpherIntersection)
		{
		#ifndef MOLLER_TRUMBORE_INTERSECT
			bool isIntersection = objects[i].intersect (r, localHit); 
		#else
			bool isIntersection = objects[i].mollerTrumboreIntersect(r, localHit); 
		#endif
			
			if (isIntersection)
			{
				d = localHit.distance (r.o); 
				if (d < t)
				{
					t = d;
					id = i;
					hit = localHit;
				}
			}
		}
	}
	*isIntersect =  t < INF;
}

inline   bool intersectHelper (const triangle * objects, const unsigned int objCount,
	const Ray &r, double &t, int &id, float3 & hit)
{
	bool  isIntersect = 0;
	intersect (objects, objCount, r, t, id, hit, &isIntersect);
	return isIntersect;
}


/*
Returns true if the light source is visible
*/
// For standard raytracing
inline   bool Visible (const  world & wrld, const Vec & hit, const Vec & light, const int & id)
{
	double distToLight = hit.distance (light);
	float3 hit1;
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

/*
Recursively trace the input ray with a light source and reflection
*/
Vec RayTrace (const  world  & wrld,const Ray & ray,unsigned int deep) {
	Vec color (0, 0, 0);
	int id = 0;
	float3 hit;// найдем полигон
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
			float3 light = wrld.lights[i] - hit;
			double distancei = wrld.lights[i].distance (hit);
			double cos = abs ((light.dot (tr.normal ().normalization ())) / (distancei));
			color = color + color*(1 / (distancei*distancei));
		}
		else
		{
			color = color*0.2;
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

					r += RayTrace (wrld, Ray (cam.cameraLocation.o + d * 140, d.normalization ()), REFLECTION_DEPTH);//

					c[i] += Vec (clamp (r.x), clamp (r.y), clamp (r.z))*k;
				}
			//--i;
		}
	}
}