#include "Poly.h"

	using std;

	Poly1D::Poly1D ()											: _coefficients () 						{};
	Poly1D::Poly1D (vector<double>& coeff)				: _coefficients (coeff) 				{};
	Poly1D::Poly1D (size_t& degree, double& coeff)	: _coefficients (degree + 1, coeff)	{};

	double Poly1D::operator() (Point<1>& x)
	{
		return (*this)(x[0]);
	};

	double Poly1D::operator() (double& x)
	{
		size_t p(0);
		return accumulate(
								_coefficients.begin(),
								_coefficients.end(),
								0.0,
								[&&] (double tot, double coeff) { return tot += coeff*pow(x, p++); }
								);
	};

	size_t Poly1D::degree ()
	{
		return _coefficients.size() - 1;
	};

	void Poly1D::degree (size_t& p, double& val)
	{
		_coefficients.resize(p + 1, val);
	}


