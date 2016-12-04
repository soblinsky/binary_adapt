#ifndef __TENSOR__H
#define __TENSOR__H

#include <array>
#include <cmath> //pow
#include <stdexcept> //invalid_argument
#include <string> //string
#include <memory> //unique_ptr
#include <array> //array
#include <vector> //vector
#include <iterator> //array::iterator

namespace Geometry
{
	using namespace std;


	template <typename T, size_t DIM>
		class Tensor
		{
			public:

				Tensor() : _tData(nullptr){};

				//constructor from array of size, memory allocated and default initialized
				Tensor (array<size_t, DIM> lengths);

				//constructor from vectors to be tensorized
				template <class Container>
					Tensor (array<Container, DIM> vectors);

				//constructor from sizes, memory allocated and default initialized
				template <typename... Args>
					Tensor(size_t dim1, Args... lengths);

				//constructor from vectors, tensor computed by tensor product of the inputs
				template <class Container, typename... Args>
					Tensor (Container v1, Args... vecs);

				//copy constructor
				Tensor& operator = (Tensor&& t)
				{
					if (&t != this)
					{
						_tData = move (t._tData);
						_arrayLength = t._arrayLength;
						_lengths = t._lengths;
						t._arrayLength = 0;
						t._lengths.fill(0);
					}
					return *this;
				};


				//call operator, returns tensor element at the input indexes position
				//say (i,j,k) indexes of an element in a 3D-Tensor T of _lengths (N,M,L),
				//then T(i,j,k) = _tData[ (N*M)*k + N*j + i ]
				template <typename... Args>
					T& operator() (size_t first_i, Args... indexes);

				array<size_t, DIM> measures()	{return _lengths;};

				size_t size() 						{return _arrayLength;};


			private:
				//compute the position at coordinates indexes in the storage array _tData
				template <typename... Args>
					T& find (T const *const start, size_t first_ind, Args... indexes);

					T& find (T const *const start, size_t last_ind);

				template <typename... Args>
					size_t prodDim (size_t iteration);

				//compute the product of the input dimensions
				template <typename... Args>
					size_t countData(size_t dim1, Args... args);

					size_t countData(size_t dim1);

				template <typename... Args>
					size_t countData(vector<T> v1, Args... args);

					size_t countData(vector<T> v1);

				//initialize _lengths and _arrayLength with the input parameters
				template <typename... Args>
					void	fillLengths(size_t dim1, Args... lengths);

				template <class Container, typename... Args>
					void	fillLengths(typename array<size_t, DIM>::iterator cell, Container v1, Args... vecs);
//					void	fillLengths(size_t* cell, Container v1, Args... vecs);
				template <class Container>
					void	fillLengths(typename array<size_t, DIM>::iterator cell, Container v);
//					void	fillLengths(size_t* cell, Container v);

			private:
				//initialize _tData with the tensor product of input vectors 
				template <class Container, typename... Args>
					void tensorize(size_t step, size_t* dim, Container firstVec, Args... args);
				template <class Container>
					void tensorize(size_t step, size_t* dim, Container lastVec);

			private:
				//matrics stored by columns, parallelepiped by successive matrices and so on
				unique_ptr<T[]> _tData;

				//array of tensor dimensions
				array<size_t, DIM> _lengths;

				//length of _tdata, is the product structure of _lengths elements
				size_t _arrayLength;
		};


/*
	METHOD DEFINITIONS
*/
//	template <typename T, size_t DIM>
//		template <class Container>
//			Tensor<T, DIM>::Tensor (array<Container, DIM> vectors)
//			{
//				for (auto iter : vectors)
//			};

	template <typename T, size_t DIM>
		Tensor<T, DIM>::Tensor (array<size_t, DIM> lengths) :	_lengths(lengths), _arrayLength(1)
		{
			for (auto dim : _lengths)
				_arrayLength *= dim;

			_tData(new T[_arrayLength]);
		};

	template <typename T, size_t DIM>
		template <typename... Args>
			Tensor<T, DIM>::Tensor(size_t dim1, Args... lengths)
			{
				if (sizeof... (lengths) != DIM - 1)
				{
					string ss("Lengths do not fit Tensor dimension");
					throw invalid_argument(ss);
				}
				else
				{
					fillLengths(_lengths.begin(), dim1, lengths...);
					_tData = move(unique_ptr<T[]> (new T[_arrayLength] ));
				}
			};

	template <typename T, size_t DIM>
		template <class Container, typename... Args>
			Tensor<T, DIM>::Tensor (Container v1, Args... vecs)
			{
				if (sizeof... (vecs) != DIM - 1)
				{
					string ss("Lengths do not fit Tensor dimension");
					throw invalid_argument(ss);
				}
				else
				{
					fillLengths(_lengths.begin(), v1, vecs...);
					_tData = move(unique_ptr<T[]> (new T[_arrayLength] ));
					size_t step(1);
					tensorize(step, &_lengths[0], v1, vecs...);
				}
			};

