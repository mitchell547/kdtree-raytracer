// triangle.cpp: определяет точку входа для консольного приложения.
//
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <stdio.h>
#include <math.h>   
#include <stdlib.h> 
#include <random>
#include <iostream>
#include <omp.h>
#include "Vec.hpp"
#include "Ray.hpp"
#include "Triangle.hpp"
#include "BMP.hpp"
#include "raytracer_misc.h"
#include "Ply.hpp"
#include "World.hpp"
#define EPSILON 0.000000001
#define INF 1e20
//#define standartTest //  or...
//#define baricentTest
//#define bmp
#define sampleInCenter
//#define filter

#define kdtrace

#include "kdtree.h"

/*struct camera {
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
*/
/*
struct imgSettings {
	unsigned int w;//width
	unsigned int h;//hight
	unsigned int samps;//sampels per pixel
	imgSettings (unsigned int w_, unsigned int h_, unsigned int samps_) :w (w_), h (h_), samps (samps_){}
};
*/
/*
struct world {
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

	void setObj (const triangle*objects_, const unsigned int objCount_)
	{
		objCount = objCount_;
		objects = (triangle *)malloc (objCount_*sizeof(triangle));
		for (unsigned int i = 0; i < objCount; ++i)
			objects[i] = objects_[i];
	}
};
*/

 //world CUDA_WORLD();
//triangle * CUDA_objects;
//enum Refl_t { DIFF, SPEC, REFR };  // material types, used in distanceiance() 


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

/*Vec::Vec(float3 & b) {
	x = b.v[0];
	y = b.v[1];
	z = b.v[2];
}

void Vec::operator=(const float3& b) {
	x = b.v[0];
	y = b.v[1];
	z = b.v[2];
}*/

bool helpOrient (triangle t, Vec v1, Vec v2) {
	Vec normalization = t.normal ();
	double x1 = normalization.dot (v1 - t.p[0]);
	double x2 = normalization.dot (v2 - t.p[0]);
	return x1*x2 + EPSILON >= 0;
}
/*
Returns true if input ray intersect some object
*/


