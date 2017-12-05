#include "INCLUDES.h"

class Time {
	private:
		sf::Clock clk;
		double delta;

	public:
		static const long SECOND = 1000L;

		Time(){
			clk.restart();
			delta = 0.0;
		}

		long getTime(){
			return clk.getElapsedTime().asMilliseconds();
		}

		double getDelta(){
			return delta;
		}

		void setDelta(double delta){
			this->delta = delta;
		}
};