	template <typename T, size_t DIM>
		template <typename... Args>
			T& Tensor<T, DIM>::operator() (size_t first_i, Args... indexes)
			{				
				if (sizeof... (indexes) != DIM - 1)
				{
					string ss("Number of indexes exceed tensor dimension!");
					throw invalid_argument(ss);
				}
				else
				{
					return find(_tData.get(), first_i, indexes...);						
				}
			};

	///////////////////////////////////////////////////////////////////////////
	////////////////////////////////MAYBE LATER, REAL USEFULNESS TO BE EVALUATED
	//			template <size_t D1>
	//				Tensor<T, DIM + D1> tensorProd (const Tensor<T, D1>& factor)
	//				{
	//					array<size_t, DIM + D1> prodMeasures;
	//					size_t count = 0;
	//					for (auto val : _lengths)
	//						prodMeasures[count++] = val;

	//					for (auto val : factor.measures())
	//						prodMeasures[count++] = val;

	//					Tensor<T, DIM + D1> prod (prodMeasures);
	//					
	//				}
	///////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////
	////////////////////////////////TO BE OPTIMIZED, THE prodDim CODE IS REDUNDANT

				//compute the position at coordinates indexes in the storage array _tData
	template <typename T, size_t DIM>
		template <typename... Args>
			T& Tensor<T, DIM>::find (T const *const start, size_t first_ind, Args... indexes)
			{
				size_t dim = prodDim(sizeof... (indexes) + 1);
				return find(start + first_ind*dim, indexes...);
			};

	template <typename T, size_t DIM>		
		T& Tensor<T, DIM>::find (T const *const start, size_t last_ind)
		{
			size_t dim = prodDim(1);
			return start[last_ind*dim];
		};

	template <typename T, size_t DIM>
		template <typename... Args>
			size_t Tensor<T, DIM>::prodDim (size_t iteration)
			{
				size_t tot = 1;
				for (size_t count = DIM - iteration; count; --count)
					tot *= _lengths[count - 1];
				return tot;
			};
	///////////////////////////////////////////////////////////////////////////////

	template <typename T, size_t DIM>
		template <typename... Args>
			size_t Tensor<T, DIM>::countData(size_t dim1, Args... args)
			{
				return dim1*countData(args...);
			};

	template <typename T, size_t DIM>
		size_t Tensor<T, DIM>::countData(size_t dim1)
		{
			return dim1;
		};

	template <typename T, size_t DIM>
		template <typename... Args>
			size_t Tensor<T, DIM>::countData(vector<T> v1, Args... args)
			{
				return v1.size()*countData(args...);
			};

	template <typename T, size_t DIM>
		size_t Tensor<T, DIM>::countData(vector<T> v1)
		{
			return v1.size();
		};


	template <typename T, size_t DIM>
		template <typename... Args>
			void	Tensor<T, DIM>::fillLengths(size_t dim1, Args... lengths)
			{
				_lengths.fill(dim1, lengths...);
				_arrayLength = 1;
				for (auto dim : _lengths)
					_arrayLength *= dim;
			};

	template <typename T, size_t DIM>
		template <class Container, typename... Args>
			void	Tensor<T, DIM>::fillLengths (typename array<size_t, DIM>::iterator cell, Container v, Args... vecs)
//			void	Tensor<T, DIM>::fillLengths (size_t* cell, Container v, Args... vecs)
			{
				auto dim1 = v.size();
				*cell = dim1;
				fillLengths(++cell, vecs...);
				_arrayLength *= dim1;
			};

	template <typename T, size_t DIM>
		template <class Container>
		void	Tensor<T, DIM>::fillLengths (typename array<size_t, DIM>::iterator cell, Container v)
//		void	Tensor<T, DIM>::fillLengths (size_t* cell, Container v)
		{
			auto dim = v.size();
			*cell = dim;
			_arrayLength = dim;
		};


	//////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////TO BE OPTIMIZED
	template <typename T, size_t DIM>
		template <class V>
			void Tensor<T, DIM>::tensorize(size_t step, size_t* dim, V lastVec)
			{
				if (step * (*dim) != _arrayLength)
				{
					string ss("Dimensions not exhausted: something went wrong!");
					throw  length_error(ss);
				}
				size_t i(0);
				for (size_t iter = 0; iter < _arrayLength / step; ++iter)
				{
					for (auto val : lastVec)
					{
						for (size_t j = 0; j < step; ++j)
							_tData[j*i] = _tData[j*i].tensor(val);
						++i;		
					}
				}
			};

