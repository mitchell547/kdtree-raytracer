/* Demonstrates how to load PLY files
* Needs some refactoring.
*
* Model needs to be triangulated
* Use blender
*
* Just the class for loading PLY files.
*
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>
//#include "Types.hpp"
#include "../Geometry/Vec.hpp"
#include "../Geometry/Triangle.hpp"
#include "../World.hpp"
//#define Vecf Vec
//typedef Vec Vecf;
typedef Vec3<float> Vecf;
//typedef Vec3<float> Vec;	//???

class Model_PLY {
	public:
		Model_PLY ();
		int Model_PLY::Load (char *filename);
		//int Model_PLY::LoadObj (char *filename);
		//void Model_PLY::Draw ();
		//Vecf Model_PLY::calculateNormal (Vecf &coord1, Vecf  &coord2, Vecf  &coord3);

		Vecf  * Faces_Triangles;
		float* Faces_intensity;
		float* Faces_Quads;
		float* Vertex_Buffer;
		Vecf  * Normals;

		int *  allpointalltr;
		int TotalConnectedTriangles;
		int TotalConnectedQuads;
		int TotalConnectedPoints;
		int TotalFaces;

		struct face {
			Vecf points[3];
			Vecf normals[3];
		};
		face * faces_out;// [M]

	private:
		FILE * openFile(char* filename);
};

Model_PLY::Model_PLY () {

}

/*Vecf Model_PLY::calculateNormal (Vecf &coord1, Vecf  &coord2, Vecf  &coord3) {
	// calculate Vector1 and Vector2 
	Vecf va, vb, vr, val;
	va = coord1 - coord2;
	vb = coord1 - coord3;	
	vr = va.cross (vb);
	// normalization factor 
	Vecf norm = vr.normalization ();
	return norm;
}	// doesnt work with Vecf
*/


