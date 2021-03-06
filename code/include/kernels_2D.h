
#pragma once

#include <cmath>

#include "kernel.h"
#include "bufferSet.h"



namespace stencil{
namespace example_kernels{


		template< typename DataStorage> 
		struct Copy_k : public Kernel<DataStorage, 2, Copy_k<DataStorage>>{

			static void withBonduaries (DataStorage& data, unsigned i, unsigned j, unsigned t){
				auto pix = getElem(data, i, j, 0);
				getElem(data, i, j, 1) = pix;
			}
			static void withoutBonduaries (DataStorage& data, int i, int j, int t){
				auto pix = getElem(data, i, j, 0);
				getElem(data, i, j, 1) = pix;
			}

			static const unsigned int neighbours = 1;
		};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename DataStorage> 
		struct Life_k : public Kernel<DataStorage, 2, Life_k<DataStorage>>{

			static void withBonduaries (DataStorage& data, int i, int j, int t) {

				unsigned sum = 0;
				for (unsigned x = MAX(0, ((int)i)-1); x < MIN(getW(data), i+1); ++x){
					for (unsigned y = MAX(0, ((int)j)-1); y < MIN(j+1, getH(data)); ++y){	

						sum += (getElem(data, x, y, t) > 125)? 1 : 0;
					}
				}
				
				if (getElem(data, i, j, t) < 128) {
					getElem(data, i, j, t+1) = sum == 3? 255: 0;
				}
				else{
					getElem(data, i, j, t+1) = sum > 3? 0: (sum<2)? 0: 255;
				}
			}

			static void withoutBonduaries (DataStorage& data, int i, int j, int t) {

				unsigned sum = 0;
				for (unsigned x = i-1; x < i+1; ++x){
					for (unsigned y = j-1; y < j+1; ++y){	

						sum += (getElem(data, x, y, t) > 125)? 1 : 0;
					}
				}
				
				if (getElem(data, i, j, t) < 128) {
					getElem(data, i, j, t+1) = sum == 3? 255: 0;
				}
				else{
					getElem(data, i, j, t+1) = sum > 3? 0: (sum<2)? 0: 255;
				}
			}


			static const unsigned int neighbours = 1;
		};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename DataStorage> 
		struct Color_k : public Kernel<DataStorage, 2, Color_k<DataStorage>>{

			static void withBonduaries (DataStorage& data, int i, int j, int t) {
				getElem(data, i, j, t+1) = t%std::numeric_limits<typename DataStorage::ElementType>::max();
			}
			static void withoutBonduaries (DataStorage& data, int i, int j, int t) {
				getElem(data, i, j, t+1) = t%std::numeric_limits<typename DataStorage::ElementType>::max();
			}

			static const unsigned int neighbours = 1;
		};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename DataStorage> 
		struct Blur3_k : public Kernel<DataStorage, 2, Blur3_k<DataStorage>>{

			static const float Kcoeff[3][3];

