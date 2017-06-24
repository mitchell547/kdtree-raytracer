#include "Include/Geometry/AABB.h"
#include "Include/Geometry/Triangle.hpp"
#include <algorithm>
#include <stack>
#include "assert.h"

#pragma once

const int BINS_CNT = 32;

struct KDNode {
	//AABB box;	// для теста
	AXIS split_axis;	// ось, по которой разбивается узел 
	float split_coord;	// координата плоскости разбиения
	int left;	// Если left < right, то является внутернним узлом
	int right;	// Если left >= right, то является листовым узлом
	int * tri_ids;
	int id_cnt;
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

int traceKDScene(const KDScene & scene, int trace_node, const Ray & ray, float3 & hit, float3 & bari, int & edgeHit);

void buildKDNode(KDNode * nodes, int node_id, triangle * triangles, int left_tri, int right_tri, int depth);

void buildKDNode(KDNode * nodes, int node_id, triangle * triangles, int * ids, int id_cnt, int depth, float prevSAH);



bool isLeafNode(const KDNode & node) {
	return node.left >= node.right;
}

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

		scene->nodes[0].tri_ids = new int[tris_cnt];
		for (int i = 0; i < tris_cnt; ++i) scene->nodes[0].tri_ids[i] = i;
		scene->nodes[0].id_cnt = tris_cnt;
		return scene;
	}

	// Строим дерево
	int * ids = new int[tris_cnt];
	for (int i = 0; i < tris_cnt; ++i) ids[i] = i;
	//buildKDNode(scene->nodes, 0, scene->triangles, 0, tris_cnt, depth);
	buildKDNode(scene->nodes, 0, scene->triangles, ids, tris_cnt, depth, INF);
	delete[] ids;
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

	}
	//node->box = bbox;	// для теста
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

