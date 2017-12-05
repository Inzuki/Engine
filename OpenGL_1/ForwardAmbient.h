#ifndef FORWARDAMBIENT_H
#define FORWARDAMBIENT_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardAmbient : public Shader {
	private:
		GLuint program;
		std::vector<uniform_case> uniforms;

	public:
		ForwardAmbient(){
			program = glCreateProgram();

			if(program == 0){
				printf("ERROR: Shader creation failed. Could not find valid memory location in constructor.");
				exit(1);
			}

			addVertexShader("forward-ambient.vs");
			addFragmentShader("forward-ambient.fs");
			compileShader();
			
			// add uniforms to shader list
			addUniform("view_mat"); // view matrix
			addUniform("projection_mat"); // projection matrix
			addUniform("transform"); // model transformation
			addUniform("tex"); // texture
			addUniform("ambientIntensity");
			
			setUniformi("tex", 0);
		}
};

#endif