			static void withBonduaries (DataStorage& data, int i, int j, int t) {

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				double sum = 0.0;

				for (unsigned x = MAX(0, ((int)i)-1); x <= MIN(getW(data)-1, i+1); ++x){
					for (unsigned y = MAX(0, ((int)j)-1); y <= MIN(j+1, getH(data)-1); ++y){	
						
						int ki =  x-i+1;
						int kj =  y-j+1;

						auto e = getElem(data, x, y, t);
						sum += e * Kcoeff[ki][kj];
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			static void withoutBonduaries (DataStorage& data, int i, int j, int t) {

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				double sum = 0.0;

				for (unsigned x = i-1; x <= i+1; ++x){
					for (unsigned y = j-1; y <= j+1; ++y){	
						
						int ki =  x-i+1;
						int kj =  y-j+1;

						auto e = getElem(data, x, y, t);
						sum += e * Kcoeff[ki][kj];
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			static const unsigned int neighbours = 1;
		};

		template< typename DataStorage> 
		const float Blur3_k<DataStorage>::Kcoeff[3][3] = {{0.01, 0.08, 0.01}, {0.08, 0.64, 0.08}, {0.01, 0.08, 0.01}};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename DataStorage> 
		struct Blur5_k : public Kernel<DataStorage, 2, Blur5_k<DataStorage>>{

			static const float Kcoeff[5][5];

			static void withBonduaries (DataStorage& data, int i, int j, int t) {

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				double sum = 0.0;

                for (unsigned x = MAX(0, ((int)i)-1, ((int)i)-2); x <= MIN(getW(data)-1, i+1, i+2); ++x){
					for (unsigned y = MAX(0, ((int)j)-1, ((int)j)-2); y <= MIN(j+2, j+1, getH(data)-1); ++y){	
						
						int ki =  x-i+1;
						int kj =  y-j+1;

						auto e = getElem(data, x, y, t);
						sum += e * Kcoeff[ki][kj];
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			static void withoutBonduaries (DataStorage& data, int i, int j, int t) {

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				double sum = 0.0;

				for (unsigned x = i-2; x <= i+2; ++x){
					for (unsigned y = j-2; y <= j+2; ++y){	
						
						int ki =  x-i+1;
						int kj =  y-j+1;

						auto e = getElem(data, x, y, t);
						sum += e * Kcoeff[ki][kj];
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			static const unsigned int neighbours = 2;
		};

		template< typename DataStorage> 
		const float Blur5_k<DataStorage>::Kcoeff[5][5] =
									{{0.01, 0.02, 0.04, 0.02, 0.01},
									 {0.02, 0.04, 0.08, 0.04, 0.02},
									 {0.04, 0.08, 0.16, 0.08, 0.04},
									 {0.02, 0.04, 0.08, 0.04, 0.02},
									 {0.01, 0.02, 0.04, 0.02, 0.01}};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//		/**
//		 * a generic  gaussian blur kernel:
//		 * http://stackoverflow.com/questions/8204645/implementing-gaussian-blur-how-to-calculate-convolution-matrix-kernel
//		 */
//		template< typename DataStorage, unsigned Size> 
//		struct BlurN_k : public Kernel<DataStorage, 2, BlurN_k<DataStorage, Size>>{
//
//			static const unsigned int neighbours = (Size-1)>>1;
//
//			static float Kcoeff[Size][Size];
//
//			static double gaussian (double x, double mu, double sigma)  {
//			  return exp( -(((x-mu)/(sigma))*((x-mu)/(sigma)))/2.0 );
//			}
//
//			static constexpr int initialize () {
//				assert(Size%2 == 1);
//
//
//				int halfSize = Size/2;
//			
//				// determine sigma
//				double sigma = Size/2.0;
//
//				// fill values
//				double sum = 0;
//				for (int row = 0; row < Size; row++){
//					for (int col = 0; col < Size; col++) {
//
//						int i = row<=halfSize? row: Size-(row+1);
//						int j = col<=halfSize? col: Size-(col+1);
//
//						Kcoeff[row][col] = gaussian(i, Size, sigma) * gaussian(j, Size, sigma);
//						sum += Kcoeff[row][col];
//					}
//				}
//
//				//		  // normalize
//				for (int row = 0; row < Size; row++){
//					for (int col = 0; col < Size; col++){
//						Kcoeff[row][col] /= sum;
//					}
//				}
//
//			}
//			static const int init = initialize();
//
//			static void withBonduaries (DataStorage& data, unsigned i, unsigned j, unsigned t) {
//
//				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
//				double sum = 0.0;
//
//				assert(i < getW(data));
//				assert(j < getH(data));
//
//				auto xMin = i;
//				for (int v = 1; v <= neighbours; ++v) xMin = MAX(0, ((int)i)-v);
//				auto xMax = i;
//				for (int v = 1; v <= neighbours; ++v) xMax = MIN(getW(data)-1, i+v);
//		
//				auto yMin = j;
//				for (int v = 1; v <= neighbours; ++v) yMin = MAX(0, ((int)j)-v);
//				auto yMax = j;
//				for (int v = 1; v <= neighbours; ++v) yMax = MIN(getH(data)-1, j+v);
//
//				for (unsigned x = xMin; x <= xMax; ++x){
//					for (unsigned y = yMin; y <= yMax; ++y){	
//						
//							// this is quite not right, this offset might produce weird values 
//							// on bonduaries, but results are uniform
//						int ki =  x-i+neighbours;
//						int kj =  y-j+neighbours;
//
//						auto e = getElem(data, x, y, t);
//						sum += e * Kcoeff[ki][kj];
//					}
//				}
//				
//				getElem(data, i, j, t+1) = sum;
//			}
//
//		};
//
//		template< typename DataStorage, unsigned Size> 
//		float BlurN_k<DataStorage, Size>::Kcoeff[Size][Size];

}// example_kernels
}// stencil
