#ifndef FORWARDSPOT_H
#define FORWARDSPOT_H

#include "INCLUDES.h"
#include "Shader.h"

class ForwardSpot : public Shader {
	public:
		ForwardSpot(){
			addFragmentShader("forward-spot.fs");
			addVertexShader("forward-spot.vs");
			compileShader();
		}

		void set_spot_light(SpotLight spot_light){
			setUniform("spot_light", spot_light);
		}
};

#endif