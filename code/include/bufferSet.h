#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "tools.h"
#include "print.h"

#include "hyperspace.h"

#include <string.h>

namespace stencil{
	

	template <typename Elem, size_t Dimensions, unsigned Copies = 2>
	struct BufferSet: public utils::Printable{

		typedef Elem ElementType;

		static const unsigned copies = Copies;
		static const unsigned dimensions = Dimensions;

		const std::array<size_t, Dimensions> dimension_sizes;
		size_t buffer_size;
		Elem* storage;

// ~~~~~~~~~~~~~~~~~~~~~~~ Canonical  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		BufferSet(const std::array<size_t, Dimensions>& dimension_sizes, const std::vector<Elem>& data)
			: dimension_sizes(dimension_sizes) 
		{ 
			buffer_size = 1;
			for (auto i = 0; i < Dimensions; ++i)  buffer_size *= dimension_sizes[i];

			storage = new Elem[buffer_size*copies];
			for (int i = 0; i < buffer_size; ++i){
				storage[i] = data[i];
			}
		}

		BufferSet(const std::array<size_t, Dimensions>& dimension_sizes, const Elem* data)
			: dimension_sizes(dimension_sizes) 
		{ 
			buffer_size = 1;
			for (auto i = 0; i < Dimensions; ++i)  buffer_size *= dimension_sizes[i];

			storage = new Elem[buffer_size*copies];
			memcpy(storage, data, buffer_size * sizeof(Elem));
		}

        // do not allow copy
		BufferSet(const BufferSet<Elem, Dimensions, Copies>& o) = delete;
		
		BufferSet(BufferSet<Elem, Dimensions, Copies>&& o)
		: dimension_sizes(o.dimension_sizes), buffer_size(o.buffer_size), storage(nullptr)
		{ 
			o.buffer_size = 0;
			std::swap(storage, o.storage);
		}

		~BufferSet()
		{ 
			delete[] storage;
		}
// ~~~~~~~~~~~~~~~~~~~~~~~ getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Elem* getPointer(unsigned copy = 0){
			return storage + buffer_size*copy;
		}

		unsigned getSize(){
			return buffer_size;
		}

		Hyperspace<dimensions> getGlobalHyperspace(){

			std::array<int, dimensions> a;
			std::array<int, dimensions> b;
			std::array<int, dimensions> da;
			std::array<int, dimensions> db;

			for (int i =0; i < dimensions; ++i){
				a[i] = 0;
				b[i] = dimension_sizes[i];
				da[i] = 0;
				db[i] = 0;
			}

			return Hyperspace<dimensions> (a, b, da, db);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ Comparison ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		bool operator == (const BufferSet<Elem, Dimensions, Copies>& o){

			if (buffer_size != o.buffer_size) return false;

			for (int c=0; c < Copies; ++c){
				for ( auto i = 0; i< buffer_size; ++i) {
					if (storage[c*buffer_size + i] != o.storage[c*buffer_size + i]){
						return false;
					}
				}
			}
			return true;
		}

		bool operator != (const BufferSet<Elem, Dimensions, Copies>& o){
			return !(*this == o);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ other tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		std::ostream& printTo(std::ostream& out) const{
			
			out << "Bufferset[";
			for (const auto& i : dimension_sizes) out << i << ",";
			out << "](" << buffer_size<< "elems)x" << copies;

			for (int c=0; c < Copies; ++c){
				out << " {\n";
				for ( auto i = 0; i< buffer_size; ++i) {
					
					if(i!=0){
						auto x = i;
						for (auto d = 0; d< dimensions; ++d) {
							if ((x % dimension_sizes[d]) == 0) {
								std::cout << "\n";
								x = x/dimension_sizes[d];
							}	
							else break;
						}
					}
					out << storage[c*buffer_size + i] << ",";
				}
				out << "\n}";
			}

			return out;
		}
	};

// ~~~~~~~~~~~~~~~~~~~~~~~ external Getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		#define FOR_DIMENSION(N) \
			template<typename E, size_t D, unsigned C>\
			inline typename std::enable_if< is_eq<D, N>::value, E&>::type

		FOR_DIMENSION(1) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(b.buffer_size && "accessing invalidated buffer");
			return b.storage[(b.buffer_size * (t%b.copies) ) + i];
		}

