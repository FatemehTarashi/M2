// Copyright 1997 Michael E. Stillman

#ifndef _weylalg_hh_
#define _weylalg_hh_

#include "polyring.hpp"
#include "gbring.hpp"

///// Ring Hierarchy ///////////////////////////////////

class WeylAlgebra : public PolynomialRing
{
protected:
  int _nderivatives;
  bool _homogeneous_weyl_algebra;
  int _homog_var;		// Only used if 'homogeneous_weyl_algebra' is true.
  int *_derivative;		// a value _derivative[i] = r >= 0 means that i is diff(r).
				// If < 0 : the variable i does not have a diff op.
  int *_commutative;		// Same as above, but in opposite direction.
  
  static int binomtop;
  static int diffcoeffstop;
  static int **binomtable;
  static int **diffcoeffstable;

  void initialize1();
  bool initialize_weyl(	
        M2_arrayint derivs,
	M2_arrayint comms,
	int homog_var);
  WeylAlgebra() {}
  virtual ~WeylAlgebra() {}
protected:
  
  void extractDerivativePart(const int *exponents, int *result) const;
  void extractCommutativePart(const int *exponents, int *result) const;
  ring_elem binomial(int top, int bottom) const;
  ring_elem multinomial(const ring_elem a, const int *exptop, const int *expbottom) const;
  bool increment(int *current_derivative, const int *top_derivative) const;

  bool divides(const int *expbottom, const int *exptop) const;
  ring_elem diff_coefficients(const ring_elem c, const int *derivatives, const int *exponents) const;

  Nterm * weyl_diff(
	  const ring_elem c,
	  const int *expf,  // The exponent vector of f
	  const int *derivatives, 
	  const Nterm *g) const;  // An entire polynomial
  vec weyl_diff(
	  const ring_elem c,
	  const int *expf,  // The exponent vector of f
	  const int *derivatives, 
	  const vec g) const;  // An entire polynomial
  vec weyl_diff(
	  const FreeModule *resultF,
	  const ring_elem c,
	  const int *expf,  // The exponent vector of f
	  int component,
	  const int *derivatives, 
	  const Nterm *g) const;  // An entire polynomial

  gbvector * gbvector_weyl_diff(
	  GBRing *GR,
	  const ring_elem c, // in K
	  int comp, // adds this component to each component of g.
	  const int *expf,  // The exponent vector of f
	  const int *derivatives, 
	  const gbvector *g) const;  // An entire polynomial
public:
  static WeylAlgebra *create(const Ring *R,
			     M2_arrayint derivatives,
			     M2_arrayint commutatives,
			     int homog_var);

  virtual bool is_pid() const       { return false; }
  virtual bool has_gcd() const      { return false; }
  virtual bool is_graded() const    { return _isgraded; }
  virtual bool is_expensive() const { return true; }

  virtual bool is_commutative_ring() const { return false; }
  virtual bool is_weyl_algebra() const { return true; }

  virtual const WeylAlgebra *cast_to_WeylAlgebra() const { return this; }

  virtual void text_out(buffer &o) const;

  virtual ring_elem power(const ring_elem f, mpz_t n) const;
  virtual ring_elem power(const ring_elem f, int n) const;

  ring_elem multinomial(const int *exptop, const int *exp) const;
  
public:
  virtual ring_elem imp_mult_by_term(const ring_elem f, 
				     const ring_elem c, const int *m) const;

  gbvector * gbvector_mult_by_term(gbvectorHeap &result,
				   const gbvector *f, 
				   const ring_elem c, // in the base K
				   const int *m, // monomial, in M
				   int comp) const; // comp is either 0 or a real component.
};

#endif
