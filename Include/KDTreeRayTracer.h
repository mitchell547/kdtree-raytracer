#include "World.hpp"
#include "Geometry\Vec.hpp"
#include "Geometry\float3.h"
#include "Geometry\Triangle.hpp"
#include "Geometry\Ray.hpp"
#include "BasicRayTracer.h"
#include "kdtree.h"
#include "assert.h"
// Ray tracing with K-d tree
// Трассировка с применением К-мерного дерева

// Main rendering method
// Основной метод рендеринга сцены (трассировка всех лучей и получение цветов всех пикселей)
void SimpleRender (const KDScene & scene, const camera & cam, Vec c[], const imgSettings & img);

// Single ray tracing
// Трассировка конкретного луча по сцене (поиск пересечений, проверка освещённости, отражения луча)
Vec RayTrace (const KDScene & scene, const Ray & ray, unsigned int deep);

// Check light source visibility
// Проверка видимости источника света из точки пересечения
inline   bool Visible (const KDScene & scene, const float3 & hit, const float3 & light, const int & tri, float3 & bari);



inline   bool Visible (const KDScene & scene, const float3 & hit, const float3 & light, const int & tri_id, float3 & bari)
{
	double distToLight = hit.distance (light);
	float3 hit1;
	Vec sub = light - hit;
	sub.normalization();
	Ray r (hit + sub * 0.001, sub);

	double distance;
	int id1 = -1;
	int edgeHit;
	int id = traceKDScene(scene, r, hit1, bari, edgeHit);
	triangle *tr = &scene.triangles[id];
	if (id < 0)
		return true;
	else {
		if (distToLight + EPSILON < r.o.distance(hit1))
			return true;
		if (id == tri_id) return true;
		return false;
	}

}

float3 calculateLighting(const KDScene & scene, const Ray & ray, const Vec & hit, const int & id, const float3 & bari) {
	triangle tri = scene.triangles[id];
	float3 color = tri.c;
	float3 diffuse, specular;
	float3 _bari;
	unsigned int lC = scene.lights_cnt;
	for (unsigned int i = 0; i < lC; ++i)
	{//проверим освещенность
		bool isVisible = Visible(scene, hit, scene.lights[i], id, _bari);
		if (isVisible)
		{
			//double distancei = scene.lights[i].distance (hit);
			//color = color + color*(1 / (distancei*distancei));

			float3 light_dir = (scene.lights[i] - hit).normalization();
			float3 face_normal = smoothNormal(tri.v_n, bari);
			diffuse = diffuse + color * max(0, face_normal.dot(light_dir)) * tri.diffuse;

			float3 view_d = ray.d.norm(), 
				refl_d = reflect(ray, tri, hit, _bari).d.norm();
			float prod = max(0, view_d.dot(refl_d)); 
			float spec = pow(prod, tri.specular) * tri.specular;
			specular += float3(spec, spec, spec);
		}

	}
	return diffuse + specular;
	//return color;
}

Vec RayTrace (const KDScene & scene, const Ray & ray,unsigned int deep) {
	Vec color (0, 0, 0);
	//int id = 0;
	float3 hit, bari;// найдем полигон
	int edgeHit=0;
	
	int tri_id = traceKDScene(scene, ray, hit, bari, edgeHit);

	#ifdef TREE_VISUALISATION
	if (edgeHit)
		//return Vec(0.8, 0.8, 0.8);
		color = color + Vec(0.05, 0.05, 0.05) * edgeHit;
	#endif
	//color = Vec(1,1,1);
	if (tri_id < 0)
		return color;
	
	triangle tri = scene.triangles[tri_id];
	color += tri.c * 0.2;

	color += calculateLighting(scene, ray, hit, tri_id, bari);
	
	if (tri.reflect > 0 && deep > 0)//найдем отражение
	{
		Ray reflRay = reflect (ray, tri, hit, bari);
		color = color*(1.0 - tri.reflect) + RayTrace(scene, reflRay, deep-1)*tri.reflect;
	}

	if (tri.refraction > 0 && deep > 0) {
		Ray refrRay = refract(ray, tri, hit, bari);
		if (refrRay.d.v[0] != 0 && refrRay.d.v[1] != 0 && refrRay.d.v[2] != 0)
			color = color*(1.0 - tri.refraction) + RayTrace(scene, refrRay, deep-1)*tri.refraction;
	}
	
	return color;
}

void SimpleRender (const KDScene & scene, const camera & cam, Vec c[], const imgSettings & img) {
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
					//if (x == 170 && y == 240-110)
					r += RayTrace(scene, Ray (cam.cameraLocation.o , d.normalization ()), REFLECTION_DEPTH);

					c[i] += Vec (clamp (r.x), clamp (r.y), clamp (r.z))*k;
				}
			//--i;
		}
	}
}