#pragma once


#include <array>

#include "hyperspace.h"
#include "bufferSet.h"
#include "tools.h"


namespace stencil{

	typedef Hyperspace<2> Zoid;


	/**
	 *	 VERSION 2, 
	 *		change the order of splits:
	 *			deltaT == 1 Base case
	 *			other case:
	 *			 if fits: space cut
	 *			 other case : time cut
	*/
				
	template <typename DataStorage, typename Kernel, unsigned Dim>
	void recursive_stencil_aux(DataStorage& data, Kernel k, const Zoid& z, int t0, int t1){

		//std::cout << "zoid: " << z <<  " from  " << t0 << " to " << t1 << std::endl;

		auto deltaT = (int)t1-t0;
		assert(t1 >= t0);
		assert(deltaT >= 0);

		// BASE CASE
		if (deltaT == 1){

			for (int t = t0; t < t1; ++t){
				int ia = z.a(0);
				int ib = z.b(0);
				int ja = z.a(1);
				int jb = z.b(1);

				for (int i = ia; i < ib; ++i){
					for (int j = ja; j < jb; ++j){
						k(data, i, j, t);
					}
				}
				ia += z.da(0);
				ib += z.db(0);
				ja += z.da(1);
				jb += z.db(1);
			}
		}
		
		else{

			//std::cout << z << " from " << t0 << " to " << t1 << " in dim: " << Dim << std::endl;
	
			auto a  = z.a(Dim);
			auto b  = z.b(Dim);
			auto da = z.da(Dim);
			auto db = z.db(Dim);
			auto deltaDim = MAX(b - a, (b + db * deltaT) - (a + da * deltaT));
			auto slopeDim = k.getSlope(Dim);

			auto split = a;
			if ((deltaDim >= 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT)){
				split = a + (b - a)/2;
			}

			//std::cout << " a:" << a << " b: "<< b << " da:" << da << " db:" << db << std::endl;
			//std::cout << " deltaDim:" << deltaDim << " deltaT:" << deltaT<< std::endl;
			//std::cout << " split:" << split << std::endl;

			auto zoids = z.split_slopes<Dim> (CutWithSlopes{split, slopeDim.first, slopeDim.second});
		
			//for(auto z: zoids){
			//	std::cout << " -" << z << std::endl;
			//}

			// spatial cut worked, recurse
			if (zoids.size() > 1){
				for (auto subZoid : zoids){
					recursive_stencil_aux<DataStorage, Kernel, (Dim+1)%Kernel::dimensions> (data, k, subZoid, t0, t1);
				}
			}
			// Time cut
			else { // if (deltaT > 1 && deltaX > 0  && deltaY > 0){
				//std::cout << "time cut: " << z << " from " << t0 << " to " << t1 <<std::endl;

				int halfTime = deltaT/2;
				assert(halfTime >= 1);

				//std::cout << " " << z << " from " << t0 << " to " << t0+halfTime <<std::endl;
				recursive_stencil_aux<DataStorage, Kernel, Dim>(data, k, z, t0, t0+halfTime);


				auto upZoid = z;
				upZoid.a(Dim) = a+da*halfTime;
				upZoid.b(Dim) = b+db*halfTime;

				//std::cout << " " << upZoid << " from " << t0+halfTime << " to " << t1 <<std::endl;
				recursive_stencil_aux<DataStorage, Kernel, Dim>(data, k, upZoid, t0+halfTime , t1);

			}
		}
	}


	template <typename DataStorage, typename Kernel>
	void recursive_stencil_2D(DataStorage& data, Kernel k, unsigned t){

		std::array<int, Kernel::dimensions> leftSlopes;
		std::array<int, Kernel::dimensions> rightSlopes;

		int smallerSlope = k.getSlope(0).first;
		for (int d = 0; d < Kernel::dimensions; ++d){
			const auto& x = k.getSlope(d);
			leftSlopes[d] = x.first;
			rightSlopes[d] = x.second;
			if (smallerSlope = MIN(smallerSlope, ABS(x.first))  );
			if (smallerSlope = MIN(smallerSlope, ABS(x.second)) );
		}

		int w = getW(data), h = getH(data);
		int smallerSide = MIN(w,h);

		// notice that the original piramid has perfect vertical sides
		Hyperspace<2> z ({0,0}, {w,h}, {0,0}, {0,0} );

		// well, if the time steps are larger that one full piramid, then we 
		// better cut slices of full computation
		int t0 = 0;
		auto step = MIN(t, (smallerSide/ (2*smallerSlope)) );
		int t1 = step;
		while (t0 < t){

			recursive_stencil_aux<DataStorage, Kernel, 0>(data, k, z, t0, t1);

			t0+=step;
			t1 = MIN(t, t1+step);
		}
	}

} // stencil namespace