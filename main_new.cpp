#include <omp.h>
#include "Ply.hpp"
#include "Types.hpp"

#define FOV_ANGLE 0.5135

struct Camera
{
	Ray cameraLocation;
	Vec cameraXangle;
	Vec cameraYangle;
	Camera ();
	Camera (Ray cameraLocation_, Vec cameraXangle_) : 
		cameraLocation (cameraLocation_),	
		cameraXangle (cameraXangle_)
	{
		cameraYangle = (cameraXangle_.cross(cameraLocation_.d)).normalization()*FOV_ANGLE;
	}
};

int main(int argc, char *argv[]) {
	double start = omp_get_wtime();
	Model_PLY rabbit;
	rabbit.Load("bun_zipper_res3.ply");

	const int w = 1024, h = 768;
	const int samples = ((argc == 2) ? atoi (argv[1]) / 4 : 1); 

	Camera cam(Ray(Vec(85, 55, 170), Vec(-0.45, -0.04, -1).normalization ()), Vec(w*.5135 / h));
	

	return 0;
}