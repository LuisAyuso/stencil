#pragma once

#include <tuple>


#if defined(_OPENMP) && defined(SEQUENTIAL)
# error omp and sequential? 
#endif

#if defined(CILK) && defined(SEQUENTIAL)
# error CILK and sequential? 
#endif

#if defined(CILK) && defined(_OPENMP)
# error CILK and openmp?
#endif

#if !defined(_OPENMP) && ! defined(CILK)
# define SEQUENTIAL 1
#endif


#ifdef SEQUENTIAL
// ~~~~~~~~~~~~~~~~~~~~~ SEQUENTIAL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	#define PARALLEL_CTX \
		{}

	template<typename F, typename... ARGS>
	inline void SPAWN (F f, ARGS& ... args){
		f(args...);
	}


	#define SYNC \
		{}

	#define P_FOR(it, B, E, S) \
		for (auto it = B; it < E; it += S)


#endif
// ~~~~~~~~~~~~~~~~~~~~~ OPENMP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef _OPENMP

#define STR(x) #x
#define STRINGIFY(x) STR(x) 

	#define PARALLEL_CTX \
		_Pragma( STRINGIFY(  omp parallel )) \
		_Pragma( STRINGIFY(  omp single nowait ))
	

	template<typename F, typename... ARGS>
	inline void SPAWN (F f, ARGS& ... args){

		// Gcc < 4.9 does not like this
		auto wrap = [&] { f(args...); };
	
		#pragma omp task
		wrap();
	}

	#define SYNC \
		_Pragma( STRINGIFY( omp taskwait ) )

	#define P_FOR(it, B, E, S) \
		_Pragma( STRINGIFY(  omp parallel )) \
		_Pragma( STRINGIFY(  omp for )) \
		for (auto it = B; it < E; it += S)  


#endif

// ~~~~~~~~~~~~~~~~~~~~~ CILK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef CILK

	#include <cilk/cilk.h>

	#define PARALLEL_CTX 
	
	template<typename F, typename... ARGS>
	inline void SPAWN (F f, ARGS& ... args){
		// Gcc < 4.9 does not like this
		auto wrap = [&] { f(args...); };
		cilk_spawn wrap();
	}

	#define SYNC \
		cilk_sync;

	#define P_FOR(it, B, E, S)\
		cilk_for (auto it = B; it < E; it += S)  


#endif
