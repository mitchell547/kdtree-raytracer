

//#include "OBJ.hpp"
//#include "Ply.hpp"
#include "Include/Geometry/Triangle.hpp"
#pragma once
struct world
{
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
		//memcpy (objects, objects_,  objCount_*sizeof(triangle));

		//memcpy (lights, lights_,  lightsCount_*sizeof(Vec));
	}

	void setObj (const triangle*objects_, const unsigned int objCount_)
	{
		objCount = objCount_;
		objects = (triangle *)malloc (objCount_*sizeof(triangle));
		for (unsigned int i = 0; i < objCount; ++i)
			objects[i] = objects_[i];
		//memcpy (objects, objects_,  objCount_*sizeof(triangle));
	}

	void addQuad(AXIS axis, float z, float x1, float y1, float x2, float y2, float3 color) {
		float3 coords[4];
		switch (axis) {
			case X: 
				coords[0] = float3(0.0, x1, y1);
				coords[1] = float3(0.0, x2, y1);
				coords[2] = float3(0.0, x2, y2);
				coords[3] = float3(0.0, x1, y2);
				break;
			case Y: 
				coords[0] = float3(x1, 0.0, y1);
				coords[1] = float3(x2, 0.0, y1);
				coords[2] = float3(x2, 0.0, y2);
				coords[3] = float3(x1, 0.0, y2);
				break;
			case Z: 
				coords[0] = float3(x1, y1, 0.0);
				coords[1] = float3(x2, y1, 0.0);
				coords[2] = float3(x2, y2, 0.0);
				coords[3] = float3(x1, y2, 0.0);
				break;
		}
		triangle tris[2] = 
			{triangle(coords[0], coords[1], coords[2], color, 0.0),
			triangle(coords[2], coords[3], coords[0], color, 0.0)};
		objects = (triangle*)realloc(objects, (objCount+2) * sizeof(triangle));
		objects[objCount] = tris[0];
		objects[objCount+1] = tris[1];
		objCount += 2;
	}
};



struct camera
{
	Ray cameraLocation;
	//Vec cameraXangle;
	//Vec cameraYangle;
	float3 cameraXangle, cameraYangle;
	camera ();
	camera (Ray cameraLocation_, float3 cameraXangle_) :
		cameraLocation (cameraLocation_),cameraXangle (cameraXangle_)
	{
		cameraYangle = (cameraXangle_.cross (cameraLocation_.d)).normalization ()*.5135;
	}
};
struct imgSettings
{
	unsigned int w;//width
	unsigned int h;//hight
	unsigned int samps;//sampels per pixel
	imgSettings ();
	imgSettings (unsigned int w_, unsigned int h_, unsigned int samps_) :w (w_), h (h_), samps (samps_){}
};

