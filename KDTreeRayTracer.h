#include "BasicRayTracer.h"
#include "kdtree.h"
// “рассировка с применением  -мерного дерева

// ќсновной метод рендеринга сцены (трассировка всех лучей и получение цветов всех пикселей)
void SimpleRender (const  KDNode &root, const  world & wrld, const camera & cam, Vec c[], const imgSettings & img);

// “рассировка конкретного луча по сцене (поиск пересечений, проверка освещЄнности, отражени€ луча)
Vec RayTrace (const  KDNode &root, const  world  & wrld, const Ray & ray,unsigned int deep);

// ѕроверка видимости источника света из точки пересечени€
inline   bool Visible (const KDNode & root, const  world & wrld, const float3 & hit, const Vec & light, const triangle & tri);



inline   bool Visible (const KDNode & root, const  world & wrld, const float3 & hit, const Vec & light, const triangle & tri)
{
	double distToLight = hit.distance (light);
	float3 hit1;
	Vec sub = light - hit;
	Ray r (hit, sub);
	double distanse;
	int id1 = -1;
	triangle *tr = traceKDTree(root, r, hit1);
	if (tr != nullptr)
	{
		bool eq = true;
		for (int i = 0; i < 3; ++i)
			if (tr->p[i] == tri.p[i]) {
				eq = false;
				break;
			}
		if (eq) return true;
		return false;
	}
	else return true;
}



Vec RayTrace (const  KDNode &root, const  world  & wrld, const Ray & ray,unsigned int deep) {
	Vec color (0, 0, 0);
	int id = 0;
	float3 hit;// найдем полигон
	double distanse ;

	triangle *tr = traceKDTree(root, ray, hit);
	
	if (tr == nullptr)
		return color;

	color = tr->c;

	unsigned int lC = wrld.lightsCount;
	for (unsigned int i = 0; i < lC; ++i)
	{//проверим освещенность
		bool isVisible = Visible(root, wrld, hit, wrld.lights[i], *tr);
		if (isVisible)
		{
			float3 light = wrld.lights[i] - hit;
			double distancei = wrld.lights[i].distance (hit);
			double cos = abs ((light.dot(tr->normal().normalization())) / (distancei));
			color = color + color*(1 / (distancei*distancei));
		}
		else
		{
			color = color*0.2;
		}
	}

	//	

	if (tr->reflect > 0 && deep > 0)//найдем отражение
	{
		Ray reflRay = reflect (ray, *tr, hit);
		color = color*(1.0 - tr->reflect) + RayTrace(root, wrld, reflRay, deep--)*tr->reflect;
	}
	return color;
}

void SimpleRender (const  KDNode &root, const  world & wrld, const camera & cam, Vec c[], const imgSettings & img) {
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

					r += RayTrace(root, wrld, Ray (cam.cameraLocation.o + d * 140, d.normalization ()), REFLECTION_DEPTH);

					c[i] += Vec (clamp (r.x), clamp (r.y), clamp (r.z))*k;
				}
			//--i;
		}
	}
}