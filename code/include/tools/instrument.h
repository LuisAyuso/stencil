
#include "stopwatch.h"
#include "hyperspace.h"

#include <sstream>

#ifdef INSTRUMENT

namespace instrument{

	template <unsigned Dims>
	uibk::StopWatch::swTicket instrument_base_case(const stencil::Hyperspace<Dims> &z){

		std::stringstream ss;
		for (int i=0; i < Dims; ++i){
			if (z.da(i) >= z.db(i)) ss << "A";
			else ss << "B";
		}
		ss << "\t" << z.getStep();
		return uibk::StopWatch::start(ss.str()); 
	}

	uibk::StopWatch::swTicket instrument_loop(int x, int t){

		std::stringstream ss;
		ss << "Chunk"; // << x;
		ss << "\t" << t;
		return uibk::StopWatch::start(ss.str()); 
	}
	
 	void instrument_end(uibk::StopWatch::swTicket& t){
		return uibk::StopWatch::stop(t); 
	}

}
	#define BEGIN_INSTRUMENT(Z) \
			auto swt = instrument::instrument_base_case(Z);

	#define LOOP_INSTRUMENT(X,T) \
			auto swt = instrument::instrument_loop(X,T);

	#define END_INSTUMENT \
			instrument::instrument_end(swt);

#else

	#define BEGIN_INSTRUMENT(Z) \
		;

	#define END_INSTUMENT \
		;

#endif
