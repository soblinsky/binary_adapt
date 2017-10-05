#ifndef __STD_F_ELEMENT_H
#define __STD_F_ELEMENT_H

#include "StdBananaFElement.h"
#include "BinaryTreeHelper.h"
#include "ConcreteFactories.h"


namespace FiniteElements
{
	using namespace std;

	template <size_t dim, BasisType FeType = InvalidFeType>
		class StdFIperCube : public StdBananaFElement<dim, FeType>
		{
			public:
				StdFIperCube()
				{
					_std_geometry = HelperFunctions::Builders<Geometry::StdIperCube<dim>>::BuildSingleton ();

					auto& basis_factory (GenericFactory::BasisFactory<dim>::Instance());
					_basis = move(basis_factory.create(FeType));

					//add here other stuff the constructor is expected to do
#ifdef DESTRUCTOR_ALERT
					cerr << "Costruisco StdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

			public:
				virtual ~StdFIperCube()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
					cerr << "Distruggo StdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual BasisType GetFeType()const
				{
					return FeType;
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point)const
				{
					return this->_basis->Evaluate(ind, point);
				};

				virtual vector<double> EvaluateBasis (size_t degree, const Geometry::Point<dim>& point)const
				{
					return this->_basis->EvaluateBasis(degree, point);
				};

				virtual size_t BasisSize(size_t degree)const
				{
					return this->_basis->ComputeSize(degree);
				};

				virtual Geometry::ElementType GetType()const
				{
					return this->_std_geometry->GetType();
				};
				virtual Geometry::QuadPointVec<dim> GetQuadPoints()const
				{
					return this->_std_geometry->GetQuadPoints();
				};
				virtual Geometry::QuadWeightVec GetQuadWeights()const
				{
					return this->_std_geometry->GetQuadWeights();
				};
				virtual size_t QuadratureOrder()const
				{
					return this->_std_geometry->QuadratureOrder();
				};

			protected:
				unique_ptr<AbstractBasis<dim>> _basis;
				shared_ptr<Geometry::StdIperCube<dim> > _std_geometry;
		};

	template <size_t dim, Geometry::ElementType Type = Geometry::InvalidElementType, BasisType FeType = InvalidFeType>
		class StdFElement : public StdBananaFElement<dim, FeType>
		{
			public:
				StdFElement()
				{
					_std_geometry = HelperFunctions::Builders<Geometry::StdElement<dim, Type> >::BuildSingleton ();
					_std_cube = HelperFunctions::Builders<StdFIperCube<dim, FeType> >::BuildSingleton ();

					auto& std_map_factory(GenericFactory::StdMapFactory<dim>::Instance());
					_ipercube_map = std_map_factory.create(Type);

					//add here other stuff the constructor is expected to do
				};

			public:
				virtual ~StdFElement()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo StdFElement" << endl;
#endif //DESTRUCTOR_ALERT
				};
				virtual BasisType GetFeType()const
				{
					return FeType;
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point)const
				{
					double result = this->_std_cube->EvaluateBasisFunction(ind, MapBackward(point));
//					double temp = sqrt(abs(this->_ipercube_map->EvaluateJacobian(point)));
//					result /= temp;
					return result;
				};

				virtual vector<double> EvaluateBasis (size_t degree, const Geometry::Point<dim>& point)const
				{
					auto result = this->_std_cube->EvaluateBasis(degree, MapBackward(point));
//					double temp = sqrt(abs(this->_ipercube_map->EvaluateJacobian(point)));
//					for (auto iter : result)
//						iter /= temp;

					return result;
				};


				virtual size_t BasisSize(size_t degree)const
				{
					return this->_std_cube->BasisSize(degree);
				};

				virtual Geometry::ElementType GetType()const
				{
					return this->_std_geometry->GetType();
				};
				virtual Geometry::QuadPointVec<dim> GetQuadPoints()const
				{
					return this->_std_geometry->GetQuadPoints();
				};
				virtual Geometry::QuadWeightVec GetQuadWeights()const
				{
					return this->_std_geometry->GetQuadWeights();
				};

				virtual size_t QuadratureOrder()const
				{
					return this->_std_geometry->QuadratureOrder();
				};

/*
				Public methods to use the _ipercube_map attribute
*/
				virtual Geometry::Point<dim> MapBackward(const Geometry::Point<dim>& p)const
				{
					return this->_ipercube_map->ComputeInverse(p);
				};

				virtual Geometry::Point<dim> MapForward(const Geometry::Point<dim>& p)const
				{
					return this->_ipercube_map->Evaluate(p);
				};

				virtual double Jacobian(const Geometry::Point<dim>& p)const
				{
					return this->_ipercube_map->EvaluateJacobian(p);
				};

			protected:
				shared_ptr<StdFIperCube<dim, FeType>>			_std_cube;
				shared_ptr<Geometry::StdElement<dim, Type>>	_std_geometry;
				unique_ptr<Geometry::Map<dim>>					_ipercube_map;
		};

} //namespace FiniteElements

#endif //__STD_F_ELEMENT_H
