#ifndef FORWARDDIRECTIONAL_H
#define FORWARDDIRECTIONAL_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardDirectional : public Shader {
	public:
		ForwardDirectional(){
			addFragmentShader("forward-directional.fs");
			addVertexShader("forward-directional.vs");
			compileShader();
		}

		void set_directional_light(DirectionalLight directional_light){
			setUniform("directional_light", directional_light);
		}
};

#endif