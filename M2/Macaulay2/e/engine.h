/* Copyright 2002 by Michael E. Stillman */

#ifndef _engine_h_
#define _engine_h_

/**
   \mainpage Hi, this is my main documentation page.
 */

#include "engine-includes.hpp"

#if defined(__cplusplus)
class Monomial;
class Monoid;
class FreeModule;
class MonomialIdeal;
class Matrix;
class MutableMatrix;
class RingMap;
class Computation;
class EngineComputation;
class MutableComplex;
// NAG begin
class M2SLEvaluator;
class M2Homotopy;
class M2SLProgram;
class StraightLineProgram;
class PathTracker;
class M2PointArray;
// NAG end
#else
/* Define the externally visible types here */
typedef struct Monomial Monomial;
typedef struct Monoid Monoid;
typedef struct FreeModule FreeModule;
typedef struct Matrix Matrix;
typedef struct MutableMatrix MutableMatrix;
typedef struct RingMap RingMap;
typedef struct Computation Computation;
typedef struct EngineComputation EngineComputation;
typedef struct MonomialIdeal MonomialIdeal;
typedef struct MutableComplex MutableComplex;
// NAG begin
typedef struct M2SLEvaluator M2SLEvaluator;
typedef struct M2Homotopy M2Homotopy;
typedef struct M2SLProgram M2SLProgram;
typedef struct StraightLineProgram StraightLineProgram;
typedef struct PathTracker PathTracker;
typedef struct M2PointArray M2PointArray;
// NAG end
#endif

#include "interface/aring.h"
#include "interface/computation.h"
#include "interface/factory.h"
#include "interface/flint.h"
#include "interface/freemodule.h"
#include "interface/groebner.h"
#include "interface/matrix.h"
#include "interface/monoid.h"
#include "interface/monomial-ordering.h"
#include "interface/random.h"
#include "interface/ring.h"
#include "interface/ringelement.h"
#include "interface/ringmap.h"

#include "interface/NAG.h"

