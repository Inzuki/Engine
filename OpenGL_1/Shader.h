#ifndef SHADER_H
#define SHADER_H

#include "INCLUDES.h"

#include "Light.h"
#include "Mesh.h"

struct uniform_case {
	std::string uniform_location;
	GLuint uniform;
};

class Shader {
	private:
		GLuint program;
		std::vector<uniform_case> uniforms;

	public:
		Shader(){
			program = glCreateProgram();

			if(program == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location in constructor.");
				exit(1);
			}

			/*
			addVertexShader("vertex.vs");
			addFragmentShader("fragment.fs");
			compileShader();
			
			// add uniforms to shader list
			addUniform("view_mat"); // view matrix
			addUniform("projection_mat"); // projection matrix
			addUniform("transform"); // model transformation
			addUniform("tex"); // texture
			// add light properties
			addUniform("base_color"); // model color
			addUniform("ambient_light");
			addUniform("light_pos");
			addUniform("light_color");
			// add directional light properties
			addUniform("directional_light.base.color");
			addUniform("directional_light.base.intensity");
			addUniform("directional_light.direction");
			// add specular properties
			addUniform("specular_intensity");
			addUniform("specular_power");
			addUniform("eyePos");
			// add point light properties
			for(int i = 0; i < MAX_POINT_LIGHTS; i++){
				char temp_uni[64];
				sprintf(temp_uni, "point_lights[%i].base.color", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "point_lights[%i].base.intensity", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "point_lights[%i].attenuation.constant", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "point_lights[%i].attenuation.linear", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "point_lights[%i].attenuation.exponent", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "point_lights[%i].position", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "point_lights[%i].range", i);
				addUniform(temp_uni);
			}
			// add spot light properties
			for(int i = 0; i < MAX_SPOT_LIGHTS; i++){
				char temp_uni[64];
				sprintf(temp_uni, "spot_lights[%i].point_light.base.color", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].point_light.base.intensity", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].point_light.attenuation.constant", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].point_light.attenuation.linear", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].point_light.attenuation.exponent", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].point_light.position", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].point_light.range", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].direction", i);
				addUniform(temp_uni);
				sprintf(temp_uni, "spot_lights[%i].cut_off", i);
				addUniform(temp_uni);
			}
			*/
		}

		void bind(){
			glUseProgram(program);
		}

		void update_uniforms(glm::mat4 &transform, Material material, glm::vec3 eyePos){
			setUniform("transform", transform);
			setUniform("eyePos", eyePos);
				
			setUniformf("specular_intensity", material.get_specular_intensity());
			setUniformf("specular_power", material.get_specular_exponent());
			setUniform("base_color", material.get_color());
		}

		void update_matrices(glm::mat4 &view_mat, glm::mat4 &projection_mat){
			bind();
			setUniform("view_mat", view_mat);
			setUniform("projection_mat", projection_mat);
		}

		void addUniform(const char *uniform){
			GLuint uniformLocation = glGetUniformLocation(program, uniform);
			if(uniformLocation == 0xFFFFFF){
				printf("ERROR: Could not find uniform %s", uniform);
				exit(1);
			}

			uniform_case temp_uniform;
			temp_uniform.uniform_location = uniform;
			temp_uniform.uniform = uniformLocation;
			uniforms.push_back(temp_uniform);
		}

		// integers
		void setUniformi(std::string uniformName, int value){
			std::string temp_name = "0";

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform1i(glGetUniformLocation(program, temp_name.c_str()), value);
		}

		// floats
		void setUniformf(std::string uniformName, float value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform1f(glGetUniformLocation(program, temp_name.c_str()), value);
		}

		// vec3
		void setUniform(std::string uniformName, glm::vec3 value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform3f(glGetUniformLocation(program, temp_name.c_str()), value.x, value.y, value.z);
		}

		// mat4
		void setUniform(std::string uniformName, glm::mat4 &value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniformMatrix4fv(glGetUniformLocation(program, temp_name.c_str()), 1, GL_FALSE, value_ptr(value));
		}

		// base light
		void setUniform(std::string uniformName, Light light){
			setUniform(uniformName + ".color", light.get_color());
			setUniformf(uniformName + ".intensity", light.get_intensity());
		}

		// directional light
		void setUniform(std::string uniformName, DirectionalLight directional_light){
			setUniform(uniformName + ".base", directional_light.get_light());
			setUniform(uniformName + ".direction", directional_light.get_direction());
		}

		// point light
		void setUniform(std::string uniformName, PointLight point_light){
			setUniform(uniformName + ".base", point_light.get_light());
			setUniformf(uniformName + ".attenuation.constant", point_light.get_attenuation().get_constant());
			setUniformf(uniformName + ".attenuation.linear", point_light.get_attenuation().get_linear());
			setUniformf(uniformName + ".attenuation.exponent", point_light.get_attenuation().get_exponent());
			setUniform(uniformName + ".position", point_light.get_position());
			setUniformf(uniformName + ".range", point_light.get_range());
		}

