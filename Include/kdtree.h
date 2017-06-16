#include "Include/Geometry/AABB.h"
#include "Include/Geometry/Triangle.hpp"
#include <algorithm>
#include <stack>

#pragma once

struct KDNode {
	AABB box;	// ограничивающий параллелепипед узла
	AXIS split_axis;	// ось, по которой разбивается узел
	float split_coord;	// координата плоскости разбиения
	triangle * triangles;	// массив входящих в листовой узел треугольников
	int tris_cnt;	
	KDNode * left;
	KDNode * right;
};

KDNode* buildKDTree(KDNode &root, triangle * triangles, int tri_cnt, int depth);

triangle* traceKDTree(KDNode &root);



float SAH(const float3 & left_box, int left_cnt, const float3 & right_box, int right_cnt) {
	return getSurfaceArea(left_box) * left_cnt + getSurfaceArea(right_box) * right_cnt;
}

void copy_triangles(KDNode &kd, triangle * triangles, int cnt) {
	kd.triangles = new triangle[cnt];
	for (int i = 0; i < cnt; ++i) 
		kd.triangles[i] = triangles[i];
}

float raySplitIntersect(const Ray & ray, AXIS a, float coord) {
	return (coord - ray.o.v[a]) / (ray.d.v[a]);
}

const int BINS_CNT = 32;

KDNode* buildKDTree(KDNode &root, triangle * triangles, int tri_cnt, int depth) {
	//KDNode root;
	root.tris_cnt = tri_cnt;
	root.left = nullptr;
	root.right = nullptr;
	
	if (tri_cnt == 0) 
		return &root;

	// Ищем ограничивающий бокс всей сцены и заодно центры треугольников
	float3 * mid_points = new float3[tri_cnt];	// "центры" треугольников
	AABB global_box = getTriangleAABB(triangles[0]); // бокс сцены
	mid_points[0] = (global_box.max + global_box.min) / 2.0;
	for (int i = 1; i < tri_cnt; ++i) { 
		AABB box = getTriangleAABB(triangles[i]);
		addAABB(global_box, box);
		mid_points[i] = (box.max + box.min) / 2.0;
	}

	root.box = global_box;

	if (depth == 0) {
		copy_triangles(root, triangles, tri_cnt);
		return &root;
	}

	root.tris_cnt = 0;
	// // Ищем оптимальное пересечение

	// Берём наибольшую длину бокса в качестве проверяемой оси
	AXIS split_axis = X;	// ось, по которой будем разбивать на интервалы
	float3 lengths = global_box.max - global_box.min;

	if (lengths.v[Y] > lengths.v[split_axis]) split_axis = Y;
	if (lengths.v[Z] > lengths.v[split_axis]) split_axis = Z;

	// Перебераем возможные плоскости 

	// Подсчитываем кол-во треугольников в интервалах
	float step = lengths.v[split_axis] / BINS_CNT;
	int * bins_sizes = new int[BINS_CNT];	// массив количеств треугольников в интервалах
	for (int i = 0; i < BINS_CNT; ++i)
		bins_sizes[i] = 0;

	float coord = global_box.min.v[split_axis];
	for (int i = 0; i < tri_cnt; ++i) {
		for (int j = 0; j < BINS_CNT; ++j) {
			if (mid_points[i].v[split_axis] >= step * j + coord
					&& mid_points[i].v[split_axis] <= step * (j+1) + coord) {
				bins_sizes[j]++;
				break;
			}
		}
	}

	// Считаем SAH для интервалов и выбираем лучшую плоскость
	float min_SAH = tri_cnt * getSurfaceArea(global_box);
	float cur_SAH;
	int split_plane;	// индекс наилучшей плоскости разбиения
	int left_cnt = 0, right_cnt = tri_cnt;
	int min_left_cnt = 0;	// количество треугольников слева от разбивающей плоскости
	float3 left_box = getSizes(global_box), right_box = getSizes(global_box);
	left_box.v[split_axis] = 0;	// ?
	
	//   0 1 2 3 4 5 6 7 8 9 . . . . 14
	// [-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-]  }16 bins
	for (int i = 0; i < BINS_CNT-1; ++i) {
		left_box.v[split_axis] += step;
		right_box.v[split_axis] -= step;

		left_cnt = left_cnt + bins_sizes[i];
		right_cnt = right_cnt - bins_sizes[i];

		cur_SAH = SAH(left_box, left_cnt, right_box, right_cnt);

		if (cur_SAH < min_SAH) {
			min_SAH = cur_SAH;
			split_plane = i;
			min_left_cnt = left_cnt;
		}
	}

	delete[] bins_sizes;
	
	root.split_axis = split_axis;
	root.split_coord = global_box.min.v[split_axis] + step * (split_plane + 1);

	// // Формируем левые и правые узлы

	printf("\n%d: %d / %d", depth, min_left_cnt, tri_cnt - min_left_cnt);

	triangle * l = new triangle[min_left_cnt];
	triangle * r = new triangle[tri_cnt - min_left_cnt];
	int lcnt = 0, rcnt = 0;
	for (int i = 0; i < tri_cnt; ++i) {
		if (mid_points[i].v[split_axis] <= root.split_coord) {
			l[lcnt] = triangles[i];
			++lcnt;
		} else {
			r[rcnt] = triangles[i];
			++rcnt;
		}
	}

	delete[] mid_points;

	// Рекурсивно формируем узлы дерева
	root.left = new KDNode();
	root.left = buildKDTree(*root.left, l, min_left_cnt, depth-1);	
	delete[] l;

	root.right = new KDNode();
	root.right = buildKDTree(*root.right, r, tri_cnt - min_left_cnt, depth-1); 
	delete[] r;

	return &root;
}

