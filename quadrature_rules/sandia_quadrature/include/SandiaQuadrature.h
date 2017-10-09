#ifndef __SANDIA_QUADRATURE_H
#define __SANDIA_QUADRATURE_H

#include "sandia_rules.hpp" //jacobi_compute

#include "Quadrature.h" //ElementType

//#include <GetPot>
#include "getpot.h"

namespace SandiaQuadrature
{
	using namespace std;

	template <size_t dim>
		Geometry::QuadPointVec<dim> TensorizePoints (const Geometry::QuadPointVec<1>& one_d_p)
		{
			Geometry::QuadPointVec<dim-1> temp = TensorizePoints<dim-1> (one_d_p);

			size_t l1 = temp.Size();
			size_t l2 = one_d_p.Size();
			Geometry::QuadPointVec<dim> result(l1*l2);

			size_t cont = 0;
			for (size_t i = 0; i < l1; ++i)
				for(size_t j = 0; j < l2; ++j)
				{
					auto p = temp[i].Tensor(one_d_p[j]);
					result.Insert(cont++, p);
				}
			return result;
		};

	template <>
		Geometry::QuadPointVec<1> TensorizePoints<1> (const Geometry::QuadPointVec<1>& one_d_p);

	template <size_t dim>
		Geometry::QuadWeightVec TensorizeWeights (const Geometry::QuadWeightVec& one_d_w)
		{
			Geometry::QuadWeightVec temp = TensorizeWeights<dim-1>(one_d_w);
			auto length1 = temp.Size();
			auto length2 = one_d_w.Size();

			Geometry::QuadWeightVec result(length1*length2);

			auto k = 0;
			for (size_t i = 0; i < length1; ++i)
				for(size_t j = 0; j < length2; ++j)
					result[k++] = temp[i]*one_d_w[j];
			return result;
		};
	template <>
		Geometry::QuadWeightVec TensorizeWeights<1> (const Geometry::QuadWeightVec& one_d_w);


	//quadrature rule for an ipercube of dimension dim
	//nodes are gaussian
	//TODO: implement other possible nodes
	template <size_t dim>
		class SandiaQuadratureRule : public Geometry::QuadratureRuleInterface<dim>
		{
			public:
				SandiaQuadratureRule()
				{
					string thisfile = __FILE__;
					string conf_file = thisfile.substr(0, thisfile.find_last_of('/')) + "/sandia_quadrature.conf";
#ifdef DEBUG
					cerr << "SandiaQuadratureRule: opening configuration file " << conf_file << endl;
#endif //DEBUG
					GetPot cl(conf_file.c_str());
					string order = "order";

					this->_order = cl(order.c_str(), 0);
					if (!this->_order)
						throw runtime_error("Unable to read the configuration file in SandiaQuadratureRule");

					Init();
				};

				SandiaQuadratureRule(size_t order)
				{
					this->_order = order;
					Init();
				};

				virtual ~SandiaQuadratureRule(){};

				virtual Geometry::QuadPointVec<dim> GetPoints() override
				{
					return this->_points;
				};

				virtual Geometry::QuadWeightVec GetWeights() override
				{
					return this->_weights;
				};

			protected:
				virtual void Init()
				{
					_n = NodesNumber(this->_order);

					double x[_n];
					double w[_n];
					webbur::jacobi_compute (_n, 0.0, 0.0, x, w);

					//I convert x and w to my type
					Geometry::QuadPointVec<1> one_d_points(this->_n);
					Geometry::QuadWeightVec one_d_weights(this->_n);

					for (size_t i = 0; i < this->_n; ++i)
					{
						one_d_points.Insert(i, x[i]);
						one_d_weights[i] = w[i];
					}

					//I tensorize to get a dim-dimensional quadrature rule
					_points = TensorizePoints<dim>(one_d_points);
					_weights = TensorizeWeights<dim>(one_d_weights);
				};

				virtual size_t NodesNumber(size_t order)
				{
					return ceil((static_cast<double>(order) + 1)/2);
				};

			private:
				//the number of nodes
				size_t _n;
				Geometry::QuadPointVec<dim> _points;
				Geometry::QuadWeightVec _weights;
		};

} //namespace SandiaQuadrature

#endif //__SANDIA_QUADRATURE_H
