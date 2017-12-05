#ifndef FORWARDDIRECTIONAL_H
#define FORWARDDIRECTIONAL_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardDirectional : public Shader {
	private:
		GLuint program;
		std::vector<uniform_case> uniforms;

	public:
		ForwardDirectional(){
			program = glCreateProgram();

			if(program == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location in constructor.");
				exit(1);
			}

			addVertexShader("forward-directional.vs");
			addFragmentShader("forward-directional.fs");
			compileShader();
			
			// add uniforms to shader list
			addUniform("view_mat"); // view matrix
			addUniform("projection_mat"); // projection matrix
			addUniform("transform"); // model transformation
			addUniform("diffuse"); // texture
			// add directional light properties
			addUniform("directional_light.base.color");
			addUniform("directional_light.base.intensity");
			addUniform("directional_light.direction");
			// add specular properties
			addUniform("specular_intensity");
			addUniform("specular_power");
			addUniform("eyePos");
			
			setUniformi("diffuse", 0);
		}

		void set_directional_light(DirectionalLight directional_light){
			setUniform("directional_light", directional_light);
		}
};

#endif