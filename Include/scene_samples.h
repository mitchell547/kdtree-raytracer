#include "Include/Geometry/Triangle.hpp"
#include "Include/Geometry/Vec.hpp"
#include "Include/Misc/Ply.hpp"

world loadRabbitModel();
world loadCubeModel();

// Cube model
// Сцена с кубом
Vec lights[] = { Vec (10, 70, 55),
				Vec (60, 17, 7) };
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
	triangle (Vec (5, 5, 5), Vec (15, 5, 5), Vec (15, 15, 5), Vec (.10, .10, .10), 0.0),//back
	triangle (Vec (5, 5, 5), Vec (5, 15, 5), Vec (15, 15, 5), Vec (.10, .10, .10), 0.0),//back
	//triangle(Vec(5,5,15	),Vec(15,5,15	),Vec(15,15,15	),Vec(.0,.10,.50),0),//front
	//triangle(Vec(5,5,15	),Vec(5,15,15	),Vec(15,15,15	),Vec(.10,.0,.50),0),//front																	 
	triangle (Vec (5, 5, 5), Vec (5, 5, 15), Vec (5, 15, 15), Vec (.50, .50, .0), 0.0),//left
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

triangle cube[] = {
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
};


world loadRabbitModel() {
	Model_PLY rabbit;
	rabbit.Load ("bun_zipper_res4.ply");
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = 2;
	world wrld = world (objCount, lightsCount, obj, lights);
	plyToMass (rabbit, wrld);
	return wrld;
}

world loadCubeModel() {
	int objCount = sizeof(obj) / sizeof(triangle);
	int lightsCount = sizeof(lights) / sizeof(Vec);
	world wrld = world (objCount-100, lightsCount, obj+100, lights);
	wrld.addQuad(X, 5, 0, 0, 20, 20, float3(0.9, 0.9, 0.0));
	return wrld;
}