	template <typename T, size_t DIM>
		template <class V, typename... Args>
			void Tensor<T, DIM>::tensorize(size_t step, size_t* dim, V firstVec, Args... args)
			{
				step *= (*dim);
				tensorize(step, ++dim, args...);
				size_t i(0);	
				for (size_t iter = 0; iter < _arrayLength / step; ++iter)
				{	
					for (auto val : firstVec)
					{
						for (size_t j = 0; j < step; ++j)
							_tData[j*i] *= val;
						++i;
					}
				}
			};
	///////////////////////////////////////////////////////////////////////////////


}
#endif //__TENSOR__H













































/*
	//class to manage square tensors of N dimensions equal to L
	// T has to have operator* defined
	template <typename T, size_t L, size_t N = 1>
		class Tensor
		{
			public:
			//compute tensorial product vec * vec * vec... N times
				Tensor( array<T, L> vec ) : _matrix()
				{
						vector<size_t> indexes();
						for ( size_t j = 0; j < N; ++j)
							indexes.push_back(0);
		
						size_t i(0);

						while (indexes[0] < L)
						{
							auto last_index = indexes.rbegin();
							T val(vec(*last_index));
							for (auto index = last_index + 1; index != indexes.rend(); ++index)
							{
								val *= vec(*index);
								if ( *(index - 1) % L == 0)
									(*index)++;
							}
							_matrix[i++] = val;
							(*last_index)++;
						}
				}
			
				Tensor(Vector<L> vec) : Tensor(vec.get()){};

				T& operator[] (array<size_t,N> indexes)
				{
					size_t index(0), exp(0);
					for (auto iter : indexes)
						index += iter * pow(L, exp++);
					return _matrix[index];
				}
				
			private:
				array<T, pow(L, N)> _matrix;
		};
*/

//template <typename T, size_t L>
//	Tensor<T, L, 2> tensorize (Tensor<T, L> w1, Tensor<T, L> w2)
//	{
//		Tensor<T, L, 2> result();
//		size_t i(0);
//		for (auto iter1 : w1._matrix)
//			for (auto iter2 : w2._matrix)
//				result._matrix[i++] = iter1 * iter2;
//		return result;
//	}




////template <typename T, size_t... args>
////	class Tensor
////	{
////		template <size_t dim1, size_t... args>
////			size_t countData( size_t dim1, size_t... args)
////		{
////			return dim1*countData(args...);
////		}

////		Tensor()
////		{
////			pData = unique_ptr<T> (new T(countData(args...)) );
////		}

////		template <vector<T> lastVec, typename... Args>
////			unique_ptr<T> tensorize(unique_ptr<T>& start, Args... args, vector<T> lastVec)
////			{
////				T* oneLess = tensorize(start, args...);
////				size_t i(0);
////				size_t step = countData(args...);
////				for (auto val : lastVec)
////				{
////					for (j = 0; j < step; ++j)
////						start[j*i] = val * oneLess[j];
////					++i;
////				}
////				return move(start);
////			}

////		template <vector<T> firstRow, typename... Args>
////			Tensor (vector<T> firstRow, Args... args) : Tensor<firstRow.size(), args.size()...>()
////			{
////				_tData = tensorize(_tData, firstRow, args...);
////			}

////		private:
////			 unique_ptr<T> _tData;

////		T& operator() (array<size_t, sizeof...(args)> indexes)
////		{
////			size_t tot = 1;
////			for (auto ind : indexes)
////				tot *= ind;
////			return *(_tData + tot);
////		}

////		template <size_t i, size_t... indexes>
////				size_t prod (size_t i, size_t... indexes)
////				{
////					return i*prod(indexes...);
////				}

////		template <size_t... indexes>
////			T& operator() (size_t... indexes)
////			{
////				if (sizeof... (indexes) != sizeof... (args))
////				{
////					string ss("Number of indexes exceed tensor dimension!");
////					throw invalid_argument(ss);
////				}
////				else
////				{
////					return *(_tData + prod(indexes...));
////				}
////			}

////		size_t size()
////		{
////			return sizeof... (args);
////		}

////			template <typename... Args>
////				void tensorize(Args... args, vector<T> lastVec)
//			template <typename V, typename... Args>
//				void tensorize(Args... args, V lastVec)
//				{
//					tensorize(args...);
//					size_t i(0);
//					size_t step = countData(args...);
//					for (auto val : lastVec)
//					{
//						for (size_t j = 0; j < step; ++j)
//							_tData[j*i] = val * _tData[j];
//						++i;
//					}
//					return;
//				}

//			template <typename V>
//				void tensorize(V firstVec)
////				void tensorize(vector<T> firstVec)
//				{
//					size_t i(0);
//					size_t step = firstVec.size();
//					for (auto val : firstVec)
//					{
//						for (size_t j = 0; j < step; ++j)
//							_tData[j*i] = val;
//						++i;
//					}
//					return;
//				}
	//			template <typename... Args>
//				void tensorize(Args... args, vector<T> lastVec)

////		private:
////			unique_ptr<T> _tData;
////	}

