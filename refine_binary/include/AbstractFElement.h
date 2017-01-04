#ifndef __ABSTRACT_F_ELEMENT_H
#define __ABSTRACT_F_ELEMENT_H

#include "ConcreteFactories.h"
#include "AbstractSpace.h"

#include <memory> //std::shared_ptr, std::unique_ptr
#include <utility> //std::move

using namespace std;
using namespace Geometry;

namespace FiniteElements
{
	template <size_t dim, BasisType FeType = InvalidFeType>
		class	AbstractFElement : public AbstractElement<dim>, public AbstractSpace_Interface<dim>
		{
			public:
/*
				_map and _ref_felement initialization depends on the pure virtual method type() return, so it cannot be done during object construction; so this operation is done by init method, which call is compulsory before using the object
*/
				AbstractFElement() : _p_level(0), _initialized(false), _ref_felement(nullptr){};

				virtual ~AbstractFElement()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AbstractFElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual void Init()
				{
					if(!(this->_initialized))
					{
	#ifdef MYDEBUG
						cout << "Provo a inizializzare AbstractFElement" << endl;
	#endif //MYDEBUG
					//TODO: create helper function to combine factory instantiation and call to the object creation
						auto& felem_factory(GenericFactory::StdFElementFactory <dim, FeType> ::Instance());
	#ifdef MYDEBUG
						cout << "cerco in StdFElementFactory<" << dim << ", " << FeType <<"> con chiave " << GetType() << endl;
	#endif //MYDEBUG
						this->_ref_felement = felem_factory.create(GetType());
	#ifdef MYDEBUG
						cout << "refFElement creato" << endl;
	#endif //MYDEBUG
						auto& map_factory(GenericFactory::AffineMapFactory<dim>::Instance());
						this->_map = move(map_factory.create(GetType()));
	#ifdef MYDEBUG
						cout << "mappa creata" << endl;
	#endif //MYDEBUG
						this->_map -> Init(GetNodes());
						this->_initialized = true;
	#ifdef MYDEBUG
						cout << "AbstractFElement inizializzato" << endl;
	#endif //MYDEBUG
					}
				};

/*
				methods depending on geometry, cannot be defined at this level
*/
				virtual NodesVector<dim> GetNodes() = 0;
				virtual ElementType GetType()const = 0;

				virtual BasisType GetFeType()const override
				{
					return FeType;
				};


				//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
				virtual QuadPointVec<dim> GetQuadPoints()const
				{
					CheckInitialization();

					QuadPointVec<dim> points = _ref_felement->GetQuadPoints();

					for (auto& p : points)
						p = _map->Evaluate(p);

					return points;
				};

				virtual QuadWeightVec GetQuadWeights()const
				{
					CheckInitialization();

					QuadWeightVec weights = _ref_felement->GetQuadWeights();
/*
					I'm taking advantage from the fact that _map is affine, so it has an evaluateJacobian() method
					not dependent on the point of evaluation
*/
					for (ptrdiff_t i(0); i < weights.size(); ++i)
						weights[i] = weights[i] * _map->Jacobian();

					return weights;
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Point<dim>& point)const
				{
					CheckInitialization();
					return 	_ref_felement->EvaluateBasisFunction(ind, _map->ComputeInverse(point))
								/								//to normalize the basis I divide by the constant jacobian^1/2 of the map
								sqrt(_map->Jacobian())
								;
				};

				//TODO: optimize storing already evaluated points
				virtual vector<double> EvaluateBasis (const Point<dim>& point)const
				{
					CheckInitialization();
					auto result = this->_ref_felement->EvaluateBasis(this->_p_level,_map->ComputeInverse(point));

					/* to normalize the basis I divide by the constant jacobian^1/2 of the map */
					for (auto& iter : result)
						iter /= sqrt(_map->Jacobian());

					return result;
				};

				virtual size_t	BasisSize(size_t degree)const
				{
					CheckInitialization();
					return _ref_felement->BasisSize(degree);
				};

				virtual size_t BasisSize()const
				{
					CheckInitialization();
					return _ref_felement->BasisSize(this->_p_level);
				};

				virtual size_t PLevel() const
				{
					return this->_p_level;
				};
				virtual void PLevel(size_t new_p)
				{
					this->_p_level = new_p;
				};

			protected:

				AffineMap<dim>& UseMap()const
				{
					CheckInitialization();
					return *(this->_map);
				};

				virtual void CheckInitialization()const
				{
					if (!_initialized)
						throw runtime_error("Trying to use uninitialized element");
				};

			protected:
/*
				It is how much is incremented the underlying basis degree w.r.t the default basis degree (which is 0 for Legendre, that is approximation by a constant)
				By default is 0; 
*/
				size_t _p_level;
/*
				A flag which controls if the init method has been called
*/
				bool _initialized;
/*
				The reference standard element associated to the type of the object
				It is meant to be a singleton, so it's stored as a shared_ptr
*/
				shared_ptr<StdBananaFElement<dim, FeType>> _ref_felement;

			private:
/*
				The affine map through which I can pass from the reference element to the current domain of the object
*/
				unique_ptr<AffineMap<dim>>	_map;
		};

};//namespace FiniteElements

#endif //__ABSTRACT_F_ELEMENT_H