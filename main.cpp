
#include <stdio.h>
#include <math.h>   
#include <stdlib.h> 
#include <random>
#include <iostream>
#include <omp.h>
#include "Include/Geometry/Vec.hpp"
#include "Include/Geometry/Ray.hpp"
#include "Include/Geometry/Triangle.hpp"
#include "Include/Misc/BMP.hpp"
#include "Include/Misc/Ply.hpp"
#include "Include/raytracer_misc.h"
#include "Include/World.hpp"
#include "Include/scene_samples.h"
#define INF 1e20


// Render settings:
// "Настройки" рендера:

//#define sampling	// antialiasing
#define MOLLER_TRUMBORE_INTERSECT
#define REFLECTION_DEPTH 3
#define TREE_DEPTH 16

// Choose model
// (*) Выбор модели (одна из)
//#define rabbit_model
#define cube_model
//#define triangles_model

// Using k-d tree tracing
// (*) Включение трассировки k-d дерева
#define kdtrace

//#define TREE_VISUALISATION	// для отладки

#include "Include/kdtree.h"
#include "Include/BasicRayTracer.h"
#include "Include/KDTreeRayTracer.h"


void print_render_info(int w, int h) {
	printf("\tRender info:\n");
	printf("* Image size: %dx%d\n", w, h);

#ifdef sampling
	printf("* Sampling is ON\n");
#else
	printf("* Sampling is OFF\n");
#endif

#ifdef kdtrace
	printf("* K-d tree is used (max depth: %d)\n", TREE_DEPTH);
#else
	printf("* K-d tree is NOT used\n");
#endif
	printf("* Depth of reflections: %d\n\n", REFLECTION_DEPTH);
}

void print_model_info(world & w) {
	printf("\tModel info:\n");
	printf("* Triangles count: %d\n", w.objCount);
	printf("* Lights count: %d\n\n", w.lightsCount);
}

int main (int argc, char *argv[])
{
	// Image settings
	// Параметры изображения
	//int w = 1024, h = 768;
	//int w = 640, h = 480;
	//int w = 512, h = 512;
	int w = 320, h = 240;
	//int w = 256, h = 256;
	int samps = argc == 2 ? atoi (argv[1]) / 4 : 1; // # samples 

	// Output image
	// Выходное изображение
	Vec  r, *c = new Vec[w*h];
	imgSettings img = imgSettings (w, h, samps);

	print_render_info(w, h);

	// 3D Scenes:
	// Сцены:

	// // Модель кролика 
#ifdef rabbit_model
	/*Model_PLY rabbit;
	rabbit.Load ("bun_zipper_res4.ply");
	//camera cam (Ray (Vec (85, 55, 170), Vec (-0.45, -0.04, -1).normalization ()), Vec (w*.5135 / h));
	camera cam (Ray (Vec (0, 70, 220), Vec (0, -0.25, -1).normalization ()), Vec (w*.5135 / h,0,0));
	//camera cam (Ray (Vec (0, 60, 200), Vec (0, -0.25, -1).normalization ()), Vec (w*.5135 / h,0,0));
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = 2;
	world wrld = world (objCount, lightsCount, obj, lights);
	plyToMass (rabbit, wrld);
	*/
	world wrld = loadRabbitModel();
	//camera cam (Ray (Vec (85, 55, 170), Vec (-0.45, -0.04, -1).normalization ()), Vec (w*.5135 / h));
	camera cam (Ray (Vec (0, 70, 220), Vec (0, -0.25, -1).normalization ()), Vec (w*.5135 / h,0,0));
	//camera cam (Ray (Vec (0, 60, 200), Vec (0, -0.25, -1).normalization ()), Vec (w*.5135 / h,0,0));
	print_model_info(wrld);

	//KDNode scene;
	double build_s = omp_get_wtime();
	KDScene * scene = buildKDScene(wrld.objects, wrld.objCount, wrld.lights, wrld.lightsCount, TREE_DEPTH);
	double build_f = omp_get_wtime();
	fprintf (stderr, "\r\nK-d tree build time %5.3f\n", build_f-build_s);
#endif

	// // Модель куба
#ifdef cube_model
	/*int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = sizeof(lights) / sizeof(Vec);
	//camera cam (Ray (Vec (140, 45, 170), Vec (-0.7, -0.15, -1).normalization ()), Vec (w*.5135 / h));
	camera cam (Ray (Vec (112, 40, 120), Vec (-0.8, -0.2, -1).normalization ()), Vec (w*.5135 / h));
	world wrld = world (objCount-100, lightsCount, obj+100, lights);
	//world wrld = world (12, 2, cube, lights);	// one cube
	*/
	//camera cam (Ray (Vec (140, 45, 170), Vec (-0.7, -0.15, -1).normalization ()), Vec (w*.5135 / h));
	camera cam (Ray (Vec (112, 40, 120), Vec (-0.8, -0.2, -1).normalization ()), Vec (w*.5135 / h));
	world wrld = loadCubeModel();
	
	KDScene * scene = buildKDScene(wrld.objects, wrld.objCount, wrld.lights, wrld.lightsCount, TREE_DEPTH);
#endif

	// Two triangles
	// // Два треугольника перед камерой
#ifdef triangles_model
	camera cam (Ray (Vec (105, 44, 190), Vec (0, 1, -0.2).normalization ()), Vec (w*.5135 / h));
	Vec light[] = {Vec(85, 45, 170)};
	triangle tri[] = {
		triangle (Vec (85, 180, 120), Vec (105, 180, 150), Vec (65, 180, 190), Vec (.70, .10, .10), 0.0),
		triangle (Vec (85, 180, 120), Vec (105, 180, 150), Vec (65, 180, 190), Vec (.10, .10, .70), 0.0).moveX(20)};
	world wrld = world (2, 1, tri, light);
	KDScene * scene = buildKDScene(wrld.objects, wrld.objCount, wrld.lights, wrld.lightsCount, TREE_DEPTH);
#endif

	// Rendering
	// Рендеринг
	double start = omp_get_wtime ();
	#ifndef kdtrace
		SimpleRender(wrld, cam, c, img);	// Basic
	#else
		SimpleRender(*scene, cam, c, img);	// K-d tree
	#endif
	double end = omp_get_wtime ();

	fprintf (stderr, "\nRender finished in %5.3fs", end-start);

	// Output
	// Вывод
	writeToBmp (c, w, h);
	getchar();
}