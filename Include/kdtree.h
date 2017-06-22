#include "Include/Geometry/AABB.h"
#include "Include/Geometry/Triangle.hpp"
#include <algorithm>
#include <stack>
#include "assert.h"

#pragma once

const int BINS_CNT = 32;

struct KDNode {
	AXIS split_axis;	// ось, по которой разбивается узел 
	float split_coord;	// координата плоскости разбиения
	int left;	// Если left < right, то является внутернним узлом
	int right;	// Если left >= right, то является листовым узлом
};

struct KDScene {
	AABB bbox;	
	int lights_cnt;
	KDNode* nodes;	// [0] is root node
	float3* lights;
	triangle* triangles;
};



KDScene* buildKDScene(triangle * triangles, int tris_cnt, float3 lights, int light_cnt, int depth);

int traceKDScene(const KDScene & scene, const Ray & ray, float3 & hit, float3 & bari, int & edgeHit);

void buildKDNode(KDNode * nodes, int node_id, triangle * triangles, int left_tri, int right_tri, int depth);



float SAH(const float3 & left_box, int left_cnt, const float3 & right_box, int right_cnt) {
	return getSurfaceArea(left_box) * left_cnt + getSurfaceArea(right_box) * right_cnt;
}

void copy_triangles(triangle * dest, triangle * src, int cnt) {
	for (int i = 0; i < cnt; ++i) dest[i] = src[i];
}

float raySplitIntersect(const Ray & ray, AXIS a, float coord) {
	return (coord - ray.o.v[a]) / (ray.d.v[a]);
}

AABB getSceneAABB(triangle * triangles, int tris_cnt) {
	if (tris_cnt <= 0) return AABB();
	AABB box = getTriangleAABB(triangles[0]);
	for (int i = 1; i < tris_cnt; ++i) 
		addAABB(box, getTriangleAABB(triangles[i]));
	return box;
}

void countBins(int * bins_sizes, int bins_cnt, float3 * mid_points, int points_cnt, AABB box, AXIS split_axis) {
	float step = (box.max - box.min).v[split_axis] / BINS_CNT;
	float coord = box.min.v[split_axis];
	for (int i = 0; i < points_cnt; ++i) {
		float point = mid_points[i].v[split_axis];
		if (point <= coord + step) {
			bins_sizes[0]++;
		} else if (point > coord + step * (BINS_CNT-1)) {
			bins_sizes[BINS_CNT-1]++;
		} else {
			float l_off = coord, r_off = coord + step;
			for (int j = 1; j < BINS_CNT-1; ++j) {
				l_off = l_off + step; r_off = r_off + step;
				if (point > l_off && point <= r_off) {
					bins_sizes[j]++;
					break;
				}
			}
		}
	}
}

class tri_comp {
    AXIS axis;
public:
    tri_comp(AXIS axis0=X) : axis(axis0) {}

    bool operator()(triangle & a, triangle & b) {
		AABB box = getTriangleAABB(a);
		float mid_a = (box.max.v[axis] + box.min.v[axis]) / 2;
		box = getTriangleAABB(b);
		float mid_b = (box.max.v[axis] + box.min.v[axis]) / 2;
		return mid_a < mid_b;
    }
};

KDScene* buildKDScene(triangle * triangles, int tris_cnt, Vec * lights, int light_cnt, int depth) {
	// Подготовка

	KDScene * scene = new KDScene();
	scene->nodes = new KDNode[(1 << (depth+1)) - 1];	
	int d = (1 << (depth+1)) - 1;
	if (light_cnt >= 0) {
		scene->lights_cnt = light_cnt;
		scene->lights = new float3[light_cnt];
		for (int i = 0; i < light_cnt; ++i) scene->lights[i] = lights[i];
	}

	if (tris_cnt >= 0) {
		scene->triangles = new triangle[tris_cnt];
		copy_triangles(scene->triangles, triangles, tris_cnt);
		scene->bbox = getSceneAABB(triangles, tris_cnt);
	}
	
	if (depth == 0 || tris_cnt == 0) {
		// root is leaf node
		scene->nodes[0].left = tris_cnt;
		scene->nodes[0].right = 0;
		return scene;
	}

	// Строим дерево

	buildKDNode(scene->nodes, 0, scene->triangles, 0, tris_cnt, depth);

	return scene;
}

