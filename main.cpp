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

//#define kdtrace

#include "kdtree.h"



float3 lights[] = { float3 (7, 70, 25),
				float3 (60, 7, 7) };
triangle obj[] = {
	triangle (float3 (37, 25, -4), float3 (39, 25, -4), float3 (39, 27, -4), float3 (.0, .0, .50), 0),//back

	triangle (float3 (5, 5, 5), float3 (6, 5, 5), float3 (6, 6, 5), float3 (0.6, 0.6, 0.6), 0.91),
	triangle (float3 (6, 5, 5), float3 (7, 5, 5), float3 (7, 6, 5), float3 (0.6, 0.6, 0.6), 0.92),
	triangle (float3 (7, 5, 5), float3 (8, 5, 5), float3 (8, 6, 5), float3 (0.6, 0.6, 0.6), 0.93),
	triangle (float3 (8, 5, 5), float3 (9, 5, 5), float3 (9, 6, 5), float3 (0.6, 0.6, 0.6), 0.94),
	triangle (float3 (9, 5, 5), float3 (10, 5, 5), float3 (10, 6, 5), float3 (0.6, 0.6, 0.6), 0.95),
	triangle (float3 (10, 5, 5), float3 (11, 5, 5), float3 (11, 6, 5), float3 (0.6, 0.6, 0.6), 0.96),
	triangle (float3 (11, 5, 5), float3 (12, 5, 5), float3 (12, 6, 5), float3 (0.6, 0.6, 0.6), 0.97),
	triangle (float3 (12, 5, 5), float3 (13, 5, 5), float3 (13, 6, 5), float3 (0.6, 0.6, 0.6), 0.98),
	triangle (float3 (13, 5, 5), float3 (14, 5, 5), float3 (14, 6, 5), float3 (0.6, 0.6, 0.6), 0.99),
	triangle (float3 (14, 5, 5), float3 (15, 5, 5), float3 (15, 6, 5), float3 (0.6, 0.6, 0.6), 0.90),

	triangle (float3 (6, 5, 5), float3 (6, 6, 5), float3 (7, 6, 5), float3 (0.6, 0.6, 0.6), 0.89),
	triangle (float3 (7, 5, 5), float3 (7, 6, 5), float3 (8, 6, 5), float3 (0.6, 0.6, 0.6), 0.88),
	triangle (float3 (8, 5, 5), float3 (8, 6, 5), float3 (9, 6, 5), float3 (0.6, 0.6, 0.6), 0.87),
	triangle (float3 (9, 5, 5), float3 (9, 6, 5), float3 (10, 6, 5), float3 (0.6, 0.6, 0.6), 0.86),
	triangle (float3 (10, 5, 5), float3 (10, 6, 5), float3 (11, 6, 5), float3 (0.6, 0.6, 0.6), 0.85),
	triangle (float3 (11, 5, 5), float3 (11, 6, 5), float3 (12, 6, 5), float3 (0.6, 0.6, 0.6), 0.84),
	triangle (float3 (12, 5, 5), float3 (12, 6, 5), float3 (13, 6, 5), float3 (0.6, 0.6, 0.6), 0.83),
	triangle (float3 (13, 5, 5), float3 (13, 6, 5), float3 (14, 6, 5), float3 (0.6, 0.6, 0.6), 0.82),
	triangle (float3 (14, 5, 5), float3 (14, 6, 5), float3 (15, 6, 5), float3 (0.6, 0.6, 0.6), 0.81),

	triangle (float3 (6, 6, 5), float3 (7, 6, 5), float3 (7, 7, 5), float3 (0.6, 0.6, 0.6), 0.89),
	triangle (float3 (7, 6, 5), float3 (8, 6, 5), float3 (8, 7, 5), float3 (0.6, 0.6, 0.6), 0.88),
	triangle (float3 (8, 6, 5), float3 (9, 6, 5), float3 (9, 7, 5), float3 (0.6, 0.6, 0.6), 0.87),
	triangle (float3 (9, 6, 5), float3 (10, 6, 5), float3 (10, 7, 5), float3 (0.6, 0.6, 0.6), 0.86),
	triangle (float3 (10, 6, 5), float3 (11, 6, 5), float3 (11, 7, 5), float3 (0.6, 0.6, 0.6), 0.85),
	triangle (float3 (11, 6, 5), float3 (12, 6, 5), float3 (12, 7, 5), float3 (0.6, 0.6, 0.6), 0.84),
	triangle (float3 (12, 6, 5), float3 (13, 6, 5), float3 (13, 7, 5), float3 (0.6, 0.6, 0.6), 0.83),
	triangle (float3 (13, 6, 5), float3 (14, 6, 5), float3 (14, 7, 5), float3 (0.6, 0.6, 0.6), 0.82),
	triangle (float3 (14, 6, 5), float3 (15, 6, 5), float3 (15, 7, 5), float3 (0.6, 0.6, 0.6), 0.81),

	triangle (float3 (7, 6, 5), float3 (7, 7, 5), float3 (8, 7, 5), float3 (0.6, 0.6, 0.6), 0.79),
	triangle (float3 (8, 6, 5), float3 (8, 7, 5), float3 (9, 7, 5), float3 (0.6, 0.6, 0.6), 0.78),
	triangle (float3 (9, 6, 5), float3 (9, 7, 5), float3 (10, 7, 5), float3 (0.6, 0.6, 0.6), 0.76),
	triangle (float3 (10, 6, 5), float3 (10, 7, 5), float3 (11, 7, 5), float3 (0.6, 0.6, 0.6), 0.75),
	triangle (float3 (11, 6, 5), float3 (11, 7, 5), float3 (12, 7, 5), float3 (0.6, 0.6, 0.6), 0.74),
	triangle (float3 (12, 6, 5), float3 (12, 7, 5), float3 (13, 7, 5), float3 (0.6, 0.6, 0.6), 0.73),
	triangle (float3 (13, 6, 5), float3 (13, 7, 5), float3 (14, 7, 5), float3 (0.6, 0.6, 0.6), 0.72),
	triangle (float3 (14, 6, 5), float3 (14, 7, 5), float3 (15, 7, 5), float3 (0.6, 0.6, 0.6), 0.71),

	triangle (float3 (7, 7, 5), float3 (8, 7, 5), float3 (8, 8, 5), float3 (0.6, 0.6, 0.6), 0.79),
	triangle (float3 (8, 7, 5), float3 (9, 7, 5), float3 (9, 8, 5), float3 (0.6, 0.6, 0.6), 0.78),
	triangle (float3 (9, 7, 5), float3 (10, 7, 5), float3 (10, 8, 5), float3 (0.6, 0.6, 0.6), 0.76),
	triangle (float3 (10, 7, 5), float3 (11, 7, 5), float3 (11, 8, 5), float3 (0.6, 0.6, 0.6), 0.75),
	triangle (float3 (11, 7, 5), float3 (12, 7, 5), float3 (12, 8, 5), float3 (0.6, 0.6, 0.6), 0.74),
	triangle (float3 (12, 7, 5), float3 (13, 7, 5), float3 (13, 8, 5), float3 (0.6, 0.6, 0.6), 0.73),
	triangle (float3 (13, 7, 5), float3 (14, 7, 5), float3 (14, 8, 5), float3 (0.6, 0.6, 0.6), 0.72),
	triangle (float3 (14, 7, 5), float3 (15, 7, 5), float3 (15, 8, 5), float3 (0.6, 0.6, 0.6), 0.71),

	triangle (float3 (8, 7, 5), float3 (8, 8, 5), float3 (9, 8, 5), float3 (0.6, 0.6, 0.6), 0.69),
	triangle (float3 (9, 7, 5), float3 (9, 8, 5), float3 (10, 8, 5), float3 (0.6, 0.6, 0.6), 0.68),
	triangle (float3 (10, 7, 5), float3 (10, 8, 5), float3 (11, 8, 5), float3 (0.6, 0.6, 0.6), 0.67),
	triangle (float3 (11, 7, 5), float3 (11, 8, 5), float3 (12, 8, 5), float3 (0.6, 0.6, 0.6), 0.66),
	triangle (float3 (12, 7, 5), float3 (12, 8, 5), float3 (13, 8, 5), float3 (0.6, 0.6, 0.6), 0.65),
	triangle (float3 (13, 7, 5), float3 (13, 8, 5), float3 (14, 8, 5), float3 (0.6, 0.6, 0.6), 0.64),
	triangle (float3 (14, 7, 5), float3 (14, 8, 5), float3 (15, 8, 5), float3 (0.6, 0.6, 0.6), 0.63),

	triangle (float3 (8, 8, 5), float3 (9, 8, 5), float3 (9, 9, 5), float3 (0.6, 0.6, 0.6), 0.69),
	triangle (float3 (9, 8, 5), float3 (10, 8, 5), float3 (10, 9, 5), float3 (0.6, 0.6, 0.6), 0.68),
	triangle (float3 (10, 8, 5), float3 (11, 8, 5), float3 (11, 9, 5), float3 (0.6, 0.6, 0.6), 0.67),
	triangle (float3 (11, 8, 5), float3 (12, 8, 5), float3 (12, 9, 5), float3 (0.6, 0.6, 0.6), 0.66),
	triangle (float3 (12, 8, 5), float3 (13, 8, 5), float3 (13, 9, 5), float3 (0.6, 0.6, 0.6), 0.65),
	triangle (float3 (13, 8, 5), float3 (14, 8, 5), float3 (14, 9, 5), float3 (0.6, 0.6, 0.6), 0.64),
	triangle (float3 (14, 8, 5), float3 (15, 8, 5), float3 (15, 9, 5), float3 (0.6, 0.6, 0.6), 0.63),

	triangle (float3 (9, 8, 5), float3 (9, 9, 5), float3 (10, 9, 5), float3 (0.6, 0.6, 0.6), 0.58),
	triangle (float3 (10, 8, 5), float3 (10, 9, 5), float3 (11, 9, 5), float3 (0.6, 0.6, 0.6), 0.57),
	triangle (float3 (11, 8, 5), float3 (11, 9, 5), float3 (12, 9, 5), float3 (0.6, 0.6, 0.6), 0.56),
	triangle (float3 (12, 8, 5), float3 (12, 9, 5), float3 (13, 9, 5), float3 (0.6, 0.6, 0.6), 0.55),
	triangle (float3 (13, 8, 5), float3 (13, 9, 5), float3 (14, 9, 5), float3 (0.6, 0.6, 0.6), 0.54),
	triangle (float3 (14, 8, 5), float3 (14, 9, 5), float3 (15, 9, 5), float3 (0.6, 0.6, 0.6), 0.53),

	triangle (float3 (9, 9, 5), float3 (10, 9, 5), float3 (10, 10, 5), float3 (0.6, 0.6, 0.6), 0.58),
	triangle (float3 (10, 9, 5), float3 (11, 9, 5), float3 (11, 10, 5), float3 (0.6, 0.6, 0.6), 0.57),
	triangle (float3 (11, 9, 5), float3 (12, 9, 5), float3 (12, 10, 5), float3 (0.6, 0.6, 0.6), 0.56),
	triangle (float3 (12, 9, 5), float3 (13, 9, 5), float3 (13, 10, 5), float3 (0.6, 0.6, 0.6), 0.55),
	triangle (float3 (13, 9, 5), float3 (14, 9, 5), float3 (14, 10, 5), float3 (0.6, 0.6, 0.6), 0.54),
	triangle (float3 (14, 9, 5), float3 (15, 9, 5), float3 (15, 10, 5), float3 (0.6, 0.6, 0.6), 0.53),

	triangle (float3 (10, 9, 5), float3 (10, 10, 5), float3 (11, 10, 5), float3 (0.6, 0.6, 0.6), 0.46),
	triangle (float3 (11, 9, 5), float3 (11, 10, 5), float3 (12, 10, 5), float3 (0.6, 0.6, 0.6), 0.47),
	triangle (float3 (12, 9, 5), float3 (12, 10, 5), float3 (13, 10, 5), float3 (0.6, 0.6, 0.6), 0.48),
	triangle (float3 (13, 9, 5), float3 (13, 10, 5), float3 (14, 10, 5), float3 (0.6, 0.6, 0.6), 0.49),
	triangle (float3 (14, 9, 5), float3 (14, 10, 5), float3 (15, 10, 5), float3 (0.6, 0.6, 0.6), 0.40),

	triangle (float3 (10, 10, 5), float3 (11, 10, 5), float3 (11, 11, 5), float3 (0.6, 0.6, 0.6), 0.46),
	triangle (float3 (11, 10, 5), float3 (12, 10, 5), float3 (12, 11, 5), float3 (0.6, 0.6, 0.6), 0.47),
	triangle (float3 (12, 10, 5), float3 (13, 10, 5), float3 (13, 11, 5), float3 (0.6, 0.6, 0.6), 0.48),
	triangle (float3 (13, 10, 5), float3 (14, 10, 5), float3 (14, 11, 5), float3 (0.6, 0.6, 0.6), 0.49),
	triangle (float3 (14, 10, 5), float3 (15, 10, 5), float3 (15, 11, 5), float3 (0.6, 0.6, 0.6), 0.41),

	triangle (float3 (11, 10, 5), float3 (11, 11, 5), float3 (12, 11, 5), float3 (0.6, 0.6, 0.6), 0.37),
	triangle (float3 (12, 10, 5), float3 (12, 11, 5), float3 (13, 11, 5), float3 (0.6, 0.6, 0.6), 0.38),
	triangle (float3 (13, 10, 5), float3 (13, 11, 5), float3 (14, 11, 5), float3 (0.6, 0.6, 0.6), 0.39),
	triangle (float3 (14, 10, 5), float3 (14, 11, 5), float3 (15, 11, 5), float3 (0.6, 0.6, 0.6), 0.30),

	triangle (float3 (11, 11, 5), float3 (12, 11, 5), float3 (12, 12, 5), float3 (0.6, 0.6, 0.6), 0.37),
	triangle (float3 (12, 11, 5), float3 (13, 11, 5), float3 (13, 12, 5), float3 (0.6, 0.6, 0.6), 0.38),
	triangle (float3 (13, 11, 5), float3 (14, 11, 5), float3 (14, 12, 5), float3 (0.6, 0.6, 0.6), 0.39),
	triangle (float3 (14, 11, 5), float3 (15, 11, 5), float3 (15, 12, 5), float3 (0.6, 0.6, 0.6), 0.31),

	triangle (float3 (12, 11, 5), float3 (12, 12, 5), float3 (13, 12, 5), float3 (0.6, 0.6, 0.6), 0.28),
	triangle (float3 (13, 11, 5), float3 (13, 12, 5), float3 (14, 12, 5), float3 (0.6, 0.6, 0.6), 0.29),
	triangle (float3 (14, 11, 5), float3 (14, 12, 5), float3 (15, 12, 5), float3 (0.6, 0.6, 0.6), 0.20),

	triangle (float3 (12, 12, 5), float3 (13, 12, 5), float3 (13, 13, 5), float3 (0.6, 0.6, 0.6), 0.28),
	triangle (float3 (13, 12, 5), float3 (14, 12, 5), float3 (14, 13, 5), float3 (0.6, 0.6, 0.6), 0.29),
	triangle (float3 (14, 12, 5), float3 (15, 12, 5), float3 (15, 13, 5), float3 (0.6, 0.6, 0.6), 0.21),

	triangle (float3 (13, 12, 5), float3 (13, 13, 5), float3 (14, 13, 5), float3 (0.6, 0.6, 0.6), 0.19),
	triangle (float3 (14, 12, 5), float3 (14, 13, 5), float3 (15, 13, 5), float3 (0.6, 0.6, 0.6), 0.10),

	triangle (float3 (14, 13, 5), float3 (14, 14, 5), float3 (15, 14, 5), float3 (0.6, 0.6, 0.6), 0.05),

	triangle (float3 (13, 13, 5), float3 (14, 13, 5), float3 (14, 14, 5), float3 (0.6, 0.6, 0.6), 0.19),
	triangle (float3 (14, 13, 5), float3 (15, 13, 5), float3 (15, 14, 5), float3 (0.6, 0.6, 0.6), 0.11),

	triangle (float3 (14, 14, 5), float3 (15, 14, 5), float3 (15, 15, 5), float3 (0.6, 0.6, 0.6), 0.04),



	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0),//down
	//triangle(float3(5,15,5	),float3(15,15,5	),float3(15,15,15	),float3(.50,.15,.0),0),//up
	//triangle(float3(5,15,5	),float3(5,15,15	),float3(15,15,15	),float3(.50,.0,.15),0),//up																	 
	//triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.10, .10, .10), 0.85),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.10, .10, .10), 0.85),//back
	//triangle(float3(5,5,15	),float3(15,5,15	),float3(15,15,15	),float3(.0,.10,.50),0),//front
	//triangle(float3(5,5,15	),float3(5,15,15	),float3(15,15,15	),float3(.10,.0,.50),0),//front																	 
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.50, .50, .0), 0.6),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .50), 0),//left
	//triangle(float3(15,5,5	),float3(15,5,15	),float3(15,15,15	),float3(.10,.50,.0),0.60),//right
	//triangle(float3(15,5,5	),float3(15,15,5	),float3(15,15,15	),float3(.0,.50,.10),0.60),//right

	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15),//down
	triangle (float3 (5, 15, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.50, .15, .0), 0).moveX (15),//up
	triangle (float3 (5, 15, 5), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.50, .0, .15), 0).moveX (15),//up																	 
	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15),//back
	triangle (float3 (5, 5, 15), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.0, .10, .50), 0).moveX (15),//front
	triangle (float3 (5, 5, 15), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.10, .0, .50), 0).moveX (15),//front																	 
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15),//left
	triangle (float3 (15, 5, 5), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.10, .50, .0), 0).moveX (15),//right
	triangle (float3 (15, 5, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.0, .50, .10), 0).moveX (15),//right

	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveZ (-15),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveZ (-15),//down
	triangle (float3 (5, 15, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.50, .15, .0), 0).moveZ (-15),//up
	triangle (float3 (5, 15, 5), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.50, .0, .15), 0).moveZ (-15),//up																	 	   
	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveZ (-15),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveZ (-15),//back
	triangle (float3 (5, 5, 15), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.0, .10, .50), 0).moveZ (-15),//front
	triangle (float3 (5, 5, 15), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.10, .0, .50), 0).moveZ (-15),//front																	 	  
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveZ (-15),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveZ (-15),//left
	triangle (float3 (15, 5, 5), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.10, .50, .0), 0).moveZ (-15),//right
	triangle (float3 (15, 5, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.0, .50, .10), 0).moveZ (-15),//right

	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15).moveZ (-15),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15).moveZ (-15),//down
	triangle (float3 (5, 15, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.50, .15, .0), 0).moveX (15).moveZ (-15),//up
	triangle (float3 (5, 15, 5), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.50, .0, .15), 0).moveX (15).moveZ (-15),//up																	 
	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15).moveZ (-15),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15).moveZ (-15),//back
	triangle (float3 (5, 5, 15), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.0, .10, .50), 0).moveX (15).moveZ (-15),//front
	triangle (float3 (5, 5, 15), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.10, .0, .50), 0).moveX (15).moveZ (-15),//front																	 
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15).moveZ (-15),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15).moveZ (-15),//left
	triangle (float3 (15, 5, 5), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.10, .50, .0), 0).moveX (15).moveZ (-15),//right
	triangle (float3 (15, 5, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.0, .50, .10), 0).moveX (15).moveZ (-15),//right

	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveY (15),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveY (15),//down
	triangle (float3 (5, 15, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.50, .15, .0), 0).moveY (15),//up
	triangle (float3 (5, 15, 5), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.50, .0, .15), 0).moveY (15),//up																	 
	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveY (15),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveY (15),//back
	triangle (float3 (5, 5, 15), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.0, .10, .50), 0).moveY (15),//front
	triangle (float3 (5, 5, 15), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.10, .0, .50), 0).moveY (15),//front																	 
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveY (15),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveY (15),//left
	triangle (float3 (15, 5, 5), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.10, .50, .0), 0).moveY (15),//right
	triangle (float3 (15, 5, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.0, .50, .10), 0).moveY (15),//right

	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15).moveY (15),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15).moveY (15),//down
	triangle (float3 (5, 15, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.50, .15, .0), 0).moveX (15).moveY (15),//up
	triangle (float3 (5, 15, 5), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.50, .0, .15), 0).moveX (15).moveY (15),//up																	 
	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15).moveY (15),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15).moveY (15),//back
	triangle (float3 (5, 5, 15), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.0, .10, .50), 0).moveX (15).moveY (15),//front
	triangle (float3 (5, 5, 15), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.10, .0, .50), 0).moveX (15).moveY (15),//front																	 
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15).moveY (15),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15).moveY (15),//left
	triangle (float3 (15, 5, 5), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.10, .50, .0), 0).moveX (15).moveY (15),//right
	triangle (float3 (15, 5, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.0, .50, .10), 0).moveX (15).moveY (15),//right

	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveZ (-15).moveY (15),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveZ (-15).moveY (15),//down
	triangle (float3 (5, 15, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.50, .15, .0), 0).moveZ (-15).moveY (15),//up
	triangle (float3 (5, 15, 5), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.50, .0, .15), 0).moveZ (-15).moveY (15),//up																	 	   
	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveZ (-15).moveY (15),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveZ (-15).moveY (15),//back
	triangle (float3 (5, 5, 15), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.0, .10, .50), 0).moveZ (-15).moveY (15),//front
	triangle (float3 (5, 5, 15), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.10, .0, .50), 0).moveZ (-15).moveY (15),//front																	 	  
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveZ (-15).moveY (15),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveZ (-15).moveY (15),//left
	triangle (float3 (15, 5, 5), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.10, .50, .0), 0).moveZ (-15).moveY (15),//right
	triangle (float3 (15, 5, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.0, .50, .10), 0).moveZ (-15).moveY (15),//right

	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15).moveZ (-15).moveY (15),//down
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (15, 5, 15), float3 (.50, .0, .0), 0).moveX (15).moveZ (-15).moveY (15),//down
	triangle (float3 (5, 15, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.50, .15, .0), 0).moveX (15).moveZ (-15).moveY (15),//up
	triangle (float3 (5, 15, 5), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.50, .0, .15), 0).moveX (15).moveZ (-15).moveY (15),//up																	 
	triangle (float3 (5, 5, 5), float3 (15, 5, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15).moveZ (-15).moveY (15),//back
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (15, 15, 5), float3 (.0, .0, .50), 0).moveX (15).moveZ (-15).moveY (15),//back
	triangle (float3 (5, 5, 15), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.0, .10, .50), 0).moveX (15).moveZ (-15).moveY (15),//front
	triangle (float3 (5, 5, 15), float3 (5, 15, 15), float3 (15, 15, 15), float3 (.10, .0, .50), 0).moveX (15).moveZ (-15).moveY (15),//front																	 
	triangle (float3 (5, 5, 5), float3 (5, 5, 15), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15).moveZ (-15).moveY (15),//left
	triangle (float3 (5, 5, 5), float3 (5, 15, 5), float3 (5, 15, 15), float3 (.0, .50, .0), 0).moveX (15).moveZ (-15).moveY (15),//left
	triangle (float3 (15, 5, 5), float3 (15, 5, 15), float3 (15, 15, 15), float3 (.10, .50, .0), 0).moveX (15).moveZ (-15).moveY (15),//right
	triangle (float3 (15, 5, 5), float3 (15, 15, 5), float3 (15, 15, 15), float3 (.0, .50, .10), 0).moveX (15).moveZ (-15).moveY (15)//right

};

