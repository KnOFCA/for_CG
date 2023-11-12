#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#include "glad/glad.h"
#include <stb/stb_image.h>

#include "camera.h"
#include "shader.h"
#include "layer.h"

#define FOR(a) for(int i=0;i<a;i++)

//namespace Mouse
//{
//    int pos_x, pos_y;
//};

class player
{
private:
	int pos_xL, pos_xR, pos_yT, pos_yB;

public:

};


class obstacles
{
private:
	int pos_xL, pos_xR, pos_yT, pos_yB;

public:

};

/*time_count class and method*/
//use to count time
struct time_count
{
	std::chrono::steady_clock::time_point start, end;
	std::chrono::duration<float> period;
	time_count()
	{
		start = std::chrono::high_resolution_clock::now();
		period = std::chrono::duration<float>();
	}
	~time_count()
	{
		end = std::chrono::high_resolution_clock::now();
		period = end - start;
		std::cout << period.count() << "s\n";
	}
};
/*time_count class ends*/

/*vstream class and method*/
//a class wrapping vertices data
//only basic type and standard vertices data (from -1.0 to 1.0) is allowed
template <typename T>
struct vstream
{
    T* stream;
    unsigned int len;
    vstream()
        :stream(nullptr), len(0) {};

    template<unsigned int N>
    vstream(T array[N])
    {
        this->len = N;
        stream = new T[N];
        memcpy(stream, array, N);
    };

    vstream(vstream<T>& other)
    {
        len = other.len;
        stream = new T[len];
        memcpy(stream, other.stream, len);
    }

    ~vstream()
    {
        delete[] stream;
    }

    unsigned int size()
    {
        return len * sizeof(T);
    }
};

/*basic VAO class and method*/
//only basic type is allowed
//use vstream class to init
template <typename T>
struct basic_VAO
{
    unsigned int VAO_ID, VBO_ID;
    std::vector<int> pointer_pos;

    basic_VAO() :pointer_pos()
    {
        glGenBuffers(1, &VAO_ID);
        glGenBuffers(1, &VBO_ID);
    }
    basic_VAO(vstream<T>& vstream)
    {
        glGenBuffers(1, &VAO_ID);
        glGenBuffers(1, &VBO_ID);

        glBindVertexArray(VAO_ID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
        glBufferData(GL_ARRAY_BUFFER, vstream.size(), vstream.stream, GL_STATIC_DRAW);

        //todo: AttribPointer
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };

    void use()
    {
        glBindVertexArray(VAO_ID);
    }
    //default enable when added
    void add_attrib_pointer(int len)
    {
        glBindVertexArray(VAO_ID);

        //Todo: AttribPointer

        glBindVertexArray(0);
    }
    //void change_attrib_pointer(int index, int new_length)
    //{
    //    if (new_length > -1)
    //    {
    //        glBindVertexArray(VAO_ID);

    //        if (index < pointer_number && index > 0)
    //        {
    //            int delta = new_length - offset[index] + offset[index - 1];
    //            for (int i = pointer_number - 1; i > index; i--)
    //            {
    //                offset[i] += delta;
    //                glVertexAttribPointer(i, l, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)offset[16]);

    //            }
    //        }

    //        glBindVertexArray(0);
    //    }
    //}
    //Todo: enable and disable pointer
    void enable_pointer(int index)
    {

    }
    void disable_pointer(int index)
    {

    }
    void reload_data(vstream<T>& vstream)
    {
        glBindVertexArray(VAO_ID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
        glBufferData(GL_ARRAY_BUFFER, vstream.size(), vstream.stream, GL_STATIC_DRAW);
    }
    void draw(unsigned int type)
    {

    }
};

/*camera class and method*/
struct camera
{

};

class FPS_counter
{
    std::chrono::steady_clock::time_point start;
    std::chrono::duration<float> period;
    int frame_num, FPS;
public:
    FPS_counter() :start(), frame_num(), FPS(-1) {};

    void count()
    {
        frame_num++;
        auto now = std::chrono::high_resolution_clock::now();
        period = now - start;
        if (period.count() > 1.0f)
        {
            start = now;
            FPS = frame_num / period.count();
            frame_num = 0;
        }
    }
    int get_FPS() {
        if (period.count() > 1.0f)
            return FPS;
        else return -1;
    }
};