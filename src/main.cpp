#include <iostream>
#include <array>
#include <cmath> //pow
using namespace std;

template <typename T, size_t N>
	class Tensor
	{
		public:
//			Tensor(std::initializer_list<T> coor) : _matrix()
//			{
//				size_t i = 0;
//				for (auto input : coor)
//					_matrix[i++] = input;
//			};
			Tensor(std::initializer_list<T> coor) : _matrix(coor){}
			std::size_t size(){return _matrix.size();}
		private:
			std::array<T, N> _matrix;
	};

int main()
{
//	std::array<double, 2> temp({1,2});
	Tensor<double, 2> test({1,2});
	std::cout << test.size() << std::endl;
	return 0;
};

