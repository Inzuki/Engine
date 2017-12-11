#ifndef SHADER_H
#define SHADER_H

#include "INCLUDES.h"

#include "Light.h"
#include "Mesh.h"

struct uniform_case {
	std::string uniform_location;
	GLuint uniform;
};

struct variable {
	std::string type, name;
};

struct struct_list {
	std::string struct_name;
	std::vector<variable> variables;
};

class Shader {
	private:
		GLuint program;
		std::vector<uniform_case> uniforms;
		std::vector<struct_list> structs;

	public:
		Shader(){
			program = glCreateProgram();

			if(program == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location in constructor.");
				exit(1);
			}

			addUniform("tex"); // texture - for ambient shader
			addUniform("diffuse"); // texture - for non-forward-ambient shaders
			addUniform("shadow_map"); // texture - for non-forward-ambient shaders

			setUniformi("tex", 0);
			setUniformi("diffuse", 0);
			setUniformi("shadow_map", 1);
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

			std::string uniformName = uniform, temp_name = "0";

			// make sure the uniform doesn't already exist
			bool add = true;
			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(strcmp(uniforms.at(i).uniform_location.c_str(), uniform) == 0){
					add = false;
					break;
				}
			}

			// add the uniform to the list if it hasn't been already
			if(add){
				uniform_case temp_uniform;
				temp_uniform.uniform_location = uniform;
				temp_uniform.uniform = uniformLocation;
				uniforms.push_back(temp_uniform);
			}
		}

		// integers
		void setUniformi(std::string uniformName, int value){
			std::string temp_name = "0";

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(strcmp(uniforms.at(i).uniform_location.c_str(), uniformName.c_str()) == 0)
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && strcmp(temp_name.c_str(), "0") == 0)
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform1i(glGetUniformLocation(program, temp_name.c_str()), value);
		}

		// floats
		void setUniformf(std::string uniformName, float value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(strcmp(uniforms.at(i).uniform_location.c_str(), uniformName.c_str()) == 0)
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && strcmp(temp_name.c_str(), "0") == 0)
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform1f(glGetUniformLocation(program, temp_name.c_str()), value);
		}

		// vec3
		void setUniform(std::string uniformName, glm::vec3 value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(strcmp(uniforms.at(i).uniform_location.c_str(), uniformName.c_str()) == 0)
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && strcmp(temp_name.c_str(), "0") == 0)
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniform3f(glGetUniformLocation(program, temp_name.c_str()), value.x, value.y, value.z);
		}

		// mat4
		void setUniform(std::string uniformName, glm::mat4 &value){
			std::string temp_name;

			for(unsigned int i = 0; i < uniforms.size(); i++){
				if(strcmp(uniforms.at(i).uniform_location.c_str(), uniformName.c_str()) == 0)
					temp_name = uniforms.at(i).uniform_location;
				// check if the uniform location exists in memory
				if(i == uniforms.size() - 1 && strcmp(temp_name.c_str(), "0") == 0)
					printf("ERROR: Couldn't find uniform location \'%s\'.\n", uniformName.c_str());
			}

			glUniformMatrix4fv(glGetUniformLocation(program, temp_name.c_str()), 1, GL_FALSE, value_ptr(value));
		}

		// base light
		void setUniform(std::string uniformName, Light &light){
			setUniform(uniformName + ".color", light.get_color());
			setUniformf(uniformName + ".intensity", light.get_intensity());
		}

		// directional light
		void setUniform(std::string uniformName, DirectionalLight &directional_light){
			setUniform(uniformName + ".base", directional_light.get_light());
			setUniform(uniformName + ".direction", directional_light.get_direction());
		}

		// point light
		void setUniform(std::string uniformName, PointLight &point_light){
			setUniform(uniformName + ".base", point_light.get_light());
			setUniformf(uniformName + ".attenuation.constant", point_light.get_attenuation().get_constant());
			setUniformf(uniformName + ".attenuation.linear", point_light.get_attenuation().get_linear());
			setUniformf(uniformName + ".attenuation.exponent", point_light.get_attenuation().get_exponent());
			setUniform(uniformName + ".position", point_light.get_position());
			setUniformf(uniformName + ".range", point_light.get_range());
		}

		// spot light
		void setUniform(std::string uniformName, SpotLight &spot_light){
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

		// this helps load up the structs from the shader for automatic uniform creation
		void load_structs(const char *file){
			char filepath[128];
			sprintf(filepath, "res/shaders/%s", file);

			std::ifstream shaderFile(filepath);
			if(!shaderFile.good())
				printf("%s not found.\n", file);
			std::string shader_str;

			int cur_line = 0, inside_struct_line = -2;
			std::string cur_struct_name;

			// input data from the shader file to memory
			if(shaderFile.is_open()){
				std::string line = "";
				while(getline(shaderFile, line)){
					if(line.substr(0, 8) == "#include")
						load_structs(line.substr(10, line.size() - 11).c_str());

					// check if the line is a struct.
					// we will add all possible variable types
					// into a vector and check so we can add
					// the appropriate values
					if(line.substr(0, 6) == "struct"){
						inside_struct_line = cur_line;
						char name[64];
						sscanf(line.c_str(), "%*s %s %*c", &name);
						cur_struct_name = name;

						// add the struct type to our list
						struct_list temp_struct;
						temp_struct.struct_name = cur_struct_name;
						structs.push_back(temp_struct);
					// check if we're at the end of the struct
					}else if(line.substr(0, 2) == "};"){
						// if we are finished with a struct, reset all information
						// as to not feed the rest of the code into the struct list
						inside_struct_line = -2;
						cur_struct_name = "";
					// check if we're still processing a struct
					}else if(cur_line == inside_struct_line + 1){
						inside_struct_line = cur_line;

						char type[32], name[64];
						sscanf(line.substr(0, line.size() - 1).c_str(), "%s %s %*c", &type, &name);

						// find the matching struct in the list
						// to add the appropriate variables
						for(unsigned int i = 0; i < structs.size(); i++){
							if(strcmp(cur_struct_name.c_str(), structs.at(i).struct_name.c_str()) == 0){
								variable temp_var;
								temp_var.type = type;
								temp_var.name = name;
								structs.at(i).variables.push_back(temp_var);
							}
						}
					}

					cur_line++;
				}

				shaderFile.close();
			}
		}

		// add the uniforms automatically so we don't have to
		// add them ourselves every time we create one in a shader
		void check(const char *parent, const char *type, const char *uniform){
			bool chk = false;
			for(unsigned int i = 0; i < structs.size(); i++){
				// check if the variable type is a struct.
				// if it is, we use recursion to check if any of the variables in the struct are a struct type.
				if(strcmp(type, structs.at(i).struct_name.c_str()) == 0){
					for(unsigned int j = 0; j < structs.at(i).variables.size(); j++){
						chk = true;
						char parent_arg[128];
						sprintf(parent_arg, "%s%s.", parent, uniform);
						check(parent_arg, structs.at(i).variables.at(j).type.c_str(), structs.at(i).variables.at(j).name.c_str());
					}
				}
				
				// once we reach the last struct with no structs as variable types,
				// we add its variables to the uniform list
				if(i == structs.size() - 1 && !chk){
					char uniform_add[128];
					sprintf(uniform_add, "%s%s", parent, uniform);
					addUniform(uniform_add);
				}
			}
		}
		
		// read a shader file and import the code
		std::string load_shader(const char *file){
			char filepath[128];
			sprintf(filepath, "res/shaders/%s", file);

			std::ifstream shaderFile(filepath);
			if(!shaderFile.good())
				printf("%s not found.\n", file);
			std::string shader_str;

			// input data from the shader file to memory
			if(shaderFile.is_open()){
				std::string line = "";
				while(getline(shaderFile, line)){
					// if the file has a header file, include that code into memory
					if(line.substr(0, 8) == "#include")
						shader_str += load_shader(line.substr(10, line.size() - 11).c_str()) + "\n";
					else {
						// check if the line is a uniform.
						// we'll be checking all information to
						// automatically add uniform variables to the list
						if(line.substr(0, 7) == "uniform"){
							char type[32], uniform[64];
							sscanf(line.substr(0, line.size() - 1).c_str(), "%*s %s %s", &type, &uniform);
						
							// check if the variable type is in our list of structs.
							// this while loop is for when a struct has a variable
							// initialized as another struct type. it can be compounding, ex. SpotLight->PointLight->BaseLight & Attenuation
							//printf("%s\n", uniform);
							check("", type, uniform);

							addUniform(uniform);
						}

						shader_str += line + "\n";
					}
				}

				shaderFile.close();
			}

			return shader_str;
		}

		void addProgram(const char *file, GLuint type){
			GLuint shader = glCreateShader(type);

			if(shader == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location when using shader.");
				exit(1);
			}

			// load structs for automatic uniform insertion
			load_structs(file);

			// compile and implement shader
			std::string shader_code = load_shader(file);
			char const *sourcePtr = shader_code.c_str();
			glShaderSource(shader, 1, &sourcePtr, NULL);
			glCompileShader(shader);
			glAttachShader(program, shader);
		}
};

class ShadowMap : public Shader {
	public:
		ShadowMap(){
			addVertexShader("shadow-map-generator.vs");
			addFragmentShader("shadow-map-generator.fs");
			compileShader();
		}
};

class DebugShader : public Shader {
	public:
		DebugShader(){
			addVertexShader("debug_depth.vs");
			addFragmentShader("debug_depth.fs");
			compileShader();
			
			addUniform("depth_map");
			setUniformi("depth_map", 0);
		}
};

class DepthShader : public Shader {
	public:
		DepthShader(){
			addVertexShader("simple_depth.vs");
			addFragmentShader("simple_depth.fs");
			compileShader();
		}
};

#endif