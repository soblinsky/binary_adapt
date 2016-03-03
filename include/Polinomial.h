#ifndef __POLY__HH
#define __POLY__HH

#include <vector>
#include <exception> //runtime_error
#include <algorithm> //max

#include "Tensorize.h"
#include "Point.h"

using namespace Geometry;
using namespace std;


template <size_t DIM>
	class Polinomial
	{
		public:
 
			Polinomial() : _coeff(), _degree(0), _degreeUpdated(true)
			{
				_oneDFactors.fill(Polinomial<1>(0));
			};

			Polinomial(size_t degree) : _degree(degree), _degreeUpdated(true)
			{
				_coeff.resize(degree);
				size_t i(0);
				for (auto pol : _coeff)
					pol = Polinomial<DIM-1> (i++);
			};

			Polinomial<DIM> (const Polinomial<DIM>& p)
			{
				*this = p;
			}

			Polinomial<DIM>& operator = (const Polinomial<DIM>& p)
			{
				if (&p != this)
				{
					_coeff = p._coeff;
					for (size_t i = 0; i < DIM; ++i)
						_oneDFactors[i] = p._oneDFactors[i];
					_degree = p._degree;
					_degreeUpdated = p._degreeUpdated;
				}
				return *this;
			}

			//polinomial product

			Polinomial<DIM> operator * (const Polinomial<DIM>& p)
			{
				size_t thisDegree = degree();
				size_t pDegree = p.degree();

				Polinomial<DIM> result (thisDegree + pDegree);

				if (p._coeff.empty() && _coeff.empty())
					for (size_t ind = 0; ind < DIM; ++ind)
						result._oneDFactors[ind] = _oneDFactors[ind] * p._oneDFactors[ind];
				else
				{
					_coeff.empty() ? convertStorage() : p.convertStorage();
					for (size_t i = 0; i < thisDegree; ++i)
						for (size_t j = 0; j < pDegree; ++j)
							result._coeff[i + j] += _coeff[i] * p._coeff[j];
				}

				result._degreeUpdated = false;
				return result;
			};


			Polinomial<DIM> operator + (const Polinomial<DIM>& add)
			{
				size_t addDegree = add.degree();
				size_t thisDegree = degree();
				size_t resDegree = max(addDegree, thisDegree);

				Polinomial<DIM>& maxPoli (
														addDegree == resDegree ? add : *this 
													);

				Polinomial<DIM>& minPoli (
														addDegree == resDegree ? *this : add 
													);													

				Polinomial<DIM> result(resDegree);
				if (_coeff.empty())
					convertStorage();
				if (add._coeff.empty())
					add.convertStorage();

				size_t ind(0);
				while (ind < minPoli.degree())
				{
					result._coeff[ind] = minPoli._coeff[ind] + maxPoli._coeff[ind];
					++ind;
				}

				while (ind < resDegree)
					result._coeff[ind] = maxPoli._coeff[ind];

				return result;
			}

			template <size_t N>
				Polinomial<DIM + N> tensor (const Polinomial<N>& p)
				{
					Polinomial<DIM + N> result();
					size_t i(0);
					size_t degree = 0;
					for (auto pol1 : _oneDFactors)
						result._oneDFactors[i++] = pol1;
	
					for (auto pol2 : p._oneDFactors)
						result._oneDFactors[i++] = pol2;

					result._degreeUpdated = false;
					return result;
				}

			//polinomial evaluation
			double operator() (Point<DIM> const& x) const
			{
				if (!_coeff.empty())
					return evaluate(x.begin());
				else
				{
					double tot = 1;
					for (size_t i = 0; i < DIM; ++i)
						tot *= _oneDFactors[i].evaluate(x[i]);
					return tot;
				}
			};


			size_t degree() const
			{
				if (!_degreeUpdated)
					updateDegree();
				return _degree;
			};

		protected:

			void updateDegree()
			{
				size_t d(0);
				if (!_coeff.empty())
				{
					for (size_t i = 0; i < _coeff.size(); ++i)
					{
						size_t curr = _coeff[i].degree() + i;
						if (curr > d)
							d = curr;
					}
				}
				else
				{
					for (auto pol : _oneDFactors)
						d += pol.degree();
				}
				_degree = d;
				_degreeUpdated = true;
			}

		private:
//INPUT TYPE TO BE CONVERTED TO ITERATOR
			double evaluate(double const* iter) const
			{
				auto next = iter + 1;
				double tot = 0;

				//begin() = rend()-1 : last one must not be multiplicated by x
				auto c_i = _coeff.rbegin();
				auto oneLessEnd = _coeff.rend() - 1;
				for (; c_i != oneLessEnd ; ++c_i)
				{
					tot += c_i->evaluate(next);
					tot *= (*iter);
				}
				tot += c_i->evaluate(next);
				return tot;				
			};

			void convertStorage()
			{
				if (_coeff.empty())
				{
					string ss("I'm not able to convert from 1DFactors in recursive format: TEACH ME");
					throw runtime_error(ss);
				}				
			}

/*
multivariate polinomial of DIM variables is generally stored as a univariate polinomial with polinomial<DIM-1> coefficients _coeff,
such that the total degree is equal to the _coeff size.
In the particular case the polinomial can be written as a product of univariate polinomial, these are stored in a DIM sized array
and _coeff is setted as empty, since it's useless
*/
			vector<Polinomial<DIM - 1> > _coeff;
			array<Polinomial<1>, DIM> _oneDFactors;

/*
degree value depends on how the multivariate polinomial is stored
degree evaluation is computed at runtime every time _degreeUpdated is false
every method that modifies polinomial coefficients must also set _degreeUpdated=false
*/
			size_t _degree;
			bool _degreeUpdated;
	};


	template <>
		class Polinomial<1>
		{
			public:
				//constructor to initialize coefficients size
				Polinomial (size_t);
				//constructor to initialize coefficients value
				Polinomial (const vector<double>&);

				//1D polinomial product
				Polinomial<1> operator * (const Polinomial<1>&) const;
				//1D polinomial sum
				Polinomial<1> operator + (const Polinomial<1>&) const;
				//method to access coefficient value of x^ind term
				double& operator [] (size_t ind);
				//evaluate polinomial in input point
				double operator() (Point<1> const&) const;
				//polinomial degree
				size_t degree() const;
			protected:
				void degree(size_t val);
			private:
				vector<double> _coeff;
		};


#endif //__POLY__HH



//	class Poly1D
//	{
//		public:
//			using CoeffContainer = std::vector<double>;

////remember that the polinomial degree is one less the size of the _coefficients vector
//		Poly1D();
//		Poly1D(std::vector<double>&);
//		Poly1D(std::size_t degree, double coeff = 0);

//		double operator () (Point<1>&);
//		double operator () (double&);
//		
//		std::size_t	degree ();
//		void			degree (std::size_t& p, double& val = 0);

//		private:
//			CoeffContainer _coefficients;
//	}
