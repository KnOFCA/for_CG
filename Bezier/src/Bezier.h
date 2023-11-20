#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "objects.h"

/** @brief class contain all info to draw a Bezier line.\n
 *
 *  coordinate sys: zero point in the center of window, x right, y up
 *
 *  data range: -1.0f to 1.0f
 *
 *  member:
 * 
 */
class Bezier
{
	std::vector<float> init_ctr_points, BC_arr, ctr_points;
	int BC_order, current_ctr_point;
	float precision, speed;
public:
	Bezier() :init_ctr_points({
			0.5f, 0.5f,
			0.5f, -0.5f,
			-0.5f, -0.5f,
		}),
		ctr_points({
			 0.5f, 0.5f,
			 0.5f, -0.5f,
			-0.5f, -0.5f,
		}), 
		BC_arr(), BC_order(2), current_ctr_point(1), precision(0.05), speed(0.01) {}

	/** @brief function to set control point.
	 *
	 *  @param[in] order: order in the control point array, value from 0 to (control points num - 1).
	 *  @param[in] x & y: where the new pos is.
	 */
	void set_ctr_point(int order, float x, float y);


};