/*float3::float3(float3 & b) {
	x = b.v[0];
	y = b.v[1];
	z = b.v[2];
}

void float3::operator=(const float3& b) {
	x = b.v[0];
	y = b.v[1];
	z = b.v[2];
}*/

bool helpOrient (triangle t, float3 v1, float3 v2) {
	float3 normalization = t.normal ();
	double x1 = normalization.dot (v1 - t.p[0]);
	double x2 = normalization.dot (v2 - t.p[0]);
	return x1*x2 + EPSILON >= 0;
}
/*
Returns true if input ray intersect some object
*/


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
			bool isIntersection = objects[i].intersect (r, localHit); 
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
inline   bool Visible (const  world & wrld, const float3 & hit, const float3 & light, const int & id)
{
	double distToLight = hit.distance (light);
	float3 hit1;
	float3 sub = light - hit;
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
inline   bool Visible (const KDNode & root, const  world & wrld, const float3 & hit, const float3 & light, const triangle & tri)
{
	double distToLight = hit.distance (light);
	float3 hit1;
	float3 sub = light - hit;
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


inline   float3 Shade (const float3 & hit, const float3 & light) {
	//double ka = 0.1; //ambient coefficient
	return float3 (0.3, 0.3, 0.3);
}
/*
Return new reflected ray
*/
/*inline   Ray reflect (const Ray & r,const  triangle & obj,const float3 & hit) {
	float3 normal = obj.normal ();
	float3 iV = hit - float3(r.o);//inputfloat3tor
	normal = normal.normalization ();
	float3 rV = iV - ((normal*(iV.dot (normal))) * 2);//reflectfloat3tor
	Ray reflect (hit, rV.normalization ());//new reflect ray
	return reflect;
}
*/
inline   Ray reflect (const Ray & r,const  triangle & obj,const float3 & hit) {
	float3 normal = obj.normal ();
	float3 iV = hit - r.o;//inputfloat3tor
	normal = normal.normalization ();
	float3 rV = iV - ((normal*(iV.dot (normal))) * 2);//reflectfloat3tor
	Ray reflect (hit, rV.normalization ());//new reflect ray
	return reflect;
}

/*
Recursively trace the input ray with a light source and reflection
*/
float3 RayTrace (const  world  & wrld,const Ray & ray,unsigned int deep) {
	float3 color (0, 0, 0);
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

float3 KDTreeRayTrace (const  KDNode &root, const  world  & wrld, const Ray & ray,unsigned int deep) {
	float3 color (0, 0, 0);
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
		color = color*(1.0 - tr->reflect) + RayTrace (wrld, reflRay, deep--)*tr->reflect;
	}
	return color;
}


#ifndef kdtrace
void SimpleRender (const  world & wrld, const camera & cam, float3 c[], const imgSettings & img) {
#else
void SimpleRender (const  KDNode &root, const  world & wrld, const camera & cam, float3 c[], const imgSettings & img) {
#endif
	float3 r; 
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
					r = float3 ();
					//double dx = 0, dy = 0;
					float3 d = cam.cameraXangle*(((sx + .5) / 2 + x) / img.w - .5) +
						cam.cameraYangle*(((sy + .5) / 2 + y) / img.h - .5) +
						cam.cameraLocation.d;

				#ifndef kdtrace
					r += RayTrace (wrld, Ray (cam.cameraLocation.o + d * 140, d.normalization ()), 1);//
				#else
					r += KDTreeRayTrace (root, wrld, Ray (cam.cameraLocation.o + d * 140, d.normalization ()), 1);
				#endif

					c[i] += float3 (clamp (r.v[X]), clamp (r.v[Y]), clamp (r.v[Z]))*k;
				}
			//--i;
		}
	}
}

