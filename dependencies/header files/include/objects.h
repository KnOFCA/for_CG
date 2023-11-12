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
#include "texture.h"

#define FOR(a) for(int i=0;i<a;i++)

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
//only standard vertices data (from -1.0 to 1.0) is allowed
struct vstream
{
    float* stream;
    unsigned int len;
    vstream()
        :stream(nullptr), len(0) {};
    vstream(float* array, unsigned int len)
    {
        this->len = len;
        stream = new float[len];
        memcpy(stream, array, len);
    };
    ~vstream()
    {
        delete[] stream;
    }

    unsigned int size()
    {
        return len * 4;
    }
};

/*basic VAO class and method*/
//a basic VAO(include only a set of points' position data from -1.0 to 1.0)
//use vstream class to init
//default:
//  manage method: static_draw, attrib pointer: 3 float in 1 group(x, y, z) and enable
struct basic_VAO
{
    unsigned int VAO_ID, VBO_ID;
    int pointer_number,group_len;
    int offset[17];//offset[16] stores group_len

    basic_VAO()
    {
        glGenBuffers(1, &VAO_ID);
        glGenBuffers(1, &VBO_ID);
        pointer_number = 0;
        offset[16] = 0;
    }
    basic_VAO(vstream vstream)
    {
        glGenBuffers(1, &VAO_ID);
        glGenBuffers(1, &VBO_ID);

        glBindVertexArray(VAO_ID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
        glBufferData(GL_ARRAY_BUFFER, vstream.size(), vstream.stream, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        pointer_number = 1;
        offset[16] = 3;
        offset[0] = 0;

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

        glVertexAttribPointer(pointer_number, len, GL_FLOAT, GL_FALSE, len * sizeof(float), (void*)offset[16]);
        glEnableVertexAttribArray(pointer_number);
        offset[pointer_number] = offset[16];
        pointer_number++;
        offset[16] += len;

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
    void enable_pointer(int index)
    {
        if (index<pointer_number && index>-1)
            glEnableVertexAttribArray(index);
    }
    void disable_pointer(int index)
    {
        if (index<pointer_number && index>-1)
            glDisableVertexAttribArray(index);
    }
    void reload_data(vstream vstream)
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