#if defined(__cplusplus)
extern "C" {
#endif
  void IM2_initialize(void); /* drg: connected */
  M2_string IM2_last_error_message(void); /* drg: connected */

  M2_string engineMemory(); /* connected MES to engineMemory */
  
  /**************************************************/
  /**** Monomial routines ***************************/
  /**************************************************/
  /* Monomials in the engine: are not associated with a monoid,
   * and may have negative exponents. Monomials are immutable objects.
   */

  /**************************************************/
  /**** polynomial ring element routines ************/
  /**************************************************/

  M2_bool IM2_RingElement_is_graded(const RingElement *a); /* drg: connected rawIsHomogeneous*/

  M2_arrayintOrNull IM2_RingElement_multidegree(const RingElement *a); /* drg: connected rawMultiDegree*/

  gmp_ZZpairOrNull rawWeightRange(M2_arrayint wts, const RingElement *a); /* drg: connected rawWeightRange*/
    /* The first component of the degree is used, unless the degree monoid is trivial,
       in which case the degree of each variable is taken to be 1.
       Returns lo,hi degree.  If the ring is not a graded ring or a polynomial ring
       then (0,0) is returned.
    */

  const RingElement* /* or null */ rawRingElementAntipode(const RingElement* f);
  /* If the ring is not a skew commuting poly ring, this is the identity map.  Otherwise
     this returns a poly, with the signs of the coefficients possibly changed, 
     this implements the (anti-)isomorphism of the ring and its opposite ring.
  */

  const Matrix* /* or null */ rawHomogenizeMatrix(const Matrix* a, const Matrix* b, const Matrix* c);
  /* TEST dummy function!! */
  
  const RingElement /* or null */ *IM2_RingElement_homogenize_to_degree(
            const RingElement *a,
            int v,
            int deg,
            M2_arrayint wts); /* drg: connected rawHomogenize*/

  const RingElement /* or null */ *IM2_RingElement_homogenize(
            const RingElement *a,
            int v,
            M2_arrayint wts); /* drg: connected rawHomogenize*/

  const RingElement /* or null */ *IM2_RingElement_term(
            const Ring *R,
            const RingElement *a,
            const Monomial *m); /* drg: connected rawTerm*/
    /* R must be a polynomial ring, and 'a' an element of the
       coefficient ring of R.  Returns a*m, if this is a valid
       element of R.  Returns NULL if not (with an error message).
    */

  const RingElement /* or null */ *IM2_RingElement_get_terms(
            int nvars, /* n variables in an outermost monoid */
            const RingElement *a,
            int lo, int hi); /* drg: connected rawGetTerms*/
    /* Returns the sum of some monomials of 'a', starting at 'lo',
       going up to 'hi'.  If either of these are negative, they are indices
       from the back of the polynomial.
       'a' should be an element of a polynomial ring.
    */

  const RingElement /* or null */ *IM2_RingElement_get_coeff(
            const Ring * coeffRing, /* ring of the result */
            const RingElement *a,
            const Monomial *m); /* drg: connected rawCoefficient*/
    /* Return (as an element of the coefficient ring) the coeff
       of the monomial 'm'.
    */

  const RingElement /* or null */ *IM2_RingElement_lead_coeff(
            const Ring * coeffRing, /* ring of the result */
            const RingElement *a); /* drg: connected rawLeadCoefficient*/

  const Monomial /* or null */ *IM2_RingElement_lead_monomial(
            int nvars, /* number of variables in an outermost monoid */
            const RingElement *a); /* drg: connected rawLeadMonomial*/

  int IM2_RingElement_n_terms(
            int nvars, /* number of variables in an outermost monoid */
            const RingElement *a); /* drg: connected rawTermCount*/

  engine_RawArrayPairOrNull IM2_RingElement_list_form(
            const Ring * coeffRing, /* ring of the result coefficients */
            const RingElement *f); /* drg: connected rawPairs */

  engine_RawRingElementArrayOrNull rawConvolve(engine_RawRingElementArray H,
                                               int convolve_type);
  /* assumes: H[0]..H[n] are in a ring.
     returns the array determined by convolving H
     (see def in x-relem.cpp for more info)
  */

  engine_RawRingElementArray rawGetParts(const M2_arrayint wts,
                                const RingElement *f);
  /* Return an array of RingElement's, each having pure weight, and sorted by
     strictly increasing weight value.  The wt vector values must fit into
     a word length integer.  */

  const RingElement /* or null */ * rawGetPart(const M2_arrayint wts,
                                     const RingElement *f,
                                     M2_bool lobound_given,
                                     M2_bool hibound_given,
                                     long lobound,
                                     long hibound);
  /* Return the sum of all of the terms t of f, which satisfy: lobound <= wt.t <= hibound,
     where, if lobound_given is false, then lobound is -infinity, and if hibound_given
     is false, then hibound is infinity. */

  int IM2_RingElement_index_if_var(const RingElement *f); /* drg: connected rawIndexIfVariable */
  /* if f is a variable of its ring, then the index of that variable is returned.
     If f is not a variable, then -1 is returned. */

  M2_arrayint rawRingElementIndices(const RingElement *f); /* drg: connected rawIndices */
  /* The list of indices of variables which occur in f is returned. */

  const RingElement /* or null */ * rawAssociateDivisor(const RingElement *f);
  /* A unit 'a' in the base coefficient ring, such that a*f is the preferred associate of f.
     For example, if f = -9x+6 in QQ[x], then -3 is returned.
     If the (ultimate) base ring of f is QQ, then a*f has no denominators.
     If the base ring of f is frac(K[x]), K=ZZ,QQ, or another field, x is a set of vars
     then a*f is in ZZ[x], or K[x].
     If the base coefficient ring is the fraction ring of a quotient poly ring, then
     an error is flagged, and NULL is returned.
   */

  ////  Content ////////////////////////////////////////
  // The content of a ring element, polynomial or vector is defined to be an
  // element of the base (either a finite field, ZZ, QQ, or a fraction)
  // If base is ZZ: the result is the gcd of all of the terms appearing in the polynomial or vector
  //   The sign is chosen so that after division by the content the resulting polynomial is monic
  // If base is QQ or frac(A): the numerator is the gcd of all of the numerators,
  //                the denominator is the lcm of all of the denominators
  // If base is kk: the result is the lead coefficient.  For vectors, what should this mean?
  //////////////////////////////////////////////////////

  const RingElement /* or null */ *rawRingElementContent(const RingElement *f); /* connect to rawContent */
  // returns the content of f (as a matrix over the base coefficient ring)

  const RingElement /* or null */ *rawRingElementRemoveContent(const RingElement *f); /* connect to rawRemoveContent */
  // returns the polynomial which results after division by the content

  const RingElement /* or null */ *rawRingElementSplitContent(const RingElement *f, const RingElement /* or null */ **result); /* connect to rawSplitContent */
  // returns the content of f (as a matrix over the base coefficient ring)
  // result is set to the polynomial which results after division by the content

  /**************************************************/
  /**** fraction field ring element routines ********/
  /**************************************************/

  /** The numerator of a fraction.
   * \param a An element of a fraction ring frac(R).
   * \return The numerator of a, as an element of the ring R.
   *
   * Connected as rawNumerator.
   *
   */
  const RingElement *IM2_RingElement_numerator(const RingElement *a); /* drg: connected rawNumerator*/

  /** The denominator of a fraction.
   * \param a An element of a fraction ring frac(R).
   * \return The denominator of a, as an element of the ring R.
   *
   * Connected as rawDenominator.
   *
   */
  const RingElement *IM2_RingElement_denominator(const RingElement *a); /* drg: connected rawDenominator*/

  const RingElement /* or null */ *IM2_RingElement_fraction(const Ring *R,
                                                    const RingElement *a,
                                                    const RingElement *b); /* drg: connected rawFraction*/

  gmp_ZZ /* or null */ rawSchurDimension(const RingElement *f); /* connected rawSchurDimension */
  /* f should be a polynomial whose base ring was created using rawSchurRing
     (otherwise NULL is returned).  If so, the dimension of the corresponding
     (virtual) GL(n) representation is returned. */

  const RingElement /* or null */ *rawSchurFromPartition(const Ring *R, M2_arrayint part);
  /* if R is a SchurRing2, then return the element corresponding to the given partition */

  const RingElement /* or null */ *rawSchurSnTensorMult(const RingElement *f, const RingElement *g);
  /* the tensor multiplication function in SchurSnRing */

  long rawDiscreteLog(const RingElement *h); /* drg: connected */
  /* returns -1 if h is 0, or if elements of the ring of h
     are not represented as powers of a primitive element.
     Otherwise returns an integer in the range 0..q-1 */

  int rawDegree(int v, const RingElement *f); /* connected to rawDegree */
  /* Returns -1 if 0 or not implemented for a given ring.  For now, valid only for tower rings */

  int rawExtensionDegree(int v, const Ring *R); /* connected to rawExtensionDegree */
  /* Currently only valid for tower rings.  Others return 0.  */

  const RingElement /* or null */ *rawDiff(int v, const RingElement *f);  /* connected */
  /* Currently only valid for tower rings */

  const RingElement /* or null */ *rawLowerP(const RingElement *f);  /* connected */
  /* Currently only valid for tower rings */

  const RingElement *rawTowerTranslatePoly(const Ring *newRing, const RingElement *F);
  /* 2 cases: ring of F is a polynomial ring, and newRing is a Tower.
     second case: ring of F is a tower, and newRing is a polynomial ring.
     In both cases, the two rings should have the same characteristic, and the same number of variables.
     This then translates F, returning the translated poly in the ring newRing.
  */

  const RingElement /* or null */ *rawPowerMod(const RingElement *f, mpz_srcptr n, const RingElement *g);  /* connected */
  /* Currently only valid for tower rings */

  /**************************************************/
  /**** Matrix routines *****************************/
  /**************************************************/

  const FreeModule * IM2_Matrix_get_target(const Matrix *M); /* drg: connected rawTarget*/

  const FreeModule * IM2_Matrix_get_source(const Matrix *M); /* drg: connected rawSource, used in rawMatrixColumns*/

  int IM2_Matrix_n_rows(const Matrix *M); /* drg: connected rawNumberOfRows*/

  int IM2_Matrix_n_cols(const Matrix *M); /* drg: connected rawNumberOfColumns*/

  M2_arrayint IM2_Matrix_get_degree(const Matrix *M); /* drg: connected rawMultiDegree*/

  M2_string IM2_Matrix_to_string(const Matrix *M); /* drg: connected */

  unsigned int rawMatrixHash(const Matrix *M); /* drg: connected to "hash"  */

  const RingElement /* or null */ * IM2_Matrix_get_entry(const Matrix *M, int r, int c); /* drg: connected rawMatrixEntry, OK*/

  /*******************************************************************************/
  const Matrix * IM2_Matrix_identity(const FreeModule *F,
                                     int preference
                                     ); /* drg: connected rawIdentity, OK*/

  const Matrix /* or null */ * IM2_Matrix_zero(const FreeModule *F,
                                       const FreeModule *G,
                                       int preference
                                       ); /* drg: connected rawZero, OK */

  const Matrix /* or null */ * IM2_Matrix_make1(const FreeModule *target,
                                        int ncols,
                                        const engine_RawRingElementArray M,
                                        int preference); /* drg: connected rawMatrix1, OK */

  const Matrix /* or null */ * IM2_Matrix_make2(const FreeModule *target,
                                        const FreeModule *source,
                                        M2_arrayint deg,
                                        const engine_RawRingElementArray M,
                                        int preference); /* drg: connected rawMatrix2, OK */

  const Matrix /* or null */ * IM2_Matrix_make_sparse1(const FreeModule *target,
                                               int ncols,
                                               M2_arrayint rows,
                                               M2_arrayint cols,
                                               const engine_RawRingElementArray entries,
                                               int preference); /* drg: connected rawSparseMatrix1, OK */

  const Matrix /* or null */ * IM2_Matrix_make_sparse2(const FreeModule *target,
                                               const FreeModule *source,
                                               M2_arrayint deg,
                                               M2_arrayint rows,
                                               M2_arrayint cols,
                                               const engine_RawRingElementArray entries,
                                               int preference); /* drg: connected rawSparseMatrix2, OK */

  M2_bool IM2_Matrix_is_implemented_as_dense(const Matrix *M); /* connected to rawIsDense */
  /* Is the matrix M implemented in the engine as a dense matrix? */

  const Matrix /* or null */ * IM2_Matrix_remake1(const FreeModule *target,
                                          const Matrix *M,
                                          int preference
                                          ); /* drg: connected rawMatrixRemake1, OK  */
  /* Create a new matrix (mutable or immutable), from M, with new target,
     and/or mutable-ness. The target free module must have the expected rank.
     The source free module is computed heuristically from the the target and the
     columns of the matrix.
  */

  const Matrix /* or null */ * IM2_Matrix_remake2(const FreeModule *target,
                                          const FreeModule *source,
                                          M2_arrayint deg,
                                          const Matrix *M,
                                          int preference
                                          ); /* drg: connected rawMatrixRemake2, OK */
  /* Create a new matrix (mutable or immutable), from M, with new target,
     source, deg and/or mutable-ness. The new free modules must have
     the expected rank.
  */

  const Matrix /* or null */ *IM2_Matrix_random(const Ring *R,
                                  int r, int c,
                                  double fraction_non_zero,
                                  int special_type, /* 0: general, 1:upper triangular, others? */
                                  int preference); /* connected to rawRandomConstantMatrix, OK */

  /**********************************************************************************/

  M2_bool IM2_Matrix_is_zero(const Matrix *M); /* drg: connected rawIsZero*/

  int IM2_Matrix_is_equal(const Matrix *M, const Matrix *N); /* drg: connected === and to rawIsEqual for use with == */
        // 1 = true, 0 = false, -1 = error
    /* This checks that the entries of M,N are the same, as well as
       that the source and target are the same (as graded free modules).
       Therefore, it can happen that M-N == 0, but M != N.
    */

  M2_bool IM2_Matrix_is_graded(const Matrix *M); /* drg: connected rawIsHomogeneous*/

  const Matrix /* or null */ * IM2_Matrix_concat(const engine_RawMatrixArray Ms); /* drg: connected rawConcat*/

  const Matrix /* or null */ * IM2_Matrix_direct_sum(const engine_RawMatrixArray Ms); /* drg: connected rawDirectSum*/

  const Matrix /* or null */ * IM2_Matrix_tensor(const Matrix *M,
                                         const Matrix *N); /* drg: connected rawTensor*/

  const Matrix /* or null */ * IM2_Matrix_transpose(const Matrix *M); /* drg: connected rawDual*/

  const Matrix /* or null */ * IM2_Matrix_reshape(const Matrix *M,
                                          const FreeModule *F,
                                          const FreeModule *G); /* drg: connected rawReshape*/

  const Matrix /* or null */ * IM2_Matrix_flip(const FreeModule *F,
                                       const FreeModule *G); /* drg: connected rawFlip*/

  const Matrix /* or null */ * rawWedgeProduct(int p,
                                       int q,
                                       const FreeModule *F); /* drg: connected rawWedgeProduct */
  /* Constructs the map
     exterior(p,F) ** exterior(q,F) --> exterior(p+q,F)
  */

  const Matrix /* or null */ * IM2_Matrix_submatrix(const Matrix *M,
                                            M2_arrayint rows,
                                            M2_arrayint cols); /* drg: connected rawSubmatrix*/

  const Matrix /* or null */ * IM2_Matrix_submatrix1(const Matrix *M,
                                             M2_arrayint cols); /* drg: connected rawSubmatrix*/

  const Matrix /* or null */ * IM2_Matrix_koszul(int p, const Matrix *M); /* drg: connected rawKoszul*/

  const Matrix /* or null */ *
  rawKoszulMonomials(int nskew,
                     const Matrix *M,
                     const Matrix *N); /* drg: connected rawKoszulMonomials */
  /* M and N should each have one row, and the base ring should be a
     polynomial ring.  The (i,j) th entry of the resulting matrix is
     1 or -1 times N_j/M_i (if M_i divides N_j). The sign is determined only from
     the first nskew variables.  The sign is the sign of M_i * (N_j/M_i) in
     exterior algebra (on this set of variables).  The actual commutativity of the
     common ring of M and N is ignored. */

  const Matrix /* or null */ * IM2_Matrix_symm(int p, const Matrix *M); /* drg: connected rawSymmetricPower*/

  const Matrix /* or null */ * IM2_Matrix_exterior(int p, const Matrix *M, int strategy); /* drg: connected rawExteriorPower*/

  M2_arrayint IM2_Matrix_sort_columns(const Matrix *M,
                                            int deg_order,
                                            int mon_order); /* drg: connected rawSortColumns*/


  const Matrix /* or null */ * IM2_Matrix_minors(int p, const Matrix *M, int strategy); /* drg: unconnected*/

  const Matrix /* or null */ * rawMinors(int p,
                           const Matrix *M,
                           int strategy,
                           int n_minors_to_compute, /* -1 means all */
                           M2_arrayintOrNull first_row_set,
                           M2_arrayintOrNull first_col_set
                           ); /* connected to rawMinors */
  /* If first_row_set or first_col_set is not NULL, they should both be non-NULL,
     and both have length p.  If not, NULL is returned.
     Compute n_minors_to_compute minors, starting at (first_row_set,first_col_set) if given,
     otherwise starting at the first (0..p-1,0..p-1).
  */

  const Matrix /* or null */ * IM2_Matrix_pfaffians(int p, const Matrix *M); /* drg: connected rawPfaffians*/

  const Matrix * rawMatrixCompress(const Matrix *M); /* connected rawMatrixCompress */

  const Matrix /* or null */ * IM2_Matrix_uniquify(const Matrix *M); /* TODO */
  /* if any two columns are the same up to a scalar multiple, then keep only
     one of the columns.  Remove any zero columns too.
     The definition of "same up to a scalar" is this:
     if K is the base field or ring (i.e. QQ in QQ(x,y,z)[s,t]),
     and if c and d are the lead scalar coeff's of vecs v,w, resp, then
     v and w are scalar multiplies iff d*v == c*w.
     Warning: Over non-domains, this might not be the intended effect.
  */

  const Matrix * rawRemoveScalarMultiples(const Matrix *m); /* connected */

  const Matrix * rawRemoveMonomialFactors(const Matrix *m, M2_bool make_squarefree_only);
  /* connected to rawRemoveMonomialFactors */

  ////  Content ////////////////////////////////////////
  // The content of a ring element, polynomial or vector is defined to be an
  // element of the base (either a finite field, ZZ, QQ, or a fraction)
  // If base is ZZ: the result is the gcd of all of the terms appearing in the polynomial or vector
  //   The sign is chosen so that after division by the content the resulting polynomial is monic
  // If base is QQ or frac(A): the numerator is the gcd of all of the numerators,
  //                the denominator is the lcm of all of the denominators
  // If base is kk: the result is the lead coefficient.  For vectors, what should this mean?
  //////////////////////////////////////////////////////

  const Matrix /* or null */ *rawMatrixContent(const Matrix *M); /* connect to rawContent */
  // returns the matrix of the content of each column of M

  const Matrix /* or null */ *rawMatrixRemoveContent(const Matrix *M); /* connect to rawRemoveContent */
  // returns the matrix with the content (as defined above) removed

  const Matrix /* or null */ *rawMatrixSplitContent(const Matrix *M, const Matrix /* or null */ **result); /* connect to rawSplitContent */
  // returns the matrix of the content of each column of M,
  // and result is set to the result of rawMatrixRemoveContent.

  const Matrix /* or null */ * IM2_Matrix_remove_content(const Matrix *M);      /* connected rawRemoveContent*/

  /* Routines for use when the base ring is a polynomial ring of some sort */

  const Matrix /* or null */ * IM2_Matrix_diff(const Matrix *M,
                                       const Matrix *N); /* drg: connected rawMatrixDiff*/

  const Matrix /* or null */ * IM2_Matrix_contract(const Matrix *M,
                                           const Matrix *N); /* drg: connected rawMatrixContract*/

  const Matrix /* or null */ * IM2_Matrix_homogenize(const Matrix *M,
                                             int var,
                                             M2_arrayint wts); /* drg: connected rawHomogenize*/

  const struct engine_RawMatrixPair_struct /* or null */ *IM2_Matrix_coeffs(const Matrix *M, M2_arrayint vars) ;/* TODO */


  const Matrix /* or null */ * rawCoefficients(M2_arrayint vars,
                                       const Matrix *monoms,
                                       const Matrix *M); /* drg: connected as rawCoefficients*/
  /* Given:
   *  vars : a list of variable indices in the (common) ring R of monoms and M
   *  monoms : a map R^b --> R^a such that each column has exactly one monomial
   *      which is only in the variables in 'vars'.
   *  M : a map R^c --> R^a such that every (module) monomial of each column of M
   *      matches one of the columns of 'monoms', in the variables 'vars'.
   *
   * Returns: a matrix C : R^c --> R^b such that
   *      (i) the entries of C do not involve the variables in 'vars', and
   *      (ii) monoms * C == M
   *
   * Assumptions on rings: if R is non-commutative, then the variables in 'vars'
   *      should commute with the variables outside of 'vars'.
   *
   * If each column of monoms has more than one monomial, or if variables other than
   *      those in 'vars' occur, then only the first monomial is used, and the other
   *      variables are ignored.
   * If a monomial occurs twice, then one of them will be used (which one is left undefined)
   */

  const Matrix /* or null */ * IM2_Matrix_monomials(M2_arrayint vars, const Matrix *M); /* drg: connected rawMonomials*/

  const Matrix * IM2_Matrix_initial(int nparts, const Matrix *M); /* drg: connected rawInitial*/

  M2_arrayint IM2_Matrix_elim_vars(int nparts, const Matrix *M); /* drg: connected rawEliminateVariables*/

  M2_arrayint IM2_Matrix_keep_vars(int nparts, const Matrix *M); /* drg: connected rawKeepVariables*/

  engine_RawMatrixAndInt IM2_Matrix_divide_by_var(const Matrix *M, int var, int maxdegree); /* drg: connected rawDivideByVariable*/
  /* If M = [v1, ..., vn], and x = 'var'th variable in the ring,
     return the matrix [w1,...,wn], where wi * x^(ai) = vi,
     and wi is not divisible by x, or ai = maxdegree,
     and the integer which is the maximum of the ai's.
     QUESTION: what rings should this work over?
  */

  engine_RawMatrixPairOrNull rawTopCoefficients(const Matrix *M); /* connected to rawTopCoefficients */
  /* Returns a pair of matrices: the first is a list of monomials (of form var^exp),
     and the second has the same row space as M.  For each column, find the smallest
     index variable, var,  which occurs, and exp, the largest degree to which it occurs
     in that column.  Place var^exp in the first matrix.
     Place the coeff of var^exp (a vector) into the second matrix.
     If the ring is not a polynomial ring, an error is given, and Null is returned.
  */

  M2_arrayintOrNull rawMatrixIndices(const Matrix *f); /* connected to rawIndices */

  M2_arrayint IM2_Matrix_min_leadterms(const Matrix *M, M2_arrayint vars); /* TODO */

  const Matrix /* or null */ * IM2_Matrix_auto_reduce(const Matrix *M); /* TODO */

  const Matrix /* or null */ * IM2_Matrix_reduce(const Matrix *M, const Matrix *N); /* TODO */

  const Matrix /* or null */ * IM2_Matrix_reduce_by_ideal(const Matrix *M, const Matrix *N); /* TODO */

  /* Routines when considering matrices as modules of some sort */

  const Matrix /* or null */ * rawModuleTensor(const Matrix *M,
                                       const Matrix *N); /* connected rawModuleTensor */

  const Matrix /* or null */ * rawBasis(const Matrix *M,
                                M2_arrayint lo_degree, /* possibly length 0 */
                                M2_arrayint hi_degree,
                                M2_arrayint wt,
                                M2_arrayint vars,
                                M2_bool do_truncation,
                                int limit); /* connected to rawBasis */
  /* Yields a monomial basis of part of the graded R-module cokernel(M).
   * Returns a matrix of monomials which maps to the target of M, such that
   *  (i) The image spans the sum of M_i, for lo_degree <= i <= hi_degree
   *       where M_i is the degree i piece of M.
   * Notes:
   *  -- 'vars' is a list of variables.  The entries of the result will only involve
   *     these variables.
   *  -- 'wt' should be a list of integers of length <= number of degrees,
   *     with the property that each variable in 'vars' has (its degree) dot wt > 0.
   *  -- if lo_degree has length 0, then it is assumed to be -infinity
   *  -- if hi_degree has length 0, then it is assumed to be infinity
   *  -- in either of these cases, or if lo_degree is not equal to hi_degree, then
   *     the degree ring of R must have one variable.
   *  -- if limit >= 0, then only the first 'limit' monomials are placed into the result.
   *  -- if do_truncation is set, then monomials of degree higher than hi_degree will be
   *     placed into .
   *
   * If R is a quotient ring, then the monomial order had better be a product order
   * such that the first block (or blocks) consists of the variables in 'vars'.
   *
   */

  int IM2_Matrix_dimension(const Matrix *M); /* TODO */

  const RingElement /* or null */ * IM2_Matrix_Hilbert(const Matrix *M); /* drg: connected rawHilbert*/
  /* This routine computes the numerator of the Hilbert series
     for coker leadterms(M), using the degrees of the rows of M.
     NULL is returned if the ring is not appropriate for
     computing Hilbert series, or the computation was interrupted. */

  const Matrix * IM2_kernel_of_GB(const Matrix *G); /* connected rawKernelOfGB */
  /* Assuming that the columns of G form a GB, this computes
     a Groebner basis of the kernel of these elements, using an appropriate Schreyer order on the
     source of G. */

  /**************************************************/
  /**** MutableMatrix routines **********************/
  /**************************************************/

  MutableMatrix * IM2_MutableMatrix_identity(const Ring *R,
                                             int nrows,
                                             M2_bool prefer_dense); /* drg: connected rawMutableIdentity, OK*/

  MutableMatrix * IM2_MutableMatrix_make(const Ring *R,
                                         int nrows,
                                         int ncols,
                                         M2_bool prefer_dense); /* drg: connected rawMutableMatrix, OK */

  MutableMatrix * IM2_MutableMatrix_from_matrix(const Matrix *M, M2_bool prefer_dense); /* drg: connected rawMutableMatrix, OK*/


  const Matrix * IM2_MutableMatrix_to_matrix(const MutableMatrix *M); /* drg: connected rawMatrix, OK*/

  M2_string IM2_MutableMatrix_to_string(const MutableMatrix *M); /* drg: connected toString, OK */

  unsigned int  rawMutableMatrixHash(const MutableMatrix *M); /* drg: connected to "hash" */

  int IM2_MutableMatrix_n_rows(const MutableMatrix *M); /* drg: connected rawNumberOfRows, OK */

  int IM2_MutableMatrix_n_cols(const MutableMatrix *M); /* drg: connected rawNumberOfColumns, OK */

  void rawMutableMatrixFillRandomDensity(MutableMatrix *M, double density, int special_type);
  /* drg: connected rawMutableMatrixFillRandom */
  /* special_type: 0 is general, 1 is (strictly) upper triangular. */

  void rawMutableMatrixFillRandom(MutableMatrix *M, long nelems);
  /* drg: connected rawMutableMatrixFillRandom */

  MutableMatrix /* or null */ *rawMutableMatrixPromote(const Ring *R, const MutableMatrix *f); /* connected to rawPromote*/

  MutableMatrix /* or null */ *rawMutableMatrixLift(int *success_return, const Ring* R, const MutableMatrix *f); /* connected to rawLift */
  // returns null if lifting not possible

  const RingElement /* or null */ * IM2_MutableMatrix_get_entry(const MutableMatrix *M,
                                                        int r, int c); /* drg: connected rawMatrixEntry, OK*/

  /* Each of these routines returns false if there was an error. */

  M2_bool IM2_MutableMatrix_set_entry(MutableMatrix *M, int r, int c, const RingElement *a); /* drg: connected rawSetMatrixEntry, OK */

  M2_bool IM2_MutableMatrix_row_swap(MutableMatrix *M, int i, int j); /* drg: connected rawMatrixRowSwap, OK */

  M2_bool IM2_MutableMatrix_column_swap(MutableMatrix *M, int i, int j); /* drg: connected rawMatrixColSwap, OK*/

  M2_bool IM2_MutableMatrix_row_operation(MutableMatrix *M,
                                          int i,
                                          const RingElement *r,
                                          int j,
                                          M2_bool opposite_mult); /* drg: connected rawMatrixRowChange, OK */
  /* row(i) <- row(i) + r * row(j), returns false if matrix is
     immutable, or rows are out of bounds */

  M2_bool IM2_MutableMatrix_column_operation(MutableMatrix *M,
                                             int i,
                                             const RingElement *r,
                                             int j,
                                             M2_bool opposite_mult); /* drg: connected rawMatrixColChange, OK*/
  /* column(i) <- column(i) + r * column(j), returns false if matrix is
     immutable, or columns are out of bounds */

  M2_bool IM2_MutableMatrix_row_scale(MutableMatrix *M,
                                      const RingElement *r,
                                      int i,
                                      M2_bool opposite_mult); /* drg: connected rawMatrixRowScale, OK*/
  /* row(i) <- r * row(i), returns false if matrix is immutable
     or row is out of bounds */

  M2_bool IM2_MutableMatrix_column_scale(MutableMatrix *M,
                                         const RingElement *r,
                                         int i,
                                         M2_bool opposite_mult); /* drg: connected rawMatrixColumnScale, OK */
  /* column(i) <- r * column(i), returns false if matrix is immutable
     or row is out of bounds */

  M2_bool IM2_MutableMatrix_insert_columns(MutableMatrix *M, int i, int n_to_add); /* connected to rawInsertColumns, OK */
  /* Insert n_to_add columns directly BEFORE column i. */

  M2_bool IM2_MutableMatrix_insert_rows(MutableMatrix *M, int i, int n_to_add); /* connected to rawInsertRows, OK */
  /* Insert n_to_add rows directly BEFORE row i. */

  M2_bool IM2_MutableMatrix_delete_columns(MutableMatrix *M, int i, int j); /* connected to rawDeleteColumns, OK */
  /* Delete columns i .. j from M */

  M2_bool IM2_MutableMatrix_delete_rows(MutableMatrix *M, int i, int j); /* connected to rawDeleteRows, OK  */
  /* Delete rows i .. j from M */


  M2_bool IM2_MutableMatrix_column_2by2(MutableMatrix *M,
                                        int c1, int c2,
                                        const RingElement *a1, const RingElement *a2,
                                        const RingElement *b1, const RingElement *b2,
                                        M2_bool opposite_mult); /* connected to rawMatrixColumnOperation2, OK */
  /* column(c1) <- a1 * column(c1) + a2 * column(c2)
     column(c2) <- b1 * column(c1) + b2 * column(c2)
  */

  M2_bool IM2_MutableMatrix_row_2by2(MutableMatrix *M,
                                     int r1, int r2,
                                     const RingElement *a1, const RingElement *a2,
                                     const RingElement *b1, const RingElement *b2,
                                     M2_bool opposite_mult); /* connected to rawMatrixRowOperation2, OK */
  /* row(r1) <- a1 * row(r1) + a2 * row(r2)
     row(r2) <- b1 * row(r1) + b2 * row(r2)
  */

  M2_bool IM2_MutableMatrix_sort_columns(MutableMatrix *M, int lo, int hi); /* connected to rawSortColumns2, OK */
  /* Returns false if M is not mutable, or lo, or hi are out of range */

  M2_bool IM2_MutableMatrix_row_permute(MutableMatrix *M,
                                        int start,
                                        M2_arrayint perm); /* connected to rawPermuteRows, OK */
  /* if perm = [p0 .. pr], then row(start + i) --> row(start + pi), and
     all other rows are unchanged.  p0 .. pr should be a permutation of 0..r */

  M2_bool IM2_MutableMatrix_column_permute(MutableMatrix *M,
                                           int start,
                                           M2_arrayint perm); /* connected to rawPermuteColumns, OK */
  /* if perm = [p0 .. pr], then column(start + i) --> column(start + pi), and
     all other rows are unchanged.  p0 .. pr should be a permutation of 0..r */

  const RingElement * IM2_Matrix_dot_product(const MutableMatrix *M, int c1, int c2); /* connected to rawColumnDotProduct */
  /* Return the dot product of columns c1 and c2 of the matrix M.  If either c1 or c2 is
     out of range, 0 is returned. */

  /**
     Is the matrix implemented as a contiguous array of elements?
   */
  M2_bool rawMutableMatrixIsDense(const MutableMatrix *M);

  M2_bool IM2_MutableMatrix_is_zero(const MutableMatrix *M); /* drg: connected rawIsZero, OK */

  M2_bool IM2_MutableMatrix_is_equal(const MutableMatrix *M,
                                           const MutableMatrix *N); /* drg: connected to rawIsEqual for use with ==, not connected to '===', OK */
  /* This checks that the entries of M,N are the same */

  MutableMatrix * IM2_MutableMatrix_copy(MutableMatrix *M, M2_bool prefer_dense); /* connected to rawMutableMatrix, OK */

  M2_bool IM2_MutableMatrix_set_values(MutableMatrix *M,
                                       M2_arrayint rows,
                                       M2_arrayint cols,
                                       engine_RawRingElementArray values); /* connected to rawSetMatrixValues, OK */
  /* Given three arrays of the same length, 'rows', 'cols', 'values', set the
     corresponding values of M.  If any elements are out of range, ignore those
     triples.  If the type of ring element is not valid, or the sizes of the
     matrices do not match, return false. */

  MutableMatrix /* or null */ * IM2_MutableMatrix_submatrix(const MutableMatrix *M,
                                                    M2_arrayint rows,
                                                    M2_arrayint cols); /* drg: connected rawSubmatrix, OK */

  MutableMatrix /* or null */ * IM2_MutableMatrix_submatrix1(const MutableMatrix *M,
                                                     M2_arrayint cols); /* drg: connected rawSubmatrix, OK */


  M2_bool IM2_MutableMatrix_reduce_by_pivots(MutableMatrix *M); /* connected rawReduceByPivots */
  /* Using row and column operations, use unit pivots to reduce the matrix */
  /* A return value of false means that the computation was interrupted */

  /** return the transpose of A */
  MutableMatrix* rawMutableMatrixTranspose(MutableMatrix* A);

  /**
     returns the rank of the matrix M.  If 'rank' is not defined on this type of matrix,
     then returns -1 (and an error message is given).
   */
  long rawLinAlgRank(MutableMatrix* M);

  /** requires: M should be a square matrix.  
      If not, or if the ring has not implemented this routine,
      then null is returned (and an error message is given).
   */
  const RingElement* rawLinAlgDeterminant(MutableMatrix* A);

  MutableMatrix* rawLinAlgInverse(MutableMatrix* A);

  /** compute the row reduced echelon form of the matrix A.
      This is a matrix of the same shape as A.
      NULL, and an error, is returned if the ring is not
      equipped to compute this, or if it has not been
      implemented for that ring type yet
  */
  MutableMatrix* rawLinAlgRREF(MutableMatrix* A);

  M2_arrayintOrNull rawLinAlgRankProfile(MutableMatrix* A, M2_bool row_profile);

  MutableMatrix* rawLinAlgNullSpace(MutableMatrix* A);

  /** Returns X s.t. AX = B.  Assumptions: 
      A has the same number of rows as B. A doesn't have to be invertible or square.
      If a usage error occurs, NULL is returned and 'success' is set to 0.
      In all other cases, 'success' is set to 1.
      If AX=B has no solutions, then NULL is returned,
      otherwise a matrix X solving AX=B is returned.
  */
  MutableMatrix* rawLinAlgSolve(const MutableMatrix* A, 
                                const MutableMatrix* B,
                                int* success);

  /** Returns X s.t. AX = B.  Assumptions: 
      A is a square matrix, with the same number of rows as B.
      If a usage error occurs, NULL is returned and 'success' is set to 0.
      In all other cases, 'success' is set to 1.
      If A turns out to be not invertible, NULL is returned,
      otherwise the unique matrix X solving AX=B is returned.
  */
  MutableMatrix* rawLinAlgSolveInvertible(const MutableMatrix* A, 
                                          const MutableMatrix* B,
                                          int* success);

  /** A,B,C should be mutable matrices over the same ring, and a,b
     elements of this ring.
     C = b*C + a * op(A)*op(B),
     where op(A) = A or transpose(A), depending on transposeA
     where op(B) = B or transpose(B), depending on transposeB
  */ 

  /** set C += A*B.  If not implemented, or sizes/rings are not compatible
      then false is returned.  Otherwise true is returned.
  */
  M2_bool rawLinAlgAddMult(MutableMatrix* C,
                        const MutableMatrix* A,
                        const MutableMatrix* B);

  /** set C -= A*B.  If not implemented, or sizes/rings are not compatible
      then false is returned.  Otherwise true is returned.
  */
  M2_bool rawLinAlgSubMult(MutableMatrix* C,
                        const MutableMatrix* A,
                        const MutableMatrix* B);

  /* return A*B, where A,B are mutable matrices, over same ring, same density type.
   */
  MutableMatrix* /* or null */ rawLinAlgMult(const MutableMatrix* A,
                                             const MutableMatrix* B);


  engine_RawRingElementArrayOrNull rawLinAlgCharPoly(MutableMatrix* A);
  // returns an array whose coefficients give the characteristic polynomial of the square matrix A

  engine_RawRingElementArrayOrNull rawLinAlgMinPoly(MutableMatrix* A);
  // returns an array whose coefficients give the minimal polynomial of the square matrix A



#if 0
  RingElement *rawFFPackDeterminant(MutableMatrix *M);
  /* connected to rawFFPackDeterminant, MES */
  /* requires: M should be a square matrix over a prime finite field */

  size_t rawFFPackRank(MutableMatrix *M);
  /* connected to rawFFPackRank, MES */
  /* requires: M should be a matrix over a prime finite field */

  MutableMatrix /* Or Null */ * rawFFPackNullSpace(MutableMatrix *M, M2_bool right_side);
  /* connected to rawFFPackNullSpace, MES */
  /* requires: M should be a matrix over a prime finite field */
  /* computes either left or right nullspace */

  MutableMatrix /* or null */ * rawFFPackSolve(MutableMatrix *A, MutableMatrix *B, M2_bool right_side);
  /* connected to rawFFPackSolve, MES */
  /* requires: M should be a matrix over a prime finite field */
  /* returns solution of AX=B or XA=B, depending on right_side */

  MutableMatrix /* or null */ *rawFFPackInvert(MutableMatrix *M);
  /* connected to rawFFPackInvert, MES */
  /* requires: M should be a square matrix over a prime finite field */

  MutableMatrix /* or null */ *rawFFPackAddMultipleTo(MutableMatrix *C,
                                                      const MutableMatrix *A,
                                                      const MutableMatrix *B,
                                                      M2_bool transposeA,
                                                      M2_bool transposeB,
                                                      const RingElement *a,
                                                      const RingElement *b);
  /* A,B,C should be mutable matrices over a finite prime field, and a,b
     elements of this field.
     C = b*C + a * op(A)*op(B),
     where op(A) = A or transpose(A), depending on transposeA
     where op(B) = B or transpose(B), depending on transposeB
     connected to rawFFPackAddMultipleTo, MES
  */

  M2_arrayintOrNull rawFFPackRowRankProfile(MutableMatrix *A);
  /* connected, MES */

  M2_arrayintOrNull rawFFPackColumnRankProfile(MutableMatrix *A);
  /* connected, MES */
#endif

  engine_RawArrayIntPairOrNull rawLQUPFactorization(MutableMatrix *A);

  /***************************************************
   ***** Lapack routines for dense mutable matrices **
   ***************************************************/

  /* Each of the following routines accepts honest MutableMatrix arguments,
     and returns false if there is an error.  The return values are placed into
     some of the (already existing) parameters of the routine */

  M2_arrayintOrNull rawLU(const MutableMatrix *A,
                           MutableMatrix *L,
                           MutableMatrix *U); /* connected */
  /* Returns the permutation array: we need to be more precise which one.
     A encodes both the L and the U part, as in lapack.
   */

  /**
     Hi, this is rawLUincremental.

     Returns the permutation array: we need to be more precise which one.
     Given (first m columns of PLU = first m columns of A (not given), and
     given the (m+1)-st column of A, then returns a Q, modified from P,
     and changes LU (which encodes L and U)
     s.t.  (first (m+1) columns of QLU = first (m+1) columns of A (not given)
     Note: LU encodes L and U in the usual manner: lower triangular part is L,
     diagonal of L is all ones, and U is the upper triangular part.
  */
  M2_arrayintOrNull rawLUincremental(M2_arrayintOrNull P, /* constant */
                                     MutableMatrix *LU, /* modified in routine */
                                     const MutableMatrix *v, /* constant */
                                     int m);

  void rawTriangularSolve(MutableMatrix *Lv, /* modified in routine */
                          MutableMatrix *x, /* modified in routine */
                          int m,
                          int strategy);

  M2_bool rawEigenvalues(MutableMatrix *A,
                         MutableMatrix *eigenvalues,
                         M2_bool isHermitian); /* connected */
  /*
   */

  M2_bool rawEigenvectors(MutableMatrix *A,
                          MutableMatrix *eigenvalues,
                          MutableMatrix *eigenvectors,
                          M2_bool isHermitian); /* connected */
  /*
   */

  M2_bool rawSVD(MutableMatrix *A,
                 MutableMatrix *Sigma,
                 MutableMatrix *U,
                 MutableMatrix *VT,
                 M2_bool use_divide_and_conquer); /* connected */
  /*
   */

  M2_bool rawLeastSquares(MutableMatrix *A,
                          MutableMatrix *b,
                          MutableMatrix *x, /* return value: argument modified */
                          M2_bool assume_full_rank); /* connected */
  /* Case 1: A is a dense matrix over RR.  Then so are b,x.
     Case 2: A is a dense matrix over CC.  Then so are b,x. */

  M2_bool rawQR(const MutableMatrix* A, /* input m x n matrix */
                MutableMatrix* Q, /* output m x n orthonormal columns matrix */
                MutableMatrix* R, /* output R matrix: upper triangular, nonsingular if A has ker A = 0 */
                M2_bool return_QR); /* if false, the output is instead the lapack encoded Householder transformations */
  /* if return_QR is false, then Q will contain the encoded Householder reflections
     and the multipliers tau_i will appear in R.
     MES TODO: be more specific here, once we know the exact format!
  */

  /**************************************************/
  /**** Mutable Complex routines ********************/
  /**************************************************/

  M2_string rawMutableComplexToString(const MutableComplex *M);

  unsigned int  rawMutableComplexHash(const MutableComplex *M);

  MutableComplex* rawMutableComplex(const engine_RawMutableMatrixArray M);

  M2_arrayint rawPruneBetti(MutableComplex* C, int n, int f);

  MutableComplex* rawPruneComplex(MutableComplex* C, int n, int f);

  engine_RawMutableMatrixArray rawPruningMorphism(MutableComplex* C, int n, int f);

  /**************************************************/
  /**** Local Ring routines *************************/
  /**************************************************/

  Matrix * rawLiftLocalMatrix(const Ring * R, const Matrix *m);
  M2_bool  rawIsLocalUnit(const RingElement *f);

  /**************************************************/
  /**** Monomial ideal routines *********************/
  /**************************************************/

  /* A MonomialIdeal is an immutable object, having a base ring.
     The base ring should be a polynomial ring or quotient of one.
     In case a quotient is given, the monomial ideal is considered
     to be in the commutative quotient ring whose quotient elements
     are the lead terms of the quotient polynomials.
     Each monomial ideal is represented by its minimal generators only */

  engine_RawMonomialIdealOrNull IM2_MonomialIdeal_make(const Matrix *m, int n); /* drg: connected rawMonomialIdeal*/
  /* Given a matrix 'm' over an allowed base ring (as above), create the
     monomial ideal consisting of all of the lead monomials of the columns
     of 'm' which have their lead term in row 'n'.  If 'n' is out of range,
     or the ring is not allowed, NULL is returned. */

  const Matrix /* or null */ *IM2_MonomialIdeal_to_matrix(const MonomialIdeal *I); /* drg: connected rawMonomialIdealToMatrix */
  /* Return a one row matrix over the base ring of I consisting
     of the monomials in I */

  M2_string IM2_MonomialIdeal_to_string(const MonomialIdeal *I); /* TODO */

  unsigned int rawMonomialIdealHash(const MonomialIdeal *I); 
  /* connected to 'hash', sequential, as it is mutable */

  int IM2_MonomialIdeal_is_equal(const MonomialIdeal *I1,
                                     const MonomialIdeal *I2); /* drg: connected === */
        // 1 = true, 0 = false, -1 = error

  int IM2_MonomialIdeal_n_gens(const MonomialIdeal *I); /* drg: connected rawNumgens*/
  /* Returns the number of minimal generators of I */


  const MonomialIdeal /* or null */ *rawRadicalMonomialIdeal(const MonomialIdeal *I); /* drg: connected rawRadicalMonomialIdeal*/
  /* The radical of the monomial ideal, that is, the monomial ideal
     generated by the square-free parts of the each monomial of I. */

  const MonomialIdeal /* or null */ *IM2_MonomialIdeal_intersect(const MonomialIdeal *I,
                                                         const MonomialIdeal *J); /* drg: connected rawIntersect*/

  const MonomialIdeal /* or null */ *rawColonMonomialIdeal1(const MonomialIdeal *I,
                                                   const Monomial *a); /* drg: connected rawColon*/
  /* If I = (m1, ..., mr),
     Form the monomial ideal (I : a) = (m1:a, ..., mr:a) */

  const MonomialIdeal /* or null */ *rawColonMonomialIdeal2(const MonomialIdeal *I,
                                                      const MonomialIdeal *J); /* drg: connected rawColon*/
  /* Form the monomial ideal (I : J) = intersect(I:m1, ..., I:mr),
     where J = (m1,...,mr) */

  const MonomialIdeal /* or null */ *rawSaturateMonomialIdeal1(const MonomialIdeal *I,
                                              const Monomial *a); /* drg: connected rawSaturateMonomialIdeal*/
  /* Form I:a^\infty.  IE, set every variable which occurs in 'a' to '1' in
     every generator of I. */

  const MonomialIdeal /* or null */ *rawSaturateMonomialIdeal2(const MonomialIdeal *I,
                                                   const MonomialIdeal *J); /* drg: connected rawSaturateMonomialIdeal*/
  /* Form (I : J^\infty) = intersect(I:m1^\infty, ..., I:mr^\infty),
     where J = (m1,...,mr). */

  const MonomialIdeal /* or null */ *IM2_MonomialIdeal_borel(const MonomialIdeal *I); /* drg: connected rawStronglyStableClosure*/
  /* This should really be named: ..._strongly_stable.
     Form the smallest monomial ideal J containing I which is strongly stable,
     that is that:
     If m is in J, then p_ij(m) is in J,
     where p_ij(m) = x_j * (m/x_i), for j <= i, s.t. x_i | m. (Here the
     variables in the ring are x1, ..., xn */

  M2_bool IM2_MonomialIdeal_is_borel(const MonomialIdeal *I); /* drg: connected rawIsStronglyStable*/
  /* This should really be named: ..._is_strongly_stable.
     Determine if I is strongly stable (see IM2_MonomialIdeal_borel for the
     definition of strongly stable */

  int IM2_MonomialIdeal_codim(const MonomialIdeal *I); /* drg: connected rawCodimension*/
  /* Return the codimension of I IN THE AMBIENT POLYNOMIAL RING. */

  const MonomialIdeal /* or null */ *rawMonomialMinimalPrimes(const MonomialIdeal *I,
                                                int codim_limit,
                                                int count); /* drg: connected */
  /* RENAME THIS ROUTINE */
  /* Return a monomial ideal whose generators correspond to the
     minimal primes of I of codim <= codim_limit.  If a minimal prime
     of I has the form (x_i1, ..., x_ir), then the corresponding monomial
     is x_i1 ... x_ir, i.e. the support of
     the monomial generates the monomial minimal prime.
     If 'count' is positive, only collect this number.
  */

  const MonomialIdeal /* or null */ *rawMaximalIndependentSets(const MonomialIdeal *I,
                                                 int count);
  /* drg: connected rawMaximalIndependentSets */
  /* Returns a monomial ideal where each generator encodes a maximal independent set
     of variables of I.  If 'count' is positive, only collect this number.
     A maximal independent set is encoded as a squarefree monomial of the product
     of all of the independent variables in the set. */

  const RingElement /* or null */ * IM2_MonomialIdeal_Hilbert(const MonomialIdeal *I); /* connected to rawHilbert */
  /* This routine computes the numerator of the Hilbert series
     for coker I.  NULL is returned if the ring is not appropriate for
     computing Hilbert series, or the computation was interrupted. */

  M2_arrayint rawMonomialIdealLCM(const MonomialIdeal *I); /* connected, same name */

  const MonomialIdeal /* or null */ *rawAlexanderDual(const MonomialIdeal *I, const M2_arrayint top, int strategy); /* connected, same name */
  /* 0 is the default, 1 is an alternate strategy */

  /**************************************************/
  /**** Groebner basis and resolution routines ******/
  /**************************************************/

  Computation /* or null */* IM2_Computation_set_stop(Computation *G,
                                     M2_bool always_stop,       /* 1 */
                                     M2_arrayint degree_limit,  /* 2*/
                                     int basis_element_limit,   /* 3 */
                                     int syzygy_limit,          /* 4 */
                                     int pair_limit,            /* 5 */
                                     int codim_limit,           /* 6 */
                                     int subring_limit,         /* 7 */
                                     M2_bool just_min_gens,     /* 8 */
                                     M2_arrayint length_limit   /* 9 */  /* not for GB */
                                     ); /* drg: connected rawGBSetStop */

  /* Each of these routines can return NULL, because of errors */

  Computation /* or null */ *rawStartComputation(Computation *G);
  /* start or continue the computation */

  enum ComputationStatusCode rawStatus1(Computation *C);

  int rawStatus2(Computation *C);
  /* The computation is complete up to and including this degree.
     The exact meaning of 'degree' is computation specific */

  M2_string IM2_GB_to_string(Computation *C); /* drg: connected, in actors4.d */

  unsigned int rawComputationHash(const Computation *C); /* drg: connected, in basic.d */

  void rawShowComputation(const Computation *C); /* Dan: connected to rawShowComputation */

  /*******************************************
   * Computation routines for Groebner bases *
   *******************************************/

  /* 
     routine to compute a Groebner basis of an ideal in a polynomial ring
     over a finite prime field.  Interfaces to mathicgb.
     reducer: 0 is ClassicReducer, 1 is MatrixReducer
   */
  const Matrix* /* or null */ rawMGB(const Matrix* input, 
                                     int reducer,
                                     int spairGroupSize,
                                     int nthreads,
                                     const M2_string logging
                                     ); /* connected: rawMGB */

  Computation /* or null */ *IM2_GB_make(const Matrix *m,
                                 M2_bool collect_syz,
                                 int n_rows_to_keep,
                                 M2_arrayint gb_weights,
                                 M2_bool use_max_degree,
                                 int max_degree,
                                 int algorithm,
                                 int strategy,
                                 int max_reduction_count); /* drg: connected rawGB */

  Computation /* or null */ *IM2_GB_force(const Matrix *m,
                                  const Matrix *gb,
                                  const Matrix *change,
                                  const Matrix *syz); /* drg: connected rawGBForce */

  Computation /* or null */ *rawMarkedGB(const Matrix *leadterms,
                                 const Matrix *m,
                                 const Matrix *gb,
                                 const Matrix *change,
                                 const Matrix *syz); /* mes: connected rawMarkedGB */

  Computation /* or null */ *rawGroebnerWalk(const Matrix *gb,
                                     const MonomialOrdering *order1);
  /* Create a GB algorithm which will compute using the generic Groebner walk algorithm
     Input: gb: a matrix which, under order1, would be a Groebner basis, except that
                'gb' is a matrix over a polynomial ring whose order is 'order2'.
            order1: a monomial ordering
     Output: a Groebner basis computation object which will compute a GB of gb wrt
            order2, using the Geneeric Groebner Walk algorithm of ...
     Assumptions: the base ring is a polynomial ring over a field, with NO quotient elements
  */

  Computation /* or null */ *IM2_GB_set_hilbert_function(Computation *G,
                                                 const RingElement *h); /* drg: connected rawGBSetHilbertFunction */


  const Matrix /* or null */ *rawGBGetMatrix(Computation *C);
  /* Get the minimal, auto-reduced GB of a GB computation.
     Each call to this may produce a different raw matrix */

  const Matrix /* or null */ *rawGBGetLeadTerms(Computation *G, int nparts);

  const Matrix /* or null */ *rawGBGetParallelLeadTerms(Computation *C, M2_arrayint w);

  const Matrix /* or null */ *rawGBMinimalGenerators(Computation *C);
  /* Yields a matrix whose columns form a minimal generating set
     for the ideal or submodule, as computed so far.  In the
     inhomogeneous case, this yields a generating set which is
     sometimes smaller than the entire Groebner basis. */

  const Matrix /* or null */ *rawGBChangeOfBasis(Computation *C);
  /* Yields the change of basis matrix from the Groebner basis to
     the original generators, at least if n_rows_to_keep was set
     when creating the GB computation.  This matrix, after the
     computation has run to completion, should satisfy:
     (original matrix) = (GB matrix) * (change of basis matrix). */

  const Matrix /* or null */ *rawGBSyzygies(Computation *C);
  /* Yields a matrix containing the syzygies computed so far
     via the GB computation C, assuming that 'collect_syz' was
     set when the computation was created.  If 'n_rows_to_keep' was
     set to a non-negative integer, then only that many rows of each
     syzygy are kept. */

  const Matrix /* or null */ *rawGBMatrixRemainder(Computation *G,
                                           const Matrix *m); /* drg: connected rawGBMatrixRemainder */

  M2_bool IM2_GB_matrix_lift(Computation *G,
                          const Matrix *m,
                          const Matrix /* or null */ **result_remainder,
                          const Matrix /* or null */ **result_quotient
                          ); /* drg: connected rawGBMatrixLift */
  /* false is returned if there is an error or if the remainder is NON-zero */

  int IM2_GB_contains(Computation *G,
                      const Matrix *m); /* drg: connected rawGBContains */


  /*******************************************
   * Computation routines for Resolutions ****
   *******************************************/

  /* LongPolynomial, Sort, Primary, Inhomogeneous, Homogeneous */
  /* Res: SortStrategy, 0, 1, 2, 3 ?? */

  Computation /* or null */ *IM2_res_make(const Matrix *m,
                                  M2_bool resolve_cokernel,
                                  int max_level,
                                  M2_bool use_max_slanted_degree,
                                  int max_slanted_degree,
                                  int algorithm,
                                  int strategy /* drg: connected rawResolution */
                                  );

  const Matrix /* or null */ *rawResolutionGetMatrix(Computation *G,int level);
  /* rawResolutionGetMatrix */

  MutableMatrix /* or null */ *rawResolutionGetMatrix2(Computation *G,int level,int degree);
  /* rawResolutionGetMatrix2 */

  // This might be temporary!
  MutableMatrix /* or null */ *
  rawResolutionGetMutableMatrixB(Computation *C,
                                 const Ring* R, // A polynomial ring with coeffs = RR, or a finite field used in C, same monoid as C's ring.
                                 int level);

  // This might be temporary!
  MutableMatrix /* or null */ *
  rawResolutionGetMutableMatrix2B(Computation *C,
                           const Ring* KK, // should be RR, or a finite field used in C.
                           int level,
                           int degree);

  const FreeModule /* or null */ *rawResolutionGetFree(Computation *G, int level);
    /*drg: connected rawResolutionGetFree*/

  M2_arrayint rawResolutionBetti(Computation *G,
                                 int type); /* drg: connected rawGBBetti */
  /* type:
         0: minimal betti numbers, (for FastNonminimal=>true, the ACTUAL betti numbers)
         1: non-minimal betti numbers (skeleton size, or size of GB's).
           (for FastNonminimal=>true, same as "0" case)
         2: number of S-pairs remaining to consider
         3: number of monomials in polynomials at this slot
         4: for FastNonminimal=>true resolutions, the minimal betti numbers
            other cases, this is an error.
     Not all of these may be accessible with all algorithms.  If not available,
     A betti diagram with all -1's is displayed.
  */

  /* I don't know what this is supposed to do (mike) */
  int IM2_Resolution_status(Computation *G,
                    int * complete_up_through_this_degree,
                    int * complete_up_through_this_level); /* drg: TODO */
  /* -1: error condition, and the error message is set.
     0: not made, and in fact it won't ever be done...
     1: not started,
     2: started,
     3: stopped because of a stopping condition
     4: finished the computation completely
  */

  enum ComputationStatusCode IM2_Resolution_status_level(Computation *G,
                                                         int level,
                                                         M2_bool minimize,
                                                         int * complete_up_through_this_degree);
  /* WARNING: 'minimize' is completely ignored, and should be removed from the interface */
  /* drg: connected rawResolutionStatusLevel */

  M2_arrayint rawMinimalBetti(Computation *G,
                              M2_arrayint slanted_degree_limit,
                              M2_arrayint length_limit); /* connectd: rawMinimialBetti */
  
  /****************************************************/
  /**** Chinese remainder and rational reconstruction */
  /****************************************************/

  const RingElement * rawRingElementCRA(const RingElement *f,
                                        const RingElement *g,
                                        mpz_srcptr m,
                                        mpz_srcptr n);

  const Matrix * rawMatrixCRA(const Matrix *f,
                              const Matrix *g,
                              mpz_srcptr m,
                              mpz_srcptr n);

  const RingElement * rawRingElementRatConversion(const RingElement *f,
                                  mpz_srcptr m,
                                  const Ring *RQ);

  // f should be an element in the polynomial ring R (over ZZ).
  // RQ should be the same ring as R, but with rational coefficients

  const Matrix * rawMatrixRatConversion(const Matrix *f,
                                        mpz_srcptr m,
                                        const Ring *RQ);
  // f should be a matrix in the polynomial ring R (over ZZ).
  // RQ should be the same ring as R, but with rational coefficients



  /**************************************************/
  /**** Fraction free LU decomposition **************/
  /**************************************************/

  M2_arrayintOrNull IM2_FF_LU(MutableMatrix *M); /* connected to rawFFLU */
  /* Replace M by a column echelon form.  No fractions are generated, but the
     base ring should be a domain.
     If M has a column change of basis matrix attached, it will be modified accordingly.
  */

  /**************************************************/
  /**** LLL bases ***********************************/
  /**************************************************/

  M2_bool rawLLL(MutableMatrix *M,
                 MutableMatrix /* or null */ *U,
                 gmp_QQ threshold,
                 int strategy); /* DAN: connected to rawLLL */
  /* Given a mutable matrix M over ZZ, and a rational number threshold, 1/4 < threshold <= 1,
     modify M so that the columns form a Lenstra-Lenstra-Lovasz
     basis of the image of (the original) M.  ASSUMPTION: (strategy=0 case)
     the columns of M are already a a basis for the
     lattice.  The algorithm used is that in Cohen's book on computational algebraic number
     theory, BUT: beware of the typos in the algorithm!
     If there is any error (interupted, M or threshold not the correct kind), then false
     is returned, and LLL is set to 0.
     If M has a column change of basis matrix attached, it will be modified accordingly.

     strategy: 0 means use original Macaulay2 engine routine.
               2 means use NTL LLL
               (strategy%3) == 3 means use one of the real number variants:
               GramSchmidt or Givens: 0 or 4 (respectively)
               LLL or BKZ: 0 or 8 (respectively)
               FP, QP1, QP, XD, RR (respectively: 0, 16, 2*16, 3*16, 4*16
               Thus: strategy 3+4+8+16 means use NTL's GBKZ_QP1.

     For the RR variants, the suggested value of the threshold is 99/100.

     If U is not NULL, then it should be an m by m matrix over ZZ, where m is the number
     of columns of the matrix M.  In this case, it is set to be the invertible transform
     matrix such that Mold * U = Mnew.
  */

  M2_bool IM2_SmithNormalForm(MutableMatrix *M); /* connected rawSmithNormalForm */
  /* Given a mutable matrix over ZZ, compute the Smith normal form for M. (replaces
     M with this normal form.
     Currently the algorithm used makes computing the change of basis matrices
     difficult (we use mod D arithmetic, where D is the determinant).
     If there is an error, then an error is flagged and false is returned.
  */

  M2_bool IM2_HermiteNormalForm(MutableMatrix *M); /* connect rawHermiteNormalForm */
  /* Given a mutable matrix over ZZ, compute the Hermite normal form for M. (replaces
     M with this normal form.
     Currently the algorithm used makes computing the change of basis matrices
     difficult (we use mod D arithmetic, where D is the determinant).
     If there is an error, then an error is flagged and false is returned.
  */

  /**************************************************/
  /**** Specialized operations **********************/
  /**************************************************/

  Matrix /* or null */ * rawSubduction(int numparts,
                                       const Matrix *M,
                               const RingMap *F,
                               Computation *C);
  /*
    Perform a subalgebra reduction of the entries of the one row matrix M.
    C should be a GB computed in high enough degree to handle the elements of M,
      of an ideal of the form y_i - m_i (m_i is the lead monomial of f_i).
    F should be a ring map R --> R, sending y_i to f_i.
    M should be a matrix over the ring R, usually only involving the variables in the f_i.
    R should be a ring containing the variables of the f_i, and the variables y_i,
      with a monomial order eliminating the first set of variables (which is 'numparts' parts of the
      monomial ordering).
    numparts: number of parts in the monomial order of the original ring.
   The resulting matrix will have no monomials which are in the subalgebra
   generated by the monomials m_i, and each entry of M and the corresponding entry of the
   result differ by an element of the subalgebra generated by the f_i.
   */

  M2_bool rawIdealOfPoints(const Ring *R,
                      const MutableMatrix *Pts,
                      Matrix /* or null */ ** result_GB,
                      Matrix /* or null */ ** result_std_monoms);
  /* Returns false if an error occured.
     Input: R: a polynomial ring of the form K[x1,...,xn]
            Pts: an n by d matrix over K.
     Action: Compute the ideal of the points in n-space
             given by the columns of 'Pts'
     Output: result_GB: the GB of this ideal
             result_std_monoms: the standard monomials (1 by d matrix)
     Question: should this return the separators as well?
  */

  /**************************************************/
  /**** Special routines for objects over RRR,CCC ***/
  /**************************************************/

  /* The code for these is in x-mutablemat.cpp */

  /* These routines set any real or complex numbers whose absolute value is less than
     epsilon.  If the ring is not over RRR or CCC, then an error message is given, and NULL
     is returned. */

  const Matrix /* or null */ *rawMatrixClean(gmp_RR epsilon, const Matrix *M);
  const RingElement /* or null */ *rawRingElementClean(gmp_RR epsilon, const RingElement *f);
  MutableMatrix /* or null */ *rawMutableMatrixClean(gmp_RR epsilon, MutableMatrix *M); /* modifies M in place */

  /* p is currently limited to infinity (with a given precision), and this routine
     returns the maximum norm of any RRR or CCC coefficient in the object.
     If the ring is not over RRR or CCC, then an error message is given, and NULL
     is returned */

  gmp_RRorNull rawMatrixNorm(gmp_RR p, const Matrix *M);
  gmp_RRorNull rawRingElementNorm(gmp_RR p, const RingElement *f);
  gmp_RRorNull rawMutableMatrixNorm(gmp_RR p, const MutableMatrix *M);

  const Matrix /* or null */ *rawGbBoolean(const Matrix *m);
  const Matrix /* or null */ *rawBIBasis(const Matrix* m, int toGroebner);

#if defined(__cplusplus)
}
#endif

#endif

/*
// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e "
// indent-tabs-mode: nil
// End:
*/