inline   void intersect (const triangle * objects, const unsigned int objCount,
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
			bool isIntersection = objects[i].intersect (r, float3(lockalHit)); // ! mixing Vec and float3 
			if (isIntersection)
			{
				d = lockalHit.distance (Vec(r.o)); // ! mixing Vec and float3 
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

inline   bool intersectHelper (const triangle * objects, const unsigned int objCount,
	const Ray &r, double &t, int &id, Vec & hit)
{
	bool  isIntersect = 0;
	intersect (objects, objCount, r, t, id, hit, &isIntersect);
	return isIntersect;
}
/*
Returns true if the light source is visible
*/
// For usual raytracing
inline   bool Visible (const  world & wrld, const Vec & hit, const Vec & light, const int & id)
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

// For K-d tree raytracing
inline   bool Visible (const KDNode & root, const  world & wrld, const float3 & hit, const Vec & light, const triangle & tri)
{
	double distToLight = hit.distance (light);
	float3 hit1;
	Vec sub = light - hit;
	Ray r (hit, sub);
	double distanse;
	int id1 = -1;
	//bool isIntersection = intersectHelper (wrld.objects, wrld.objCount, r, distanse, id1, hit1);
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


inline   Vec Shade (const Vec & hit, const Vec & light) {
	//double ka = 0.1; //ambient coefficient
	return Vec (0.3, 0.3, 0.3);
}
/*
Return new reflected ray
*/
inline   Ray reflect (const Ray & r,const  triangle & obj,const Vec & hit) {
	Vec normal = obj.normal ();
	Vec iV = hit - Vec(r.o);//inputVector
	normal = normal.normalization ();
	Vec rV = iV - ((normal*(iV.dot (normal))) * 2);//reflectVector
	Ray reflect (hit, rV.normalization ());//new reflect ray
	return reflect;
}
/*
Recursively trace the input ray with a light source and reflection
*/
Vec RayTrace (const  world  & wrld,const Ray & ray,unsigned int deep) {
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

Vec KDTreeRayTrace (const  KDNode &root, const  world  & wrld, const Ray & ray,unsigned int deep) {
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
		bool isVisible = Visible (root, wrld, hit, wrld.lights[i], *tr);
		if (isVisible)
		{
			Vec light = wrld.lights[i] - hit;
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
		color = color*(1.0 - tr->reflect) + RayTrace (wrld, reflRay, deep--)*tr->reflect;
	}
	return color;
}

Ray * RenderRayHelper (const camera & cam, const imgSettings & img) {
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

void renderNEW (const Ray * allRays, const  world & wrld, const imgSettings & img, Vec *c) {
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
	Vec r; 
	int i = 0;
	#pragma omp parallel for schedule(dynamic, 2) private(r)       
	for (/*unsigned*/ int y = 0; y < img.h; ++y) // Loop over image rows 
	{
		fprintf (stderr, "\rRendering (%d spp) %5.2f%%", img.samps * 4, 100.*y / (img.h - 1));
		for (unsigned int x = 0; x < img.w; ++x)   // Loop cols 
		{
			int i = y*img.w + x;	// use this with OpenMP
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
				//for (unsigned int sa = 0; sa < img.samps; ++sa)// for distributed ray tracer
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

#ifndef kdtrace
void SimpleRender (const  world & wrld, const camera & cam, Vec c[], const imgSettings & img) {
#else
void SimpleRender (const  KDNode &root, const  world & wrld, const camera & cam, Vec c[], const imgSettings & img) {
#endif
	Vec r; 
	int i = 0;
	#pragma omp parallel for schedule(dynamic, 2) private(r, i)       
	for (int y = img.h - 1; y >= 0; --y) { // Loop over image rows 
		fprintf (stderr, "\rRemain (%d spp) %d lines ", 1, y);
		for (int x = img.w - 1, i = y * img.w + x; x >= 0; --x, --i) {  // Loop cols 
			//i = y * img.w + x;	// use this with OpenMP
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
					//double dx = 0, dy = 0;
					Vec d = cam.cameraXangle*(((sx + .5) / 2 + x) / img.w - .5) +
						cam.cameraYangle*(((sy + .5) / 2 + y) / img.h - .5) +
						cam.cameraLocation.d;

				#ifndef kdtrace
					r += RayTrace (wrld, Ray (cam.cameraLocation.o + d * 140, d.normalization ()), 1);//
				#else
					r += KDTreeRayTrace (root, wrld, Ray (cam.cameraLocation.o + d * 140, d.normalization ()), 1);
				#endif

					c[i] += Vec (clamp (r.x), clamp (r.y), clamp (r.z))*k;
				}
			//--i;
		}
	}
}

int main (int argc, char *argv[])
{
	//int w = 1024, h = 768;
	int w = 320, h = 240;
	int samps = argc == 2 ? atoi (argv[1]) / 4 : 1; // # samples 
	
	Vec  r, *c = new Vec[w*h];
	imgSettings img = imgSettings (w, h, samps);

	// // Rabbit
	/*Model_PLY rabbit;
	rabbit.Load ("bun_zipper_res4.ply");
	//camera cam (Ray (Vec (85, 55, 170), Vec (-0.45, -0.04, -1).normalization ()), Vec (w*.5135 / h));
	camera cam (Ray (Vec (0, 70, 220), Vec (0, -0.25, -1).normalization ()), Vec (w*.5135 / h,0,0));
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = 1;
	world wrld = world (objCount, lightsCount, obj, lights);
	plyToMass (rabbit, wrld);
	
	KDNode scene;
	double build_s = omp_get_wtime();
	buildKDTree(scene, wrld.objects, wrld.objCount,8);
	double build_f = omp_get_wtime();
	fprintf (stderr, "\rtime %5.3f\n", build_f-build_s);
	*/

	// // Cube
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = sizeof(lights) / sizeof(Vec);
	camera cam (Ray (Vec (140, 45, 170), Vec (-0.7, -0.15, -1).normalization ()), Vec (w*.5135 / h));
	world wrld = world (objCount, lightsCount, obj, lights);
	
	KDNode scene;
	buildKDTree(scene, obj, objCount, 4);
	
	// // One triangle
	/*
	camera cam (Ray (Vec (105, 44, 190), Vec (0, 1, -0.2).normalization ()), Vec (w*.5135 / h));
	Vec light[] = {Vec(85, 45, 170)};
	triangle tri[] = {
		triangle (Vec (85, 180, 120), Vec (105, 180, 150), Vec (65, 180, 190), Vec (.70, .10, .10), 0.1)};
	world wrld = world (1, 1, tri, light);
	*/

	//выделяем память для device копий для OBJECTS
	//int size = objCount*sizeof(triangle);
	//Ray * allRays = RenderRayHelper (cam, img);

	double start = omp_get_wtime ();
	//renderNEW (allRays, wrld, img, c);
#ifndef kdtrace
	SimpleRender(wrld, cam, c, img);
#else
	SimpleRender(scene, wrld, cam, c, img);
#endif
	double end = omp_get_wtime ();

	fprintf (stderr, "\nElapsed time %5.3f", end-start);

	writeToBmp (c, w, h);
	getchar();
}