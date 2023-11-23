#include "Bezier.h"

void Bezier::set_ctr_point(int index, float x, float y)
{
	if (index<0 || index>BC_order) {
		throw std::exception("Error while set control point's pos: point's order out of range.");
	}
	else
	{
		ctr_points[2 * index] = x;
		ctr_points[2 * index + 1] = y;
	}
    set_line_points();
}

void Bezier::set_BC_arr()
{
    BC_arr.clear();
    int k, j;
    for (k = 0; k <= BC_order; k++)
    {
        BC_arr.push_back(1);
        for (j = BC_order; j >= k + 1; j--)//1*n*(n-1)*(n-2)*...*(k+2)*(k+1)
        {
            BC_arr[k] *= j;
        }
        for (j = BC_order - k; j >= 2; j--)//иой╫ / ( (n-k)*(n-k-1)*...*3*2*1 )
        {
            BC_arr[k] /= j;
        }
    }

}

void Bezier::set_line_points()
{
    line_points.clear();

    line_points.push_back(*(ctr_points.rbegin() + 1));
    line_points.push_back(*ctr_points.rbegin());

    //t in t^mt^{n-m}
    double t = precision;
    int point_num = 1;
    while (t <= 1)
    {
        line_points.push_back(0);
        line_points.push_back(0);

        double p = pow(t, BC_order);
        for (int i = 0; i < BC_order + 1; i++) {
            line_points[4 * point_num - 2] += BC_arr[i] * p * ctr_points[2 * i];
            line_points[4 * point_num - 1] += BC_arr[i] * p * ctr_points[2 * i + 1];
            p = p * (1 - t) / t;
        }
        line_points.push_back(line_points[4 * point_num - 2]);
        line_points.push_back(line_points[4 * point_num - 1]);
        point_num++;
        t += precision;
    }

    line_points.push_back(ctr_points[0]);
    line_points.push_back(ctr_points[1]);
}

void Bezier::set_precision(float new_precision)
{
    precision = new_precision;
}

int Bezier::get_BC_order()
{
	return BC_order;
}

int Bezier::get_ctr_num()
{
    return ctr_points.size() / 2;
}

float* Bezier::get_ctr_pointer()
{
    return ctr_points.data();
}

int Bezier::get_line_num()
{
    return line_points.size() / 2;
}

float* Bezier::get_line_pointer()
{
    return line_points.data();
}

void Bezier::add_ctr_point(float x, float y)
{
    ctr_points.push_back(x);
    ctr_points.push_back(y);
    BC_order++;
    set_BC_arr();
    set_line_points();
}

void Bezier::reset_ctr_points()
{
    ctr_points = ss_ctr_points;
    BC_order = ss_ctr_points.size() / 2;
    set_BC_arr();
    set_line_points();
}

void Bezier_array::add(Bezier in)
{
    int pos = find_usable_flag();
    if (pos == -1) {
        throw std::exception("ERROR: no space to add more Bezier obj.");
        return;
    }
    _pool[pos] = new Bezier;
    *_pool[pos] = std::move(in);
    _flag += ((std::uint64_t)1 << pos);
    return;
}

void Bezier_array::clear()
{
    for (int i = 0; i < 64; i++) {
        delete _pool[i];
        _pool[i] = nullptr;
    }

}

void Bezier_array::clear(int index)
{
    delete _pool[index];
    _pool[index] = nullptr;
}
