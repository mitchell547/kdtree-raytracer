

//#include "OBJ.hpp"
//#include "Ply.hpp"
#include "Triangle.hpp"
#pragma once
struct world
{
	triangle*objects;
	float3	*lights;
	unsigned int objCount;
	unsigned int lightsCount;

	world ();
	world (const unsigned int objCount_, const unsigned int lightsCount_,
		const triangle*objects_, const float3	*lights_) :
		objCount (objCount_),
		lightsCount (lightsCount_)
	{
		objects = (triangle *)malloc (objCount_*sizeof(triangle));
		lights = (float3 *)malloc (lightsCount_*sizeof(float3));
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
};

/*template<typename T>
struct worldT
{
	//triangle *objects;
	Vec3<T> * a1;
	Vec3<T> * a2;
	Vec3<T> * a3;
	Vec3<T> * Na1;
	Vec3<T> * Na2;
	Vec3<T> * Na3;
	Vec3<T> * N;
	Vec3<T> * c;
	T * diffuse;
	T * specular;
	T * refract;
	
	Vec3<T>	*lights;
	int objCount;
	int lightsCount;

	  worldT (){};
	worldT (const unsigned int objCount_, const unsigned int lightsCount_,
		const polygon3<T>*objects_, const Vec3<T>	*lights_) :
		objCount (objCount_),
		lightsCount (lightsCount_)
	{
		objCount = objCount_;
		lightsCount = lightsCount_;
		//objects = (triangle *)malloc (objCount_*sizeof(triangle));
		a1	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));
		a2	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		a3	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		Na1 = (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		Na2 = (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		Na3 = (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		N	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		c	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		diffuse = (T*)malloc (objCount_*sizeof(T));
		specular = (T*)malloc (objCount_*sizeof(T));;
		refract = (T*)malloc (objCount_*sizeof(T));;
		lights = (Vec3<T> *)malloc (lightsCount_*sizeof(Vec3<T>));
		for (int i = 0; i < objCount_; ++i)
		{
			a1[i] = objects_[i].a1;
			a2[i] = objects_[i].a2;
			a3[i] = objects_[i].a3;
			Na1[i] = objects_[i].Na1;
			Na2[i] = objects_[i].Na2;
			Na3[i] = objects_[i].Na3;
			N[i] = objects_[i]._normal;
			c[i] = objects_[i].c;
			diffuse[i] = objects_[i].diffuse;
			specular[i] = objects_[i].specular;
			refract[i] = objects_[i].refract;
		}

		memcpy (lights, lights_, lightsCount_*sizeof(Vec3<T>));
	}
	void setObj (const polygon3<T>*objects_, const unsigned int objCount_)
	{
		objCount = objCount_;
		//objects = (triangle *)malloc (objCount_*sizeof(triangle));
		//memcpy (objects, objects_, objCount_*sizeof(triangle));
		a1	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));
		a2	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		a3	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		Na1 = (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		Na2 = (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		Na3 = (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		N	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		c	= (Vec3<T> *)malloc (objCount_*sizeof(Vec3<T>));;
		diffuse = (T*)malloc (objCount_*sizeof(T));
		specular = (T*)malloc (objCount_*sizeof(T));;
		refract = (T*)malloc (objCount_*sizeof(T));;
		
		for (int i = 0; i < objCount_; i++)
		{
			a1[i] = objects_[i].a1;
			a2[i] = objects_[i].a2;
			a3[i] = objects_[i].a3;
			Na1[i] = objects_[i].Na1;
			Na2[i] = objects_[i].Na2;
			Na3[i] = objects_[i].Na3;
			N[i] = objects_[i]._normal;
			c[i] = objects_[i].c;
			diffuse[i] = objects_[i].diffuse;
			specular[i] = objects_[i].specular;
			refract[i] = objects_[i].refract;
		}
	}
};*/

//typedef worldT<float> worldf;

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

/*
void  plyToMass (Model_PLY  & model, world & wrld)
{
	triangle * objects;
	unsigned int objCount_ = model.TotalFaces + 10;

	objects = (triangle *)malloc (objCount_*sizeof(triangle));
	int tr = 0;

	//#pragma omp parallel for schedule(dynamic, 1)        // OpenMP 
	for (unsigned int i = 0; i < objCount_ - 10; ++i)
	{

		Vec a1 = Vec (model.Faces_Triangles[tr + 0].x, model.Faces_Triangles[tr + 0].y, model.Faces_Triangles[tr + 0].z) * 200;
		Vec a2 = Vec (model.Faces_Triangles[tr + 1].x, model.Faces_Triangles[tr + 1].y, model.Faces_Triangles[tr + 1].z) * 200;
		Vec a3 = Vec (model.Faces_Triangles[tr + 2].x, model.Faces_Triangles[tr + 2].y, model.Faces_Triangles[tr + 2].z) * 200;

		float inten = model.Faces_intensity[i];
		Vec Na1 = Vec (model.faces_out[i].normals[0].x, model.faces_out[i].normals[0].y, model.faces_out[i].normals[0].z);// Normals[tr]);
		Vec Na2 = Vec (model.faces_out[i].normals[1].x, model.faces_out[i].normals[1].y, model.faces_out[i].normals[1].z);//Normals[tr + 1]);
		Vec Na3 = Vec (model.faces_out[i].normals[2].x, model.faces_out[i].normals[2].y, model.faces_out[i].normals[2].z);//Normals[tr + 2]);

		triangle a = triangle (a1, a2, a3, Vec (inten, inten, inten), Na1, Na2, Na3, 0.6);
		objects[i] = a;
		tr += 3;
	}

	objects[objCount_ - 10] = triangle (Vec (70, 5, -100), Vec (70, 5, 100), Vec (70, 105, -100), Vec (0, 0.3, 0), 0).moveZ (10);
	objects[objCount_ - 9] = triangle (Vec (70, 5, 100), Vec (70, 105, 100), Vec (70, 105, -100), Vec (0, 0.3, 0), 0).moveZ (10);

	objects[objCount_ - 8] = triangle (Vec (-70, 5, -100), Vec (-70, 105, -100), Vec (-70, 5, 100), Vec (0.3, 0, 0), 0).moveZ (10);
	objects[objCount_ - 7] = triangle (Vec (-70, 5, 100), Vec (-70, 105, -100), Vec (-70, 105, 100), Vec (0.3, 0, 0), 0).moveZ (10);

	objects[objCount_ - 6] = triangle (Vec (-70, 105, -100), Vec (-70, 105, 100), Vec (70, 105, 100), Vec (0.02, 0.02, 0.02), 0).moveZ (10);
	objects[objCount_ - 5] = triangle (Vec (-70, 105, -100), Vec (70, 105, -100), Vec (70, 105, 100), Vec (0.02, 0.02, 0.02), 0).moveZ (10);
	triangle b = triangle (Vec (-70, 105, -100), Vec (70, 5, -100), Vec (70, 105, -100), Vec (0.1, 0.1, 0.1), 0.6).moveZ (10);
	b.diffuse = 0;
	objects[objCount_ - 4] = b;
	b = triangle (Vec (-70, 5, -100), Vec (70, 5, -100), Vec (-70, 105, -100), Vec (0.1, 0.1, 0.1), 0.6).moveZ (10);
	b.diffuse = 0;
	objects[objCount_ - 3] = b;
	objects[objCount_ - 2] = triangle (Vec (-70, 5, -100), Vec (-70, 5, 100), Vec (70, 5, 100), Vec (0.01, 0.01, 0.02), 0).moveZ (10);//пол
	objects[objCount_ - 1] = triangle (Vec (-70, 5, -100), Vec (70, 5, 100), Vec (70, 5, -100), Vec (0.01, 0.01, 0.02), 0).moveZ (10);

	wrld.setObj (objects, objCount_);
}
*/
/*
void  objToMass (objmo::Model  & model, world & wrld)
{
	triangle * objects;
	std::vector<unsigned short> faces = model.faces["default"];
	unsigned int objCount_ = faces.size () / 3 + 12;

	objects = (triangle *)malloc (objCount_*sizeof(triangle));
	int tr = 0;

	//#pragma omp parallel for schedule(dynamic, 1)        // OpenMP 
	for (unsigned int i = 0; i < objCount_ - 12; ++i)
	{
		int vertex1 = faces[tr + 0];
		int vertex2 = faces[tr + 1];
		int vertex3 = faces[tr + 2];
		//Vec a1 = Vec (model.vertex[vertex1 * 3 + 0], model.vertex[vertex1 * 3 + 1], model.vertex[vertex1 * 3 + 2]) * 35;
		//Vec a2 = Vec (model.vertex[vertex2 * 3 + 0], model.vertex[vertex2 * 3 + 1], model.vertex[vertex2 * 3 + 2]) * 35;
		//Vec a3 = Vec (model.vertex[vertex3 * 3 + 0], model.vertex[vertex3 * 3 + 1], model.vertex[vertex3 * 3 + 2]) * 35 ;

		Vec Na1 = Vec (model.normal[vertex1 * 3 + 0], model.normal[vertex1 * 3 + 1], model.normal[vertex1 * 3 + 2]);// Normals[tr]);
		Vec Na2 = Vec (model.normal[vertex2 * 3 + 0], model.normal[vertex2 * 3 + 1], model.normal[vertex2 * 3 + 2]);//Normals[tr + 1]);
		Vec Na3 = Vec (model.normal[vertex3 * 3 + 0], model.normal[vertex3 * 3 + 1], model.normal[vertex3 * 3 + 2]);//Normals[tr + 2]);

		triangle a = triangle (a1, a2, a3, Vec (0.4, 0.4, 0.4), Na1, Na2, Na3, 0.3).moveY (5);
		objects[i] = a;
		tr += 3;
	}

	objects[objCount_ - 12] = triangle (Vec (70, 5, -100), Vec (70, 5, 100), Vec (70, 105, -100), Vec (0, 0.3, 0), 0).moveZ (10);
	objects[objCount_ - 11] = triangle (Vec (70, 5, 100), Vec (70, 105, 100), Vec (70, 105, -100), Vec (0, 0.3, 0), 0).moveZ (10);

	objects[objCount_ - 10] = triangle (Vec (-70, 5, -100), Vec (-70, 105, -100), Vec (-70, 5, 100), Vec (0.3, 0, 0), 0).moveZ (10);
	objects[objCount_ - 9] = triangle (Vec (-70, 5, 100), Vec (-70, 105, -100), Vec (-70, 105, 100), Vec (0.3, 0, 0), 0).moveZ (10);

	objects[objCount_ - 8] = triangle (Vec (-70, 105, -100), Vec (-70, 105, 100), Vec (70, 105, 100), Vec (0.02, 0.02, 0.02), 0).moveZ (10);
	objects[objCount_ - 7] = triangle (Vec (-70, 105, -100), Vec (70, 105, -100), Vec (70, 105, 100), Vec (0.02, 0.02, 0.02), 0).moveZ (10);
	
	triangle b = triangle (Vec (-70, 105, -100), Vec (70, 5, -100), Vec (70, 105, -100), Vec (0.1, 0.1, 0.1), 0.8).moveZ (10);
	b.diffuse = 0;
	objects[objCount_ - 6] = b;

	b = triangle (Vec (-70, 5, -100), Vec (70, 5, -100), Vec (-70, 105, -100), Vec (0.1, 0.1, 0.1), 0.8).moveZ (10);
	b.diffuse = 0;
	objects[objCount_ - 5] = b;
	b = triangle (Vec (0, 5, -100), Vec (70, 5, -70), Vec (0, 105, -100), Vec (0.1, 0.1, 0.1), 0.9).moveZ (10);
	b.diffuse = 0;
	objects[objCount_ - 4] = b;
	b = triangle (Vec (70, 5, -70), Vec (70, 105, -70), Vec (0, 105, -100), Vec (0.1, 0.1, 0.1), 0.9).moveZ (10);
	b.diffuse = 0;
	objects[objCount_ - 3] = b;

	objects[objCount_ - 2] = triangle (Vec (-70, 5, -100), Vec (-70, 5, 100), Vec (70, 5, 100), Vec (0.01, 0.01, 0.02), 0).moveZ (10);//пол
	objects[objCount_ - 1] = triangle (Vec (-70, 5, -100), Vec (70, 5, 100), Vec (70, 5, -100), Vec (0.01, 0.01, 0.02), 0).moveZ (10);

	wrld.setObj (objects, objCount_);
}
*/