#pragma once
#include <iostream>
#include <vector>

#include "glad/glad.h"
#include <stb/stb_image.h>

/*Texture class and method*/
//a 2D texture object, include 1 or more images,must load VAO before using by calling init_func or load_VAO
//default: 
//  wrap method: repeat, filter method: linear, mipmap: linear
struct Texture_2D
{
    std::vector<unsigned int>IDs;
    unsigned int ID_now,m_VAO;
    bool is_usable;

    Texture_2D() :IDs(), ID_now(), m_VAO(), is_usable(false) {};

    Texture_2D(unsigned int VAO,const char* imagePath)
    {
        is_usable = true;

        glBindVertexArray(VAO);
        m_VAO = VAO;

        glGenTextures(1, &ID_now);
        IDs.push_back(ID_now);
        glBindTexture(GL_TEXTURE_2D, ID_now);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }

        stbi_image_free(data);
        glBindVertexArray(0);
    }

    void load_VAO(unsigned int VAO) {
        m_VAO = VAO;
        is_usable = true;
    }

    void use() {
        if (is_usable) {
        glBindVertexArray(m_VAO);
        glBindTexture(GL_TEXTURE_2D, ID_now);
        glBindVertexArray(0);
        }
    }

    //add and use
    void add_texture(const char* imagePath)
    {
        if (is_usable) {
            glBindVertexArray(m_VAO);

            glGenTextures(1, &ID_now);
            IDs.push_back(ID_now);
            glBindTexture(GL_TEXTURE_2D, ID_now);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, nrChannels;
            unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }

            stbi_image_free(data);
            glBindVertexArray(0);
        }
    }

    //TODO
    void untie()
    {
        glBindVertexArray(m_VAO);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    bool change_tex(int num)
    {
        if (is_usable) {
            if (-1 < num && num < IDs.size()) {
                ID_now = IDs[num];
                glBindVertexArray(m_VAO);
                glBindTexture(GL_TEXTURE_2D, ID_now);
                glBindVertexArray(0);
                return true;
            }
        }
        return false;
    }

    ~Texture_2D()
    {
        untie();
    }
};

struct Layer
{
    float* pos;
    int len, end;
    std::vector<int> m_pos;
    Texture_2D tex;
    Layer() :pos(new float[48]), len(int()), end(int()), m_pos(), tex() {  };
    void add_new_module(int mleft, int mright, int mtop, int mbo)
    {
        m_pos.push_back(mleft);
        m_pos.push_back(mright);
        m_pos.push_back(mtop);
        m_pos.push_back(mbo);
        if (end == len) {
            float* tmp = new float[len + 48];
            memcpy(tmp, pos, len * sizeof(float));
            delete[]pos;
            pos = tmp;
        }
        float left = 2 * (float)(mleft - (int)SCR_WIDTH / 2) / (float)SCR_WIDTH;
        float right = 2 * (float)(mright - (int)SCR_WIDTH / 2) / (float)SCR_WIDTH;
        float top = 2 * (float)((int)SCR_HEIGHT / 2 - mtop) / (float)SCR_HEIGHT;
        float bo = 2 * (float)((int)SCR_HEIGHT / 2 - mbo) / (float)SCR_HEIGHT;
        //0
        pos[end++] = left;
        pos[end++] = top;
        pos[end++] = 0.0f;
        pos[end++] = 0.0f;
        //1
        pos[end++] = left;
        pos[end++] = bo;
        pos[end++] = 0.0f;
        pos[end++] = 1.0f;
        //2
        pos[end++] = right;
        pos[end++] = top;
        pos[end++] = 1.0f;
        pos[end++] = 0.0f;
        //3
        pos[end++] = left;
        pos[end++] = bo;
        pos[end++] = 0.0f;
        pos[end++] = 1.0f;
        //4
        pos[end++] = right;
        pos[end++] = top;
        pos[end++] = 1.0f;
        pos[end++] = 0.0f;
        //5
        pos[end++] = right;
        pos[end++] = bo;
        pos[end++] = 1.0f;
        pos[end++] = 1.0f;
        len += 24;
    }
    bool check(int x, int y)
    {
        for (int i = 0; i < m_pos.size(); i += 4) {
            if (m_pos[i] <= x && x <= m_pos[i + 1])
                if (m_pos[i + 2] <= y && y <= m_pos[i + 3])
                    return true;
        }
        return false;
    }
    void add_texture(const char* imagePath) {
        tex.add_texture(imagePath);
    }
    void load_VAO(unsigned int VAO) {
        tex.load_VAO(VAO);
    }
    bool change_tex(int num) {
        return tex.change_tex(num);
    }
    void use() {
        tex.use();
    }
};

/*Texture class ends*/
