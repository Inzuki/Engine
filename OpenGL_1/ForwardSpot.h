#ifndef FORWARDSPOT_H
#define FORWARDSPOT_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardSpot : public Shader {
	private:
		GLuint program;
		std::vector<uniform_case> uniforms;

	public:
		ForwardSpot(){
			program = glCreateProgram();

			if(program == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location in constructor.");
				exit(1);
			}

			addVertexShader("forward-spot.vs");
			addFragmentShader("forward-spot.fs");
			compileShader();
			
			// add uniforms to shader list
			addUniform("view_mat"); // view matrix
			addUniform("projection_mat"); // projection matrix
			addUniform("transform"); // model transformation
			addUniform("diffuse"); // texture
			// add point light properties
			addUniform("spot_light.point_light.base.color");
			addUniform("spot_light.point_light.base.intensity");
			addUniform("spot_light.point_light.attenuation.constant");
			addUniform("spot_light.point_light.attenuation.linear");
			addUniform("spot_light.point_light.attenuation.exponent");
			addUniform("spot_light.point_light.position");
			addUniform("spot_light.point_light.range");
			addUniform("spot_light.direction");
			addUniform("spot_light.cutoff");
			
			setUniformi("diffuse", 0);
		}

		void set_spot_light(SpotLight spot_light){
			setUniform("spot_light", spot_light);
		}
};

#endif