#ifndef __COEFFICIENT_COMPUTER_H
#define __COEFFICIENT_COMPUTER_H

#include "TypeEnumerations.h"
#include "AbstractFElement.h"
#include "BinaryTreeHelper.h"

#include <stdexcept> //std::logic_error

namespace FiniteElements
{
	template <size_t dim, BasisType FeType = InvalidFeType>
		struct CoefficientComputer
		{
			static void ComputeCoefficients (const AbstractFElement<dim, FeType>&,
														BinaryTree::FunctionPtr<dim>,
														vector<double>*)
			{
//TODO: scrivere la versione generale per il calcolo dei coefficienti (prodotto l2 tra le funzioni di base etc.)
				throw invalid_argument("I don't know how to compute coefficients for " + to_string(FeType) + " finite element type");
			};
		};

	template <size_t dim>
		struct CoefficientComputer<dim, InvalidFeType>
		{
			static void ComputeCoefficients (const AbstractFElement<dim, InvalidFeType>&,
														BinaryTree::FunctionPtr<dim>,
														vector<double>*)
			{
				throw logic_error("Trying to compute coefficients for an InvalidFeType object");
			};
		};

	template <size_t dim>
		struct CoefficientComputer<dim, LegendreType>
		{
			static void ComputeCoefficients (const AbstractFElement<dim, LegendreType>& f_el,
														BinaryTree::FunctionPtr<dim> interp_func,
														vector<double>* coeff_location)
			{
				//I remember how many coefficients have been already computed
				size_t cursor = coeff_location->size();
				//New number of coefficients
				size_t s = f_el.BasisSize();

				if (cursor == s)
					//Coefficients already computed
					return;

				//memory allocation
				coeff_location->resize(s);

				//I don't recompute already stored coefficients
				for(; cursor < s; ++cursor)
				{
					(*coeff_location)[cursor] = f_el.L2prod ( *interp_func,
																			[&f_el, &cursor]
																			(const Geometry::Point<dim>& p)
																			{ return f_el.EvaluateBasisFunction(cursor, p);}
																		 );
//TODO: controllare se nel caso multidimensionale continua a funzionare

					/* dividing by (k + 0.5)^-1 */
					(*coeff_location)[cursor] *= (cursor + 0.5);
				}	
			};
		};
} //namespace FiniteElements

#endif //__COEFFICIENT_COMPUTER_H
