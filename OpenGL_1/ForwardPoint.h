#ifndef FORWARDPOINT_H
#define FORWARDPOINT_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardPoint : public Shader {
	private:
		GLuint program;
		std::vector<uniform_case> uniforms;

	public:
		ForwardPoint(){
			program = glCreateProgram();

			if(program == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location in constructor.");
				exit(1);
			}

			addVertexShader("forward-point.vs");
			addFragmentShader("forward-point.fs");
			compileShader();
			
			// add uniforms to shader list
			addUniform("view_mat"); // view matrix
			addUniform("projection_mat"); // projection matrix
			addUniform("transform"); // model transformation
			addUniform("diffuse"); // texture
			// add point light properties
			addUniform("point_light.base.color");
			addUniform("point_light.base.intensity");
			addUniform("point_light.attenuation.constant");
			addUniform("point_light.attenuation.linear");
			addUniform("point_light.attenuation.exponent");
			addUniform("point_light.position");
			addUniform("point_light.range");
			
			setUniformi("diffuse", 0);
		}

		void set_point_light(PointLight point_light){
			setUniform("point_light", point_light);
		}
};

#endif