int main (int argc, char *argv[])
{
	//int w = 1024, h = 768;
	//int w = 640, h = 480;
	int w = 320, h = 240;
	int samps = argc == 2 ? atoi (argv[1]) / 4 : 1; // # samples 
	
	float3  r, *c = new float3[w*h];
	imgSettings img = imgSettings (w, h, samps);

	// // Rabbit
	/*Model_PLY rabbit;
	rabbit.Load ("bun_zipper_res4.ply");
	//camera cam (Ray (float3 (85, 55, 170), float3 (-0.45, -0.04, -1).normalization ()), float3 (w*.5135 / h));
	camera cam (Ray (float3 (0, 70, 220), float3 (0, -0.25, -1).normalization ()), float3 (w*.5135 / h,0,0));
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = 1;
	world wrld = world (objCount, lightsCount, obj, lights);
	plyToMass (rabbit, wrld);
	
	KDNode scene;
	double build_s = omp_get_wtime();
	buildKDTree(scene, wrld.objects, wrld.objCount,3);
	double build_f = omp_get_wtime();
	fprintf (stderr, "\rtime %5.3f\n", build_f-build_s);
	*/

	// // Cube
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = sizeof(lights) / sizeof(float3);
	camera cam (Ray (float3 (140, 45, 170), float3 (-0.7, -0.15, -1).normalization ()), float3 (w*.5135 / h));
	world wrld = world (objCount, lightsCount, obj, lights);
	
	KDNode scene;
	buildKDTree(scene, obj, objCount, 0);
	
	// // One triangle
	/*
	camera cam (Ray (float3 (105, 44, 190), float3 (0, 1, -0.2).normalization ()), float3 (w*.5135 / h));
	float3 light[] = {float3(85, 45, 170)};
	triangle tri[] = {
		triangle (float3 (85, 180, 120), float3 (105, 180, 150), float3 (65, 180, 190), float3 (.70, .10, .10), 0.1)};
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