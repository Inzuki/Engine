#ifndef FORWARDPOINT_H
#define FORWARDPOINT_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardPoint : public Shader {
	public:
		ForwardPoint(){
			addFragmentShader("forward-point.fs");
			addVertexShader("forward-point.vs");
			compileShader();
		}

		void set_point_light(PointLight point_light){
			setUniform("point_light", point_light);
		}
};

#endif