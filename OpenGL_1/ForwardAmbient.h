#ifndef FORWARDAMBIENT_H
#define FORWARDAMBIENT_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardAmbient : public Shader {
	public:
		ForwardAmbient(){
			addFragmentShader("forward-ambient.fs");
			addVertexShader("forward-ambient.vs");
			compileShader();
		}
};

#endif