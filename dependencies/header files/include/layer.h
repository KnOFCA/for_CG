#pragma once
#include <iostream>
#include <vector>
#include <source_location>

#include "glad/glad.h"
#include <stb/stb_image.h>

#include "global.h"
//#include "thread pool.h"

/*Texture class and method*/
//a 2D texture object, include 1 or more images,must load VAO before using by calling init_func or load_VAO().
//catch format_error to locate function.
//default:
//  wrap method: repeat, filter method: linear, mipmap: linear
class Texture_2D
{
	unsigned int ID_now, m_VAO, image_type;
	bool is_usable;
	std::vector<unsigned int>IDs;
	std::vector<unsigned char*> tex_arr;

public:
	Texture_2D() :IDs(), ID_now(), m_VAO(), is_usable(false), tex_arr() {};

	Texture_2D(unsigned int VAO, const char* imagePath):ID_now()
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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_FLOAT, data);
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
		else throw std::format_error(std::format("function %s must call after load VAO.", std::source_location::current().function_name()));
	}

	//add and use this texture
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
				glTexImage2D(GL_TEXTURE_2D, 0, image_type, width, height, 0, image_type, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			stbi_image_free(data);
			glBindVertexArray(0);
		}
		else throw std::format_error(std::format("function %s must call after load VAO.",std::source_location::current().function_name()));
	}

	void untie()
	{
		glBindVertexArray(m_VAO);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}

	void change_tex(int num)
	{
		if (is_usable) {
			if (-1 < num && num < IDs.size()) {
				ID_now = IDs[num];
				glBindVertexArray(m_VAO);
				glBindTexture(GL_TEXTURE_2D, ID_now);
				glBindVertexArray(0);
			}
		}
		else throw std::format_error(std::format("function %s must call after load VAO.", std::source_location::current().function_name()));
	}

	~Texture_2D()
	{
		untie();
	}
	void set_image_type(unsigned int type) {
		image_type = type;
	}
};

class Layer
{
	float* pos;
	int len, end;
	Texture_2D tex;
	std::vector<int> m_pos;

public:
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
	void change_tex(int num) {
		tex.change_tex(num);
	}
	void use() {
		tex.use();
	}
	int get_len() {
		return len;
	}
	float* get_pos_array() {
		return pos;
	}
	void set_image_type(unsigned int type) {
		tex.set_image_type(type);
	}
};

/*Texture class ends*/
