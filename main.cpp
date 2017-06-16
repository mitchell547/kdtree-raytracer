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
#define EPSILON 0.000000001
#define INF 1e20
//#define standartTest //  or... (ray triangle test)
//#define baricentTest


// Render settings:
// "Настройки" рендера:

//#define sampling	// antialiasing
#define MOLLER_TRUMBORE_INTERSECT
#define REFLECTION_DEPTH 2
#define TREE_DEPTH 1

// Choose model
// (*) Выбор модели (одна из)
//#define rabbit_model
#define cube_model

// Using k-d tree tracing
// (*) Включение трассировки k-d дерева
#define kdtrace


#include "Include/kdtree.h"
#include "Include/BasicRayTracer.h"
#include "Include/KDTreeRayTracer.h"

// Cube model
// Сцена с кубом
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

int main (int argc, char *argv[])
{
	// Image settings
	// Параметры изображения
	//int w = 1024, h = 768;
	//int w = 640, h = 480;
	int w = 320, h = 240;
	int samps = argc == 2 ? atoi (argv[1]) / 4 : 1; // # samples 
	
	// Output image
	// Выходное изображение
	Vec  r, *c = new Vec[w*h];
	imgSettings img = imgSettings (w, h, samps);

	// 3D Scenes:
	// Сцены:

	// // Модель кролика 
#ifdef rabbit_model
	Model_PLY rabbit;
	rabbit.Load ("bun_zipper_res4.ply");
	//camera cam (Ray (Vec (85, 55, 170), Vec (-0.45, -0.04, -1).normalization ()), Vec (w*.5135 / h));
	camera cam (Ray (Vec (0, 70, 220), Vec (0, -0.25, -1).normalization ()), Vec (w*.5135 / h,0,0));
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = 2;
	world wrld = world (objCount, lightsCount, obj, lights);
	plyToMass (rabbit, wrld);

	KDNode scene;
	double build_s = omp_get_wtime();
	buildKDTree(scene, wrld.objects, wrld.objCount, TREE_DEPTH);
	double build_f = omp_get_wtime();
	fprintf (stderr, "\rBuild time %5.3f\n", build_f-build_s);
#endif

	// // Модель куба
#ifdef cube_model
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = sizeof(lights) / sizeof(Vec);
	camera cam (Ray (Vec (140, 45, 170), Vec (-0.7, -0.15, -1).normalization ()), Vec (w*.5135 / h));
	world wrld = world (objCount, lightsCount, obj, lights);
	
	KDNode scene;
	buildKDTree(scene, obj, objCount, TREE_DEPTH);
#endif

	// One triangle
	// // Один треугольник перед камерой
	/*
	camera cam (Ray (Vec (105, 44, 190), Vec (0, 1, -0.2).normalization ()), Vec (w*.5135 / h));
	Vec light[] = {Vec(85, 45, 170)};
	triangle tri[] = {
		triangle (Vec (85, 180, 120), Vec (105, 180, 150), Vec (65, 180, 190), Vec (.70, .10, .10), 0.1)};
	world wrld = world (1, 1, tri, light);
	*/

	// Rendering
	// Рендеринг
	double start = omp_get_wtime ();
	#ifndef kdtrace
		SimpleRender(wrld, cam, c, img);	// Basic
	#else
		SimpleRender(scene, wrld, cam, c, img);	// K-d tree
	#endif
	double end = omp_get_wtime ();

	fprintf (stderr, "\nElapsed time %5.3f", end-start);

	// Output
	// Вывод
	writeToBmp (c, w, h);
	getchar();
}