		// spot light
		void setUniform(std::string uniformName, SpotLight spot_light){
			setUniform(uniformName + ".point_light", spot_light.get_point_light());
			setUniform(uniformName + ".direction", spot_light.get_direction());
			setUniformf(uniformName + ".cut_off", spot_light.get_cut_off());
		}

		// add appropriate shaders
		void addVertexShader(const char *file){
			addProgram(file, GL_VERTEX_SHADER);
		}
		void addFragmentShader(const char *file){
			addProgram(file, GL_FRAGMENT_SHADER);
		}
		void addGeometryShader(const char *file){
			addProgram(file, GL_GEOMETRY_SHADER);
		}

		void compileShader(){
			glLinkProgram(program);
			glValidateProgram(program);
		}

		void addProgram(const char *file, GLuint type){
			GLuint shader = glCreateShader(type);

			if(shader == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location when using shader.");
				exit(1);
			}

			char filepath[128];
			sprintf(filepath, "res/shaders/%s", file);

			std::ifstream shaderFile(filepath);
			if(!shaderFile.good())
				printf("%s not found.\n", file);
			std::string shader_str;

			// input data from the shader file to memory
			if(shaderFile.is_open()){
				std::string line = "";
				while(getline(shaderFile, line))
					shader_str += line + "\n";

				shaderFile.close();
			}

			// compile and implement shader
			char const *sourcePtr = shader_str.c_str();
			glShaderSource(shader, 1, &sourcePtr, NULL);
			glCompileShader(shader);
			glAttachShader(program, shader);
		}
};

class LightShader {
	private:
		GLuint program;
		std::vector<uniform_case> uniforms;

	public:
		LightShader(){
			program = glCreateProgram();

			if(program == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location in constructor.");
				exit(1);
			}

			addVertexShader("light.vs");
			addFragmentShader("light.fs");
			compileShader();
			
			// add uniforms to shader list
			addUniform("view_mat"); // view matrix
			addUniform("projection_mat"); // projection matrix
			addUniform("transform"); // model transformation
			addUniform("light_color"); // light color
		}

		void bind(){
			glUseProgram(program);
		}

		void addUniform(const char *uniform){
			GLuint uniformLocation = glGetUniformLocation(program, uniform);
			if(uniformLocation == 0xFFFFFF){
				printf("ERROR: Could not find uniform %s", uniform);
				exit(1);
			}

			uniform_case temp_uniform;
			temp_uniform.uniform_location = uniform;
			temp_uniform.uniform = uniformLocation;
			uniforms.push_back(temp_uniform);
		}

		// integers
		void setUniformi(std::string uniformName, int value){
			std::string temp_name = "0";

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform1i(glGetUniformLocation(program, temp_name.c_str()), value);
		}

		// floats
		void setUniformf(std::string uniformName, float value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform1f(glGetUniformLocation(program, temp_name.c_str()), value);
		}

		// vec3
		void setUniform(std::string uniformName, glm::vec3 value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform3f(glGetUniformLocation(program, temp_name.c_str()), value.x, value.y, value.z);
		}

		// mat4
		void setUniform(std::string uniformName, glm::mat4 &value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(!uniformName.compare(uniforms.at(i).uniform_location))
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && !temp_name.compare("0"))
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniformMatrix4fv(glGetUniformLocation(program, temp_name.c_str()), 1, GL_FALSE, value_ptr(value));
		}

		// add appropriate shaders
		void addVertexShader(const char *file){
			addProgram(file, GL_VERTEX_SHADER);
		}
		void addFragmentShader(const char *file){
			addProgram(file, GL_FRAGMENT_SHADER);
		}
		void addGeometryShader(const char *file){
			addProgram(file, GL_GEOMETRY_SHADER);
		}

		void compileShader(){
			glLinkProgram(program);
			glValidateProgram(program);
		}

		void addProgram(const char *file, GLuint type){
			GLuint shader = glCreateShader(type);

			if(shader == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location when using shader.");
				exit(1);
			}

			char filepath[128];
			sprintf(filepath, "res/shaders/%s", file);

			std::ifstream shaderFile(filepath);
			if(!shaderFile.good())
				printf("%s not found.\n", file);
			std::string shader_str;

			// input data from the shader file to memory
			if(shaderFile.is_open()){
				std::string line = "";
				while(getline(shaderFile, line))
					shader_str += line + "\n";

				shaderFile.close();
			}

			// compile and implement shader
			char const *sourcePtr = shader_str.c_str();
			glShaderSource(shader, 1, &sourcePtr, NULL);
			glCompileShader(shader);
			glAttachShader(program, shader);
		}
};

#endif