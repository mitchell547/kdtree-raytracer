//#include "Vec.hpp"
#include "Include/Geometry/float3.h"
#pragma once

struct Ray
{
	//Vec o, d; //o-начало d-направление
	float3 o, d;
	  //Ray (Vec o_, Vec d_) : o (o_), d (d_) {}
	Ray (float3 o_, float3 d_) : o (o_), d (d_) {}
};
