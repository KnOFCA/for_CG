#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "objects.h"

/** @brief class contain all info to draw a Bezier line.\brief
 *
 *  coordinate sys: zero point in the center of window, x right, y up.\n
 *
 *  data range: -1.0f to 1.0f.\n
 * 
 *  @attention (1). To simplify using of draw line function in opengl, line_points array store all middle points twice. \n
 *  @exception
 */
class Bezier
{
	std::vector<float> ss_ctr_points; /// store snapshot control points array
	std::vector<float> BC_arr; /// binomial coefficients array
	std::vector<float> ctr_points; /// control points array
	std::vector<float> line_points; /// calculated points on Bezier line
	int BC_order;
	float precision;
public:
	Bezier() :ss_ctr_points({
			0.5f, 0.5f,
			0.5f, -0.5f,
			-0.5f, -0.5f,
		}),
		ctr_points({
			 0.5f, 0.5f,
			 0.5f, -0.5f,
			-0.5f, -0.5f,
		}), 
		line_points(), BC_arr(), BC_order(2), precision(0.05) {
		set_BC_arr();
		set_line_points();
	}

	/**
	 * @brief set initial control points. Throw exception if param vector can not make up legal point
	 * 
	 * \param init
	 */
	Bezier(const std::vector<float>& init)
	{
		if (init.size() / 2 == 1) {
			throw std::exception("illegal input: check if it can make pairs (x,y).");
		}
		else {
			ss_ctr_points = init;
			ctr_points = init;
			line_points.clear();
			BC_order = init.size() / 2 - 1;
			precision = 0.05;
			set_BC_arr();
			set_line_points();
		}
	}

	/** 
	 *  @brief function to set control point.
	 *
	 *  @exception Throw exception if order out of range.
	 *  @param[in] index: index in the control point array, value from 0 to (control points num - 1).
	 *  @param[in] x & y: where the new pos is.
	 */
	void set_ctr_point(int index, float x, float y);

	/**
	 *  @brief calculate binomial coefficients array.
	 *
	 *  @param[in] order: the binomial coefficient order
	 */
	void set_BC_arr();

	/**
	 * @brief calculate Bezier line. store in line_points array
	 * 
	 */
	void set_line_points();

	/**
	 * @brief set precision. Smaller precision can get better effect but calculate more and need more space.
	 * 
	 * @param[in] new_precision
	 */
	void set_precision(float new_precision);

	/**
	 * @brief get current binomial coefficient order.
	 */
	int get_BC_order();

	/**
	 * @brief get control points num.
	 */
	int get_ctr_num();

	/**
	 * @brief get control points array pointer from pos 0.
	 * 
	 * @return a float pointer refers to 0 pos of control points array.
	 */
	float* get_ctr_pointer();

	/**
	 * @brief get line points num.
	 */
	int get_line_num();

	/**
	 * @brief get line points array pointer from pos 0.
	 *
	 * @return a float pointer refers to 0 pos of line points array.
	 */
	float* get_line_pointer();

	/**
	 * @brief add a control point to the back of control points array, also set BC_order and calculate BC_array.
	 * 
	 * @param[in] x
	 * @param[in] y
	 */
	void add_ctr_point(float x, float y);

	/**
	 * @brief reset control points to initial ones.
	 * 
	 */
	void reset_ctr_points();

};

template<typename T>
	concept is_Bezier = (typeid(T) == typeid(Bezier));

/**
 * @brief Class to contain up to 64 Bezier class.\brief
 * 
 * @attention (1). 
 */
class Bezier_array
{
	Bezier** _pool;
	std::uint64_t _flag;

	int find_usable_flag()
	{
		int pos = -1;
		for (int i = 0; i < 64; i++) {
			if (!check_flag(i)) {
				pos = i;
				break;
			}
		}
		return pos;
	}

	void add(){}

public:
	static const int MAX_SIZE = 64;

	bool check_flag(int pos)
	{
		return _flag & ((std::uint64_t)1 << pos);
	}

	Bezier_array() :_flag(0) {
		_pool = new Bezier * [64](nullptr);
	}
	~Bezier_array()
	{
		for (int i = 0; i < 64; i++) {
				delete _pool[i];
		}
		delete _pool;
	}

	void add(Bezier in);

	template<typename T, typename... others>
	void add(T in, others... arg) {
		if (!(typeid(T) == typeid(Bezier))) {
//TODO: consider use concept to avoid this exception.
			throw std::exception("ERROR: pass a non-Bezier class in Bezier_array's add function.");
		}
		int pos = find_usable_flag();
		if (pos == -1) {
			throw std::exception("ERROR: no space to add more Bezier obj.");
			return;
		}
		_pool[pos] = new Bezier;
		*_pool[pos] = std::move(in);
		_flag += ((std::uint64_t)1 << pos);

		add(arg...);
	}

	Bezier& operator[](int pos) {
		if (_flag & ((unsigned long long)1 << pos)) {
			return *(_pool[pos]);
		}
		else{
			throw std::exception("ERROR: index out of range.");
		}
	}

	void clear();

	void clear(int index);
};