		FOR_DIMENSION(2) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned j, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(b.buffer_size && "accessing invalidated buffer");
			return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])];
		}
		
		FOR_DIMENSION(3) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned j, unsigned k, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(k<b.dimension_sizes[2] && "k out of range");
			return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0])];
		}

		FOR_DIMENSION(4) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned j, unsigned k, unsigned w, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(k<b.dimension_sizes[2] && "k out of range");
			return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0]) + 
										(w*b.dimension_sizes[2]*b.dimension_sizes[1]*b.dimension_sizes[0])];
		}
		
		#undef FOR_DIMENSION


		#define FROM_DIMENSION(N) \
			template<typename E, size_t D, unsigned C>\
			inline typename std::enable_if< is_ge<D, N>::value, const int>::type

		FROM_DIMENSION(1) getW(const BufferSet<E,D,C>& b){
			return b.dimension_sizes[0];
		}
		FROM_DIMENSION(2) getH(const BufferSet<E,D,C>& b){
			return b.dimension_sizes[1];
		}
		FROM_DIMENSION(3) getD(const BufferSet<E,D,C>& b){
			return b.dimension_sizes[2];
		}

		#undef FROM_DIMENSION

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BUFFER 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/**
	 * this buffer hols values of the same position next to each other, instead of
	 * having coy 1 and then 2
	 */
	template <typename Elem, size_t Dimensions>
	struct BufferSet2: public utils::Printable{

		typedef Elem ElementType;
		typedef std::pair<Elem, Elem>  PairType;

		static const unsigned dimensions = Dimensions;

		const std::array<size_t, Dimensions> dimension_sizes;
		size_t buffer_size;
		PairType* storage;

// ~~~~~~~~~~~~~~~~~~~~~~~ Canonical  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		BufferSet2(const std::array<size_t, Dimensions>& dimension_sizes, const std::vector<Elem>& data)
			: dimension_sizes(dimension_sizes) 
		{ 
			buffer_size = 1;
			for (auto i = 0; i < Dimensions; ++i)  buffer_size *= dimension_sizes[i];

			storage = new PairType[buffer_size];
			for (int i = 0; i < buffer_size; ++i){
				storage[i].first = data[i];
			}
		}

		BufferSet2(const std::array<size_t, Dimensions>& dimension_sizes, const Elem* data)
			: dimension_sizes(dimension_sizes) 
		{ 
			buffer_size = 1;
			for (auto i = 0; i < Dimensions; ++i)  buffer_size *= dimension_sizes[i];

			storage = new PairType[buffer_size];
			for (int i = 0; i < buffer_size; ++i){
				storage[i].first = data[i];
			}

		}

        // do not allow copy
		BufferSet2(const BufferSet2<Elem, Dimensions>& o) = delete;
		
		BufferSet2(BufferSet2<Elem, Dimensions>&& o)
		: dimension_sizes(o.dimension_sizes), buffer_size(o.buffer_size), storage(nullptr)
		{ 
			o.buffer_size = 0;
			std::swap(storage, o.storage);
		}

		~BufferSet2()
		{ 
			delete[] storage;
		}
// ~~~~~~~~~~~~~~~~~~~~~~~ getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		unsigned getSize(){
			return buffer_size;
		}

		Hyperspace<dimensions> getGlobalHyperspace(){

			std::array<int, dimensions> a;
			std::array<int, dimensions> b;
			std::array<int, dimensions> da;
			std::array<int, dimensions> db;

			for (int i =0; i < dimensions; ++i){
				a[i] = 0;
				b[i] = dimension_sizes[i];
				da[i] = 0;
				db[i] = 0;
			}

			return Hyperspace<dimensions> (a, b, da, db);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ Comparison ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		bool operator == (const BufferSet2<Elem, Dimensions>& o){

			if (buffer_size != o.buffer_size) return false;

			for ( auto i = 0; i< buffer_size; ++i) {
				if (storage[i].first != o.storage[i].first){
					return false;
				}
				if (storage[i].second != o.storage[i].second){
					return false;
				}
			}
			return true;
		}

		bool operator != (const BufferSet2<Elem, Dimensions>& o){
			return !(*this == o);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ other tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		std::ostream& printTo(std::ostream& out) const{
			
			out << "Bufferset[";
			for (const auto& i : dimension_sizes) out << i << ",";
			out << "](" << buffer_size<< "elems)x2";

			return out;
		}
	};

// ~~~~~~~~~~~~~~~~~~~~~~~ external Getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		#define FOR_DIMENSION(N) \
			template<typename E, size_t D>\
			inline typename std::enable_if< is_eq<D, N>::value, E&>::type

		FOR_DIMENSION(1) getElem(BufferSet2<E,D>& b, unsigned i, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(b.buffer_size && "accessing invalidated buffer");

			if (t%2 == 0) return b.storage[i].first;
			else		  return b.storage[i].second;
		}

		FOR_DIMENSION(2) getElem(BufferSet2<E,D>& b, unsigned i, unsigned j, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(b.buffer_size && "accessing invalidated buffer");
		//	return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])];
			if (t%2 == 0) return b.storage[i+(j*b.dimension_sizes[0])].first;
			else		  return b.storage[i+(j*b.dimension_sizes[0])].second;
		}
		
		FOR_DIMENSION(3) getElem(BufferSet2<E,D>& b, unsigned i, unsigned j, unsigned k, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(k<b.dimension_sizes[2] && "k out of range");
		//	return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0])];
			if (t%2 == 0) return b.storage[i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0])].first;
			else		  return b.storage[i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0])].second;
		}

		FOR_DIMENSION(4) getElem(BufferSet2<E,D>& b, unsigned i, unsigned j, unsigned k, unsigned w, unsigned t){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(k<b.dimension_sizes[2] && "k out of range");
		//	return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0]) + 
		//								(w*b.dimension_sizes[2]*b.dimension_sizes[1]*b.dimension_sizes[0])];
			if (t%2 == 0) return b.storage[i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0]) + 
										(w*b.dimension_sizes[2]*b.dimension_sizes[1]*b.dimension_sizes[0])].first;
			else		  return b.storage[i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0]) + 
										(w*b.dimension_sizes[2]*b.dimension_sizes[1]*b.dimension_sizes[0])].second;
		}
		
		#undef FOR_DIMENSION

		#define FROM_DIMENSION(N) \
			template<typename E, size_t D>\
			inline typename std::enable_if< is_ge<D, N>::value, int>::type

		FROM_DIMENSION(1) getW(const BufferSet2<E,D>& b){
			return b.dimension_sizes[0];
		}
		FROM_DIMENSION(2) getH(const BufferSet2<E,D>& b){
			return b.dimension_sizes[1];
		}
		FROM_DIMENSION(3) getD(const BufferSet2<E,D>& b){
			return b.dimension_sizes[2];
		}

		#undef FROM_DIMENSION



}