void buildKDNode(KDNode * nodes, int node_id, triangle * triangles, int * ids, int id_cnt, int depth, float prevSAH) {
	KDNode * node = &nodes[node_id];
	node->id_cnt = 0;
	node->left = 0;	// Предполагаем листовой узел
	node->right = -1;

	//int tris_cnt = right_tri - left_tri;

	if (depth <= 0) {
		//std::copy(ids, ids+id_cnt, node->tri_ids);
		node->tri_ids = new int[id_cnt];
		for (int i = 0; i < id_cnt; ++i) node->tri_ids[i] = ids[i];
		node->id_cnt = id_cnt;
		//printf("\n(%d) %d : %d [%d]", depth, node_id, id_cnt, ids[0]);
		return;
	}

	if (id_cnt <= 0) {
		node->left = -1;
		node->right = -1;
		//printf("\n(%d) %d : %d [%d]", depth, node_id, id_cnt, ids[0]);
		return;
	}

	AABB bbox = getTriangleAABB(triangles[ids[0]]);
	float3 * mid_points = new float3[id_cnt];	// "центры" треугольников
	for (int i = 0; i < id_cnt; ++i) { 
		AABB box = getTriangleAABB(triangles[ids[i]]);
		addAABB(bbox, box);
		mid_points[i] = (box.max + box.min) / 2.0;
	}
	//node->box = bbox;	// для теста
	// Ищем оптимальное разбиение, используя разбиение на интервалы и SAH

	// Берём наибольшую длину бокса в качестве проверяемой оси
	AXIS split_axis = X;	// ось, по которой будем разбивать на интервалы 
	float3 lengths = bbox.max - bbox.min;
	KDNode * parent = &nodes[(node_id + 1) / 2 - 1];
	if (parent->split_axis == X) split_axis = Y;
	if (parent->split_axis != Y && lengths.v[Y] > lengths.v[split_axis]) split_axis = Y;
	if (parent->split_axis != Z && lengths.v[Z] > lengths.v[split_axis]) split_axis = Z;
	
	//if (lengths.v[Y] > lengths.v[split_axis]) split_axis = Y;
	//if (lengths.v[Z] > lengths.v[split_axis]) split_axis = Z;

	// Подсчитываем кол-во треугольников в интервалах
	float step = lengths.v[split_axis] / BINS_CNT;
	int * bins_sizes = new int[BINS_CNT];	// массив количеств треугольников в интервалах
	for (int i = 0; i < BINS_CNT; ++i) bins_sizes[i] = 0;
	float coord = bbox.min.v[split_axis];
	countBins(bins_sizes, BINS_CNT, mid_points, id_cnt, bbox, split_axis);	// ??

	// Считаем SAH для интервалов и выбираем лучшую плоскость
	//float min_SAH = id_cnt * getSurfaceArea(bbox);
	float min_SAH = prevSAH;
	float cur_SAH;
	int split_plane = -1;		// индекс наилучшей плоскости разбиения
	int left_cnt = 0, right_cnt = id_cnt;
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

	//KDNode * par_parent = &nodes[(node_id + 1) / 4 - 1];
	
	if (min_SAH + 10000.0 >= prevSAH || 
		(split_axis == parent->split_axis && node->split_coord == parent->split_coord) || 
		((node_id % 2) == 0 && node_id > 2 && split_axis == nodes[node_id-1].split_axis 
			&& node->split_coord == nodes[node_id-1].split_coord)) {
		node->left = 0;	
		node->right = -1;
		node->tri_ids = new int[id_cnt];
		for (int i = 0; i < id_cnt; ++i) node->tri_ids[i] = ids[i];
		node->id_cnt = id_cnt;
		return;
	}
	//printf("\n%d : %d %d %f %f", node_id, split_axis, split_plane, min_SAH, node->split_coord);
	
	//printf("\n\n--- %d ---\n", node_id);
	//for (int i = 0; i < id_cnt; ++i)
	//	printf("%d, ", ids[i]);

	// // Формируем левые и правые узлы

	// Recursively create nodes
	// Рекурсивно формируем узлы дерева
	int * l_ids = new int[id_cnt];
	int * r_ids = new int[id_cnt];
	int lcnt = 0, rcnt = 0;
	for (int i = 0; i < id_cnt; ++i) {
		AABB box = getTriangleAABB(triangles[ids[i]]);
		//if (mid_points[i].v[split_axis] <= node->split_coord) {
		if (box.min.v[split_axis] <= node->split_coord) {
			l_ids[lcnt] = ids[i];
			lcnt++;
		}
		//if (mid_points[i].v[split_axis] >= node->split_coord) {
		if (box.max.v[split_axis] >= node->split_coord) {
			r_ids[rcnt] = ids[i];
			rcnt++;
		}
	}
	delete[] mid_points;

	int right_id = (node_id + 1) * 2, 
		left_id = right_id - 1;
	node->left = left_id; node->right = right_id;
	buildKDNode(nodes, left_id, triangles, l_ids, lcnt, depth-1, min_SAH);
	delete[] l_ids;

	buildKDNode(nodes, right_id, triangles, r_ids, rcnt, depth-1, min_SAH);
	delete[] r_ids;
	//printf("\n(%d) %d : %d [%d]", depth, node_id, id_cnt, ids[0]);
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

int hasIntersection(triangle * tris, int * ids, int id_cnt, const Ray & ray, float & distance, float3 & hit, float3 & bari) {
	float min_dist = INF;
	int id = -1;
	float3 localHit, localBari;
	for (int i = 0; i < id_cnt; ++i) {
	#ifndef MOLLER_TRUMBORE_INTERSECT
		//if (node->triangles[i].intersect(ray, hit)) {
		assert(false);
		{
	#else
		if (tris[ids[i]].mollerTrumboreIntersect(ray, localHit, localBari)) {
	#endif
			float dist = localHit.distance(ray.o);
			if (dist < min_dist) {
				min_dist = dist;
				id = ids[i];
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
			//int left_is_far = 0;
			int left_is_far = (node->split_coord > ray.o.v[node->split_axis]) ? 0 : 1;
			//int left_is_far = (node->split_coord < (ray.o + ray.d.norm() * tmin).v[node->split_axis]) ? 0 : 1;
			//int left_is_far = (ray.d.v[node->split_axis] < 0) ? 1 : 0;
			//if (node->split_coord < ray.o.v[node->split_axis] != ray.d.v[node->split_axis] < 0)
			//	int q = 1;
			int l_id = node->left + left_is_far, 
				r_id = node->left + (1-left_is_far);
			// Проблема с длинными треугольниками:
			// если они вылазят за границы своего узла, то пересечения не будет найдено
			// (решается с помощью массива индексов)
			if (tsplit >= tmax || tsplit < 0) {
				node = &scene.nodes[l_id];
			} else if (tsplit <= tmin) {
				node = &scene.nodes[r_id];
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
		//int tri_id = hasIntersection(scene.triangles, node->right, node->left, ray, dist, local_hit, local_bari);
		int tri_id = hasIntersection(scene.triangles, node->tri_ids, node->id_cnt, ray, dist, local_hit, local_bari);
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


int traceKDScene(const KDScene & scene, int trace_node, const Ray & ray, float3 & hit, float3 & bari, int & edgeHit) {
	edgeHit = 0;
	float tmin, tmax;

	KDNode * node = &scene.nodes[trace_node];

	float dist;
	if (isLeafNode(*node))
		return hasIntersection(scene.triangles, node->tri_ids, node->id_cnt, ray, dist, hit, bari);

	// (!)
	/*if (!RayAABBIntersect(ray, node->box, tmin, tmax))
		return -1;
		*/
	int res_id = -1;
	
	
	float3 localhit, localbari;
	int id;
	res_id = traceKDScene(scene, node->left, ray, hit, bari, edgeHit);
	id = traceKDScene(scene, node->right, ray, localhit, localbari, edgeHit);
	if (id >= 0 && ray.o.distance(localhit) < ray.o.distance(hit) || res_id < 0) {
		res_id = id;
		hit = localhit;
		bari = localbari;
	}
	return res_id;
}