triangle* traceKDTree(const KDNode &root, const Ray &ray, float3 &pHit) {

	struct TraceInfo {
		float tmax;
		KDNode * node;
	};

	float tmin, tmax, tsplit;
	// Проверка глобального бокса
	bool isIntersect = RayAABBIntersect(ray, root.box, tmin, tmax);
	if (!isIntersect)
		return nullptr;
	
	std::stack<TraceInfo> s;

	const KDNode * node; // Текущий узел
	node = &root;

	float min_dist = INF;
	const KDNode * hit_node = node;
	int id;
	// Трассируем, пока не достигнем результата
	while (true) {

		// Листовой узел
		while (node->left == nullptr && node->right == nullptr) {
			float3 hit;		
			
			for (int i = 0; i < node->tris_cnt; ++i) {
			#ifndef MOLLER_TRUMBORE_INTERSECT
				if (node->triangles[i].intersect(ray, hit)) {
			#else
				if (node->triangles[i].mollerTrumboreIntersect(ray, hit)) {
			#endif
					float dist = hit.distance(ray.o);
					if (dist < min_dist) {
						min_dist = dist;
						id = i;
						hit_node = node;
						pHit = hit;
					}
				}
			}

			if (s.empty()) {
				if (min_dist <= INF)
					return &hit_node->triangles[id];
				else
					return nullptr;
			}
			// Возвращаем найденное пересечение, либо возвращаемся к стеку и трассируем дальше
			//if (min_dist <= INF)
			//	return &hit_node->triangles[id];
			//else {
			{

				tmin = tmax;
				TraceInfo ti;
				ti = s.top();
				node = ti.node;
				tmax = ti.tmax;
				s.pop();
			}
		}

		tsplit = raySplitIntersect(ray, node->split_axis, node->split_coord);

		if(tsplit <= tmin) {
			node = node->left;
		}
		else if(tsplit >= tmax) {
			node = node->right;
		}
		else {
			TraceInfo ti;
			ti.node = node->left;
			ti.tmax = tmax;
			s.push(ti);
			node = node->right;
			tmax = tsplit;
		}
	}

	if (min_dist < INF)
		return &hit_node->triangles[id];
	else
		return nullptr;
}