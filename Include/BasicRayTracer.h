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


/*
Returns true if the light source is visible
*/
// For standard raytracing
inline   bool Visible (const  world & wrld, const Vec & hit, const Vec & light, const int & id, float3 & bari)
{
	double distToLight = hit.distance (light);
	float3 hit1;
	Vec sub = light - hit;
	//Vec sub = hit - light;
	sub.normalization();
	Ray r (hit + sub * 0.001, sub);
	//Ray r (hit, sub);
	double distance;
	//float3 bari;
	int id1 = -1;
	bool isIntersection = findIntersection (wrld.objects, wrld.objCount, r, distance, id1, hit1, bari);
	if (!isIntersection)
		return true;
	else {
		if (id == id1) return true;
		if (distToLight + EPSILON < distance) return true;
		return false;
	}

}

float3 calculateLighting(const  world & wrld, const Ray & ray, const Vec & hit, const int & id, const float3 & bari) {
	triangle tri = wrld.objects[id];
	float3 color = tri.c;
	float3 diffuse, specular;
	float3 _bari;
	unsigned int lC = wrld.lightsCount;
	for (unsigned int i = 0; i < lC; ++i)
	{//проверим освещенность
		bool isVisible = Visible (wrld, hit, wrld.lights[i], id, _bari);
		if (isVisible)
		{
			float3 light_dir = (wrld.lights[i] - hit).normalization();
			double distancei = wrld.lights[i].distance (hit);
			
			//float3 face_normal = wrld.objects[id]._normal;
			float3 face_normal = smoothNormal(wrld.objects[id].v_n, bari);
			
			//color = color + color*(1 / (distancei*distancei));
			//color = color + 0.2*color*max(0, face_normal.dot(light_dir));
			diffuse = diffuse + color * max(0, face_normal.dot(light_dir)) *  wrld.objects[id].diffuse;

			float3 view_d = ray.d.norm(), 
				refl_d = reflect(ray, wrld.objects[id], hit, _bari).d.norm();
			float prod = max(0, view_d.dot(refl_d)); 
			float spec = pow(prod, wrld.objects[id].specular) * wrld.objects[id].specular;
			specular += float3(spec, spec, spec);
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
	
	bool isIntersection = findIntersection (wrld.objects, wrld.objCount, ray, distance, id, hit, bari);
	
	if (!isIntersection)
		return color;
	triangle tr = wrld.objects[id];

	color = tr.c * 0.2;	// ambient

	color += calculateLighting(wrld, ray, hit, id, bari);	// diffuse and specular

	if (tr.reflect > 0 && deep > 0)//найдем отражение
	{
		Ray reflRay = reflect (ray, tr, hit, bari);
		color = color*(1.0 - tr.reflect) + RayTrace (wrld, reflRay, deep-1)*tr.reflect;
	}
	//color += calculateLighting(wrld, ray hit, id, bari);
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