void buildKDNode(KDNode * nodes, int node_id, triangle * triangles, int left_tri, int right_tri, int depth) {
	KDNode * node = &nodes[node_id];

	node->left = right_tri;	// Предполагаем листовой узел
	node->right = left_tri;

	int tris_cnt = right_tri - left_tri;

	if (depth <= 0) {
		return;
	}

	if (tris_cnt <= 0) {
		node->left = -1;
		node->right = -1;
		return;
	}

	AABB bbox = getSceneAABB(triangles + left_tri, tris_cnt);	// (!) Локальный бокс может быть меньше глобального по всем осям
	float3 * mid_points = new float3[tris_cnt];	// "центры" треугольников
	for (int i = 0; i < tris_cnt; ++i) { 
		AABB box = getTriangleAABB(triangles[i+left_tri]);
		mid_points[i] = (box.max + box.min) / 2.0;
		if (fabs(mid_points[i].v[0]) < EPSILON)
			int e = 3;
	}

	// Ищем оптимальное разбиение, используя разбиение на интервалы и SAH

	// Берём наибольшую длину бокса в качестве проверяемой оси
	AXIS split_axis = X;	// ось, по которой будем разбивать на интервалы 
	float3 lengths = bbox.max - bbox.min;
	if (lengths.v[Y] > lengths.v[split_axis]) split_axis = Y;
	if (lengths.v[Z] > lengths.v[split_axis]) split_axis = Z;

	// Подсчитываем кол-во треугольников в интервалах
	float step = lengths.v[split_axis] / BINS_CNT;
	int * bins_sizes = new int[BINS_CNT];	// массив количеств треугольников в интервалах
	for (int i = 0; i < BINS_CNT; ++i) bins_sizes[i] = 0;
	float coord = bbox.min.v[split_axis];
	countBins(bins_sizes, BINS_CNT, mid_points, tris_cnt, bbox, split_axis);	// ??

	delete[] mid_points;

	// Считаем SAH для интервалов и выбираем лучшую плоскость
	float min_SAH = tris_cnt * getSurfaceArea(bbox);
	float cur_SAH;
	int split_plane = -1;		// индекс наилучшей плоскости разбиения
	int left_cnt = 0, right_cnt = tris_cnt;
	int min_left_cnt = 0;	// количество треугольников слева от разбивающей плоскости
	float3 left_box = getSizes(bbox), right_box = getSizes(bbox);
	left_box.v[split_axis] = 0;	
	
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

	node->split_axis = split_axis;
	node->split_coord = bbox.min.v[split_axis] + step * (split_plane + 1);

	std::sort(triangles + left_tri, triangles + right_tri, tri_comp(split_axis));
	
	// // Формируем левые и правые узлы

	// Recursively create nodes
	// Рекурсивно формируем узлы дерева
	int right_id = (node_id + 1) * 2, 
		left_id = right_id - 1;
	node->left = left_id; node->right = right_id;
	buildKDNode(nodes, left_id, triangles, left_tri, left_tri + min_left_cnt, depth-1);
	buildKDNode(nodes, right_id, triangles, left_tri + min_left_cnt, right_tri, depth-1);

	return;
}