int Model_PLY::Load (char* filename) {
	this->TotalConnectedTriangles = 0;
	this->TotalConnectedQuads = 0;
	this->TotalConnectedPoints = 0;

	/*char* pch = strstr (filename, ".ply");
	if (pch == NULL) {
		printf ("File does not have a .PLY extension. ");
		return -1;
	}

	FILE* file = fopen (filename, "r");

	if (file == NULL) {
		printf ("File can't be opened\n"); 
		return -1;
	}

	char header[5];
	fscanf(file, "%s", header);
	if (strcmp(header, "ply") != 0)	{
		printf("The file is not in a PLY format\n");
		return -1;
	}*/

	FILE* file = this->openFile(filename);
	if (file == NULL)
		return -1;

	fseek (file, 0, SEEK_END);
	long long fileSize = ftell (file);
	fseek (file, 0, SEEK_SET);

	//Vertex_Buffer = (float*)malloc (ftell (file)); // there was bug, because fteel returns 0

	if (Vertex_Buffer == NULL) 
	{
		printf("Cannot allocate memory for vertex buffer\n");
		return -1;
	}

	/*Faces_Triangles = (Vecf * )malloc (fileSize * sizeof(Vecf) / 3);
	Faces_intensity = (float*)malloc (fileSize * sizeof(float) / 3);
	Normals = ((Vecf *)malloc (fileSize * sizeof(Vecf) / 3));
	*/

	int i = 0;
	int quads_index = 0;
	int triangle_index = 0;
			
	char buffer[1024];

	fgets (buffer, 300, file);			

	// Read header

	// Find number of vertexes
	while (strncmp ("element vertex", buffer, strlen ("element vertex")) != 0)
	{
		fgets (buffer, 300, file);			
	}
	strcpy (buffer, buffer + strlen ("element vertex"));
	sscanf (buffer, "%i", &this->TotalConnectedPoints);
	Vertex_Buffer = (float*)malloc (TotalConnectedPoints * 3 * sizeof(float));

	// Find number of faces
	//fseek (file, 0, SEEK_SET);
	while (strncmp ("element face", buffer, strlen ("element face")) != 0)
	{
		fgets (buffer, 300, file);			
	}
	strcpy (buffer, buffer + strlen ("element face"));
	sscanf (buffer, "%i", &this->TotalFaces);

	Faces_Triangles = (Vecf * )malloc (TotalFaces * sizeof(Vecf) * 3);
	Faces_intensity = (float*)malloc (TotalFaces * sizeof(float)  * 2);
	Normals = (Vecf *)malloc (TotalFaces * sizeof(Vecf) * 3);

	// go to end_header
	while (strncmp ("end_header", buffer, strlen ("end_header")) != 0)
	{
		fgets (buffer, 300, file);			
	}

	// Read vertices
	i = 0;
	Vecf *points = new Vecf[TotalConnectedPoints] (); 
	for (int iterator = 0; iterator < this->TotalConnectedPoints; iterator++)
	{
		fgets (buffer, 300, file);
		sscanf (buffer, "%f %f %f", &Vertex_Buffer[i], &Vertex_Buffer[i+1], &Vertex_Buffer[i+2]);
		points[iterator] = Vecf (Vertex_Buffer[i], Vertex_Buffer[i + 1], Vertex_Buffer[i + 2]);
		i += 3;
	}

	// Read faces
	i = 0;
	allpointalltr = (int *)malloc (TotalFaces * 3 * sizeof(int));
			
	int** faces = new int*[TotalFaces];// [M][3]
	for (int i = 0; i < TotalFaces; ++i)
		faces[i] = new int[3];

	for (int iterator = 0; iterator < this->TotalFaces; iterator++)
	{
		fgets (buffer, 300, file);
		if (buffer[0] == '3')
		{
			int vertex1 = 0, vertex2 = 0, vertex3 = 0;
			buffer[0] = ' ';
			sscanf (buffer, "%i%i%i", &vertex1, &vertex2, &vertex3);
					
			faces[iterator][0] = vertex1;
			faces[iterator][1] = vertex2;
			faces[iterator][2] = vertex3;
					
			Faces_intensity[iterator] = (Vertex_Buffer[3 * vertex1 + 4] + Vertex_Buffer[3 * vertex2 + 4] + Vertex_Buffer[3 * vertex3 + 4]) / 3;

			Faces_Triangles[triangle_index + 0] = Vecf (Vertex_Buffer[3 * vertex1], Vertex_Buffer[3 * vertex1 + 1], Vertex_Buffer[3 * vertex1 + 2]);
			Faces_Triangles[triangle_index + 1] = Vecf (Vertex_Buffer[3 * vertex2], Vertex_Buffer[3 * vertex2 + 1], Vertex_Buffer[3 * vertex2 + 2]);
			Faces_Triangles[triangle_index + 2] = Vecf (Vertex_Buffer[3 * vertex3], Vertex_Buffer[3 * vertex3 + 1], Vertex_Buffer[3 * vertex3 + 2]);
					
			triangle_index += 3;
			TotalConnectedTriangles += 3;
		}
		i += 3;
	}

	//1) вычисляем нормали всех полигонов и заносим в нормали вершин
	Vecf * normals = new Vecf[TotalConnectedPoints] ();
	for (int p = 0; p < TotalConnectedPoints; p++)
		normals[p] = Vecf (0, 0, 0);
			
	for (int f = 0; f < TotalFaces; f++)
	{
		Vecf n;
		n =  (points[faces[f][1]] - points[faces[f][0]]).cross( points[faces[f][2]] - points[faces[f][1]]);
		n = n.normalization ();

		for (int v = 0; v < 3; v++)
			normals[faces[f][v]] = normals[faces[f][v]] + n;
	}

	//2) нормируем
	for (int f = 0; f < TotalConnectedPoints; f++)
	{
		normals[f] = normals[f].normalization ();
	}
	
	faces_out = new face[TotalFaces] ();
	//3) заносим в face_out
	for (int f = 0; f<TotalFaces; f++)
		for (int v = 0; v < 3; v++)
		{

			//faces_out[f].points[v] = faces[f][v];
			faces_out[f].normals[v] = normals[faces[f][v]];
		}

	fclose (file);

	return 0;
}

