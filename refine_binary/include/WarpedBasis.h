#ifndef __WARPED_BASIS_H
#define __WARPED_BASIS_H

#include "Basis.h"


namespace FiniteElements
{
	class WarpedBasis : public AbstractBasis<2>
	{
		public:
			WarpedBasis();
			WarpedBasis(const WarpedBasis&);
			WarpedBasis& operator= (const WarpedBasis&);

			virtual double OneDEvaluation(size_t, double)const override;
			virtual double Evaluate(size_t, const Geometry::Point<2>&) override;
			virtual vector<double> EvaluateBasis(size_t, const Geometry::Point<2>&) override;
	};

}//namespace FiniteElements

#endif //__WARPED_BASIS_H
