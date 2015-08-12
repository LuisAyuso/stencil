#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "tools.h"
#include "print.h"

namespace stencil{
	

	template <typename Elem, size_t Dimensions, unsigned Copies = 2>
	struct BufferSet: public utils::Printable{

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

		BufferSet(const BufferSet<Elem, Dimensions, Copies>& o)
		: dimension_sizes(o.dimension_sizes), buffer_size(o.buffer_size)
		{
			storage = new Elem[buffer_size*copies];
			memcpy(storage, o.storage, buffer_size*copies * sizeof(Elem));
		//	for (int i = 0; i < buffer_size*copies; ++i){
		//		storage[i] = o.storage[i];
		//	}
		}
		
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

//		std::vector<Elem> getData(unsigned copy = 0){
//			return std::vector<Elem>(getPointer(copy), buffer_size);
//		}

		unsigned getSize(){
			return buffer_size;
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ Comparison ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		bool operator == (const BufferSet<Elem, Dimensions, Copies>& o){

			if (buffer_size != o.buffer_size) return false;

			for (int i = 0; i< buffer_size; ++i){
				if (storage[i] != o.storage[i]) return false;
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
				out << " {";
				for ( auto i = 0; i< buffer_size; ++i) out << storage[c*buffer_size + i] << ",";
				out << "}";
			}

			return out;
		}
	};

// ~~~~~~~~~~~~~~~~~~~~~~~ external Getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		#define FOR_DIMENSION(N) \
			template<typename E, size_t D, unsigned C>\
			inline typename std::enable_if< is_eq<D, N>::value, E&>::type

		FOR_DIMENSION(1) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned t = 0){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(b.buffer_size && "accessing invalidated buffer");
			return b.storage[(b.buffer_size * (t%b.copies) ) + i];
		}

		FOR_DIMENSION(2) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned j, unsigned t = 0){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(b.buffer_size && "accessing invalidated buffer");
			return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])];
		}
		
		FOR_DIMENSION(3) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned j, unsigned k, unsigned t = 0){
			assert(i<b.dimension_sizes[0] && "i out of range");
			assert(j<b.dimension_sizes[1] && "j out of range");
			assert(k<b.dimension_sizes[2] && "k out of range");
			return b.storage[b.buffer_size*(t%b.copies) + i+(j*b.dimension_sizes[0])+(k*b.dimension_sizes[1]*b.dimension_sizes[0])];
		}
		
		#undef FOR_DIMENSION

		#define FROM_DIMENSION(N) \
			template<typename E, size_t D, unsigned C>\
			inline typename std::enable_if< is_ge<D, N>::value, int>::type

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
}