FILE * Model_PLY::openFile(char* filename)
{
	if (strstr (filename, ".ply") == NULL) {
		printf ("File does not have a .PLY extension\n");
		return NULL;
	}

	FILE* file = fopen (filename, "r");

	if (file == NULL) {
		printf ("File can't be opened\n"); 
		return NULL;
	}

	char header[5];
	fscanf(file, "%s", header);
	if (strcmp(header, "ply") != 0)	{
		printf("The file is not in a PLY format\n");
		return NULL;
	}

	return file;
}

void  plyToMass (Model_PLY  & model, world & wrld)
{
	triangle * objects;
	unsigned int objCount_ = model.TotalFaces + 10;
	
	objects = (triangle *)malloc((objCount_) * sizeof(triangle));
	int tr = 0;
	for (unsigned int i = 0; i < objCount_ - 10; ++i) {
		Vec a1 = Vec (model.Faces_Triangles[tr].x, model.Faces_Triangles[tr].y, model.Faces_Triangles[tr].z);
		Vec a2 = Vec (model.Faces_Triangles[tr+1].x, model.Faces_Triangles[tr+1].y, model.Faces_Triangles[tr+1].z);
		Vec a3 = Vec (model.Faces_Triangles[tr+2].x, model.Faces_Triangles[tr+2].y, model.Faces_Triangles[tr+2].z);

		float inten =  model.Faces_intensity[i];
		triangle a = triangle (a1 * 200, a2 * 200, a3 * 200, Vec (inten, inten, inten), 0.6);
		/*triangle a = triangle (model.Faces_Triangles[tr] * 200, 
			model.Faces_Triangles[tr + 1] * 200, 
			model.Faces_Triangles[tr + 2] * 200, Vec (inten, inten, inten), 0);
		*/
		objects[i] = a;
		//tr += 9;
		tr += 3;
	}

	objects[objCount_ - 10] = triangle (Vec (70, 5, -100), Vec (70, 5, 100), Vec (70, 105, -100), Vec (0, 0.3, 0), 0).moveZ (10);
	objects[objCount_ - 9] = triangle (Vec (70, 5, 100), Vec (70, 105, 100), Vec (70, 105, -100), Vec (0, 0.3, 0), 0).moveZ (10);

	objects[objCount_ - 8] = triangle (Vec (-70, 5, -100), Vec (-70, 105, -100), Vec (-70, 5, 100), Vec (0.3, 0, 0), 0).moveZ (10);
	objects[objCount_ - 7] = triangle (Vec (-70, 5, 100), Vec (-70, 105, -100), Vec (-70, 105, 100), Vec (0.3, 0, 0), 0).moveZ (10);

	objects[objCount_ - 6] = triangle (Vec (-70, 105, -100), Vec (-70, 105, 100), Vec (70, 105, 100), Vec (0.02, 0.02, 0.02), 0).moveZ (10);
	objects[objCount_ - 5] = triangle (Vec (-70, 105, -100), Vec (70, 105, -100), Vec (70, 105, 100), Vec (0.02, 0.02, 0.02), 0).moveZ (10);
	triangle b = triangle (Vec (-70, 105, -100), Vec (70, 5, -100), Vec (70, 105, -100), Vec (0.1, 0.1, 0.1), 0.6).moveZ (10);
	//b.diffuse = 0;
	objects[objCount_ - 4] = b;
	b = triangle (Vec (-70, 5, -100), Vec (70, 5, -100), Vec (-70, 105, -100), Vec (0.1, 0.1, 0.1), 0.6).moveZ (10);
	//b.diffuse = 0;
	objects[objCount_ - 3] = b;
	objects[objCount_ - 2] = triangle (Vec (-70, 5, -100), Vec (-70, 5, 100), Vec (70, 5, 100), Vec (0.01, 0.01, 0.02), 0).moveZ (10);//пол
	objects[objCount_ - 1] = triangle (Vec (-70, 5, -100), Vec (70, 5, 100), Vec (70, 5, -100), Vec (0.01, 0.01, 0.02), 0).moveZ (10);


	wrld.setObj (objects, objCount_);	
	delete[] objects;
}