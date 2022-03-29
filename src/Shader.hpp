#ifndef SHADER
#define SHADER

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <external/glm/glm.hpp>
#include <external/glm/gtc/matrix_transform.hpp>
#include <external/glm/gtc/type_ptr.hpp>
#include <external/glad/glad.h>
#include <SDL.h>

using namespace std;

class Shader {
public:
	unsigned int programID;

	Shader(string vertexPath, string fragmentPath)
	{
        ifstream vShaderFile;
        ifstream fShaderFile;

        const char* vertexCode;
        const char* fragmentCode;

        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            stringstream vShaderStream, fShaderStream;
            
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            const char* vertexCode = vShaderStream.str().c_str();
            const char* fragmentCode = fShaderStream.str().c_str();
        }
        catch(ifstream::failure e)
        {
            cout << "ERROR: SHADER FILE NOT SUCCESSFULLY READ" << endl;
        }

        unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vShader, 1, &vertexCode, NULL);
		glShaderSource(fShader, 1, &fragmentCode, NULL);

		glCompileShader(vShader);
		glCompileShader(fShader);

		int success;
		char errorLog[512];
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vShader, 512, NULL, errorLog);
			cout << "ERROR: Shader compilation failed" << endl << errorLog << endl;
		}

		programID = glCreateProgram();
		glAttachShader(programID, vShader);
		glAttachShader(programID, fShader);

		glLinkProgram(programID);

		glGetShaderiv(programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programID, 512, NULL, errorLog);
			cout << "ERROR: Shader to program linking failed" << endl << errorLog << endl;
		}

		glDetachShader(programID, vShader);
		glDetachShader(programID, fShader);
		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}

	void Activate()
	{
		glUseProgram(programID);
	}

	// bool uniform
	void SetUniform(const string& name, bool value)
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
	}

	// int and int vector uniforms
	void SetUniform(const string& name, int value)
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
	}

	void SetUniform(const string& name, int value1, int value2)
	{
		glUniform2i(glGetUniformLocation(programID, name.c_str()), value1, value2);
	}

	void SetUniform(const string& name, int value1, int value2, int value3)
	{
		glUniform3i(glGetUniformLocation(programID, name.c_str()), value1, value2, value3);
	}

	void SetUniform(const string& name, int value1, int value2, int value3, int value4)
	{
		glUniform4i(glGetUniformLocation(programID, name.c_str()), value1, value2, value3, value4);
	}

	// float and float vector uniforms
	void SetUniform(const string& name, float value)
	{
		glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
	}

	void SetUniform(const string& name, float value1, float value2)
	{
		glUniform2f(glGetUniformLocation(programID, name.c_str()), value1, value2);
	}

	void SetUniform(const string& name, float value1, float value2, float value3)
	{
		glUniform3f(glGetUniformLocation(programID, name.c_str()), value1, value2, value3);
	}

	void SetUniform(const string& name, float value1, float value2, float value3, float value4)
	{
		glUniform4f(glGetUniformLocation(programID, name.c_str()), value1, value2, value3, value4);
	}

	void SetUniform(const string& name, glm::vec2 vector)
	{
		glUniform2f(glGetUniformLocation(programID, name.c_str()), vector.x, vector.y);
	}

	void SetUniform(const string& name, glm::vec3 vector)
	{
		glUniform3f(glGetUniformLocation(programID, name.c_str()), vector.x, vector.y, vector.z);
	}

	void SetUniform(const string& name, glm::vec4 vector)
	{
		glUniform4f(glGetUniformLocation(programID, name.c_str()), vector.x, vector.y, vector.z, vector.w);
	}

	// matrices
	void SetUniform(const string& name, glm::mat3 matrix)
	{
		glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void SetUniform(const string& name, glm::mat4 matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

};

#endif