int hasIntersection(triangle * tris, int left_id, int right_id, const Ray & ray, float & distance, float3 & hit, float3 & bari) {
	float min_dist = INF;
	int id = -1;
	float3 localHit, localBari;
	for (int i = left_id; i < right_id; ++i) {
	#ifndef MOLLER_TRUMBORE_INTERSECT
		//if (node->triangles[i].intersect(ray, hit)) {
		assert(false);
		{
	#else
		if (tris[i].mollerTrumboreIntersect(ray, localHit, localBari)) {
	#endif
			float dist = localHit.distance(ray.o);
			if (dist < min_dist) {
				min_dist = dist;
				id = i;
				hit = localHit;
				bari = localBari;
			}
		}
	}
	distance = min_dist;
	return id;
}

int traceKDScene(const KDScene & scene, const Ray & ray, float3 & hit, float3 & bari, int & edgeHit) {
	edgeHit = 0;
	float tmin, tmax;
	if (!RayAABBIntersect(ray, scene.bbox, tmin, tmax))
		return -1;
	
	struct TraceInfo {
		int node;
		float tmax;
		TraceInfo(int id, float t) : node(id), tmax(t) {};
	};

	#ifdef TREE_VISUALISATION
	// Визуализация граней бокса
	if (RayEdgeIntersect(ray, scene.bbox, tmin) || RayEdgeIntersect(ray, scene.bbox, tmax))
			edgeHit+=2;	// для усиления подсвечиваемости
	#endif

	// Трассируем, начиная с корневого узла
	KDNode * node = &scene.nodes[0];
	std::stack<TraceInfo> s;
	float min_dist = INF;
	float tsplit;
	int res_id = -1;
	while (true) {
		// Спускаемся до листового узла
		while (node->left < node->right) {
			tsplit = raySplitIntersect(ray, node->split_axis, node->split_coord);
			int left_is_far = (node->split_coord > ray.o.v[node->split_axis]) ? 0 : 1;
			int l_id = node->left + left_is_far, 
				r_id = node->left + (1-left_is_far);
			if (tsplit <= tmin) {
				node = &scene.nodes[r_id];
			} else if (tsplit >= tmax) {
				node = &scene.nodes[l_id];
			} else {

				#ifdef TREE_VISUALISATION
				// Визуализация плоскостей разбиения
				if (RayEdgeIntersect(ray, node->split_axis, node->split_coord, tsplit)) 
						edgeHit++;	
				#endif

				s.push(TraceInfo(r_id, tmax));
				tmax = tsplit;
				node = &scene.nodes[l_id];
			}
		}

		// В листовом узле ищем пересечение
		float3 local_hit, local_bari;
		float dist;
		int tri_id = hasIntersection(scene.triangles, node->right, node->left, ray, dist, local_hit, local_bari);
		if (tri_id >= 0) {
			//dist = ray.o.distance(local_hit);
			if (dist < min_dist) {
				min_dist = dist;
				hit = local_hit;
				bari = local_bari;
				res_id = tri_id;
				if (dist <= tmax)
					return res_id;
			}
		} 

		// Выходим, если закончили все проверки, или поднимаемся по стеку
		if (s.empty()) return res_id;
		TraceInfo ti = s.top();
		s.pop();
		node = &scene.nodes[ti.node];
		tmin = tmax;
		tmax = ti.tmax;
		
	}
	return res_id;
}

/*KDNode* buildKDTree(KDNode &root, triangle * triangles, int tri_cnt, int depth) {
	//KDNode root;
	root.tris_cnt = tri_cnt;
	root.left = nullptr;
	root.right = nullptr;
	
	if (tri_cnt == 0) 
		return &root;

	// Calculate bounding box of whole scene
	// Ищем ограничивающий бокс всей сцены и заодно центры треугольников
	float3 * mid_points = new float3[tri_cnt];			// "центры" треугольников
	AABB global_box = getTriangleAABB(triangles[0]);	// бокс сцены
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

	// Search optimal splitting
	// // Ищем оптимальное разбиение

	// Take the longest side of bounding box
	// Берём наибольшую длину бокса в качестве проверяемой оси
	AXIS split_axis = X;	// ось, по которой будем разбивать на интервалы
	float3 lengths = global_box.max - global_box.min;

	if (lengths.v[Y] > lengths.v[split_axis]) split_axis = Y;
	if (lengths.v[Z] > lengths.v[split_axis]) split_axis = Z;

	// Look through possible splittings
	// Перебераем возможные плоскости 

	// Count the number of triangles in the bins
	// Подсчитываем кол-во треугольников в интервалах
	float step = lengths.v[split_axis] / BINS_CNT;
	int * bins_sizes = new int[BINS_CNT];	// массив количеств треугольников в интервалах
	for (int i = 0; i < BINS_CNT; ++i)
		bins_sizes[i] = 0;

	float coord = global_box.min.v[split_axis];
	for (int i = 0; i < tri_cnt; ++i) {
		float point = mid_points[i].v[split_axis];
		if (point <= coord + step) {
			bins_sizes[0]++;
		} else if (point > coord + step * (BINS_CNT-1)) {
			bins_sizes[BINS_CNT-1]++;
		} else {
			float l_off = coord, r_off = coord + step;
			for (int j = 1; j < BINS_CNT-1; ++j) {
				l_off = l_off + step;
				r_off = r_off + step;
				if (point > l_off && point <= r_off) {
					bins_sizes[j]++;
					break;
				}
			}
		}
	}

	// Calculate SAH for bins and choose the best plane
	// Считаем SAH для интервалов и выбираем лучшую плоскость
	float min_SAH = tri_cnt * getSurfaceArea(global_box);
	float cur_SAH;
	int split_plane = -1;		// индекс наилучшей плоскости разбиения
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
	assert(split_plane != -1);

	delete[] bins_sizes;
	
	root.split_axis = split_axis;
	root.split_coord = global_box.min.v[split_axis] + step * (split_plane + 1);

	// Create left and right nodes
	// // Формируем левые и правые узлы

	printf("\n%d: %d / %d", depth, min_left_cnt, tri_cnt - min_left_cnt);

	// Проблема: если границы треугольника выходят за пределы его интервала, то он обрезается
	triangle * l = new triangle[min_left_cnt];
	triangle * r = new triangle[tri_cnt - min_left_cnt];
	int lcnt = 0, rcnt = 0;
	for (int i = 0; i < tri_cnt; ++i) {
		if (mid_points[i].v[split_axis] <= root.split_coord) {
			assert(lcnt < min_left_cnt);
			l[lcnt] = triangles[i];
			++lcnt;
		} else {
			assert(rcnt < tri_cnt - min_left_cnt);
			r[rcnt] = triangles[i];
			++rcnt;
		}
	}
	assert(lcnt == min_left_cnt);

	delete[] mid_points;

	// Recursively create nodes
	// Рекурсивно формируем узлы дерева
	root.left = new KDNode();
	root.left = buildKDTree(*root.left, l, min_left_cnt, depth-1);	
	delete[] l;

	root.right = new KDNode();
	root.right = buildKDTree(*root.right, r, tri_cnt - min_left_cnt, depth-1); 
	delete[] r;

	return &root;
}
*/

/*triangle* traceKDTree(const KDNode &root, const Ray &ray, float3 &pHit, bool &edgeHit) {

	edgeHit = false;

	struct TraceInfo {
		float tmax;
		KDNode * node;
	};

	float tmin, tmax, tsplit;

	// Check the global bounding box
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
		#ifdef TREE_VISUALISATION
		if (RayEdgeIntersect(ray, node->box, tmin) || RayEdgeIntersect(ray, node->box, tmax))
			edgeHit = true;
		#endif
		
		// Leaf node
		// Листовой узел
		while (node->left == nullptr && node->right == nullptr) {
			float3 hit, bari;		
			
			for (int i = 0; i < node->tris_cnt; ++i) {
			#ifndef MOLLER_TRUMBORE_INTERSECT
				if (node->triangles[i].intersect(ray, hit)) {
			#else
				if (node->triangles[i].mollerTrumboreIntersect(ray, hit, bari)) {
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
				if (min_dist < INF)
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
*/