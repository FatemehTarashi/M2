-------------------------------------------------------------------------------
-- licensed under GPL v2 or any later version
-------------------------------------------------------------------------------
newPackage(
    "RationalPoints2",
    Version => "0.4",
    Date => "Feb 3, 2021",
    Authors => { {Name => "Jieao Song", Email => "jieao.song@imj-prg.fr"} },
    Headline => "find the rational points on a variety",
    Keywords => {"Commutative Algebra"},
    DebuggingMode => false
)
-------------------------------------------------------------------------------
-- This program takes in an ideal generated by a list of polynomials, and finds
-- all of the common zeros of the polynomials. Over a finite field the ideal
-- can be arbitrary; over a number field it must be of dimension 0.
-------------------------------------------------------------------------------
-- The program naively tests points coordinate by coordinate. There are however
-- several tricks used to improve the performance.
--
-- * At each coordinate we use elimination to compute the possible values
-- instead of enumerating over the entire field.
--
-- * If we get a prime ideal of degree 1, we solve a linear system of equations
-- to quickly get all the points contained in the corresponding linear
-- subspace.
--
-- * If the ideal is homogeneous, we will enumerate the rational points in the
-- projective space first, then reconstruct the points in the affine space;
-- this can also be done recursively.
-------------------------------------------------------------------------------
needsPackage "Elimination"
export{"rationalPoints", "Amount"}
-------------------------------------------------------------------------------
-- els: a list of elements in the finite field k
local els;
-- the options
local amount;
local projective;
local verbose;
-- listNonSplitPols: a list of non-splitting polynomials, used in verbose mode
local listNonSplitPols;
-------------------------------------------------------------------------------
-- Takes in a finite field and returns the list of its elements
--
listFieldElements = (k) -> (
    if isFinitePrimeField k then return apply(char k, i->i*1_k);
    k0 := k;
    -- if k is constructed using `toField`
    if not instance(k, GaloisField) then (
        try k = GF baseRing k
        else error "the coefficient ring is not a field";
    );
    x := k.PrimitiveElement;
    p := k.char;
    d := k.degree;
    try return ({0_k} | (for e in (1..<p^d) list x^e*1_k)) / (x->sub(x, k0))
    else error "the field is too big";
);
-------------------------------------------------------------------------------
-- Takes a number n and a list
-- Produces the direct product of the list with itself n times
-- Same as lst**lst**lst**... but with flattened elements
--
pow = (n, lst) -> (
    if n < 0 then error "power of a list is not defined for n negative";
    result := {{}};
    for i in (0..<n) do result = flatten \ (result ** lst);
    return result;
);
-------------------------------------------------------------------------------
-- Utility function that shuffles two list u and v according to a list ind
-- of indices in 0 (for u) and 1 (for v)
--
shuffle = (ind, u, v) -> (
    (i,j) := (-1,-1);
    return for k in (0..<#ind) list if ind_k==0 then (i=i+1; u_i) else (j=j+1; v_j);
);
-------------------------------------------------------------------------------
-- Takes in a list of coefficients representing a univariate polynomial
-- Returns the list of zeros
-- E.g. {2,-3,1} -> 2-3*x+x^2 which returns {1, 2}
--
zeros := (coeffs) -> (
    k := ring first coeffs;
    (R, p, factors) := (null, null, null);
    getValue := (R, c) -> -coefficient(1_R, c) / coefficient(R_0, c);
    try ( -- try use `factor` first
        R = k(monoid[getSymbol "x"]);
        p = sum apply(#coeffs, i -> coeffs_i * R_0^i);
        factors = value \ first \ toList factor p;
    ) else ( -- over k where `factor` is not defined, we use `decompose`
        k' := baseRing k;
        R = k'(monoid[getSymbol "x"]);
        p = sum apply(#coeffs, i -> (map(k',k,gens k')) coeffs_i * R_0^i);
        factors = (c->(gens c)_(0,0)) \ decompose(ideal p, Strategy=>"Legacy");
        getValue = (x->sub(x, k)) @@ getValue;
    );
    linearFactors := select(factors, c->(first degree c == 1));
    otherFactors := select(factors, c->(first degree c > 1));
    if verbose then listNonSplitPols = listNonSplitPols | otherFactors;
    return getValue_R \ linearFactors;
);
-- Distinguish `zeros` from `mzeros`, since when using `mzeros` the
-- factorization will not be run twice, and thus will not print the
-- non-splitting polynomials in verbose mode
mzeros = memoize zeros;
-------------------------------------------------------------------------------
-- Takes in a linear ideal and returns the list of solutions
--
linearSolve := (I) -> (
    R := ring I;
    k := coefficientRing R;
    x := gens R;
    n := #x;
    system := first entries gens gb I;
    if system == {} then return pow_n els;
    -- solve a system of linear equations Ax=b
    A := matrix apply(system, f->apply(x, xi->coefficient(xi, f)));
    b := matrix apply(system, f->{-coefficient(1_R, f)});
    v := transpose solve(A, b);
    if #system == n then return {first entries v};
    M := transpose gens kernel A;
    getSolution := x -> first entries (v + matrix{toList x} * M);
    return getSolution \ pow_(n-#system) els;
);
linearSolve = memoize linearSolve;
-------------------------------------------------------------------------------
-- Utility function that normalizes a list of homogeneous coordinates so that
-- the first non-zero coordinate is 1
--
homogCoord = p -> (a := p_(position(p, x->(x!=0))); (x->x/a) \ p);
-------------------------------------------------------------------------------
-- Utility function that tests if an ideal is trivial
--
isTrivial = I -> (1_(ring I) % I == 0);
-------------------------------------------------------------------------------
-- The main function that carries out the enumeration of points
-- Takes in an ideal and enumerate the points coordinate by coordinate
-- Returns the list of points or the number of points if `Amount` is set to
-- true
--
findPoints = I -> (
    result := if amount then 0 else {};
    if isTrivial I then return result; -- trivial case
    R := ring I;
    k := coefficientRing R;
    x := gens R;
    n := #x;
    I = ideal gens gb I;
    -- use `linearSolve` if I is linear
    if degree I == 1 and (dim I == 0 or isPrime I) then (
        if amount then return (#els)^(dim I)
        else return linearSolve I;
    );
    -- use `findProjPoints` if I is homogeneous
    if isHomogeneous I then (
        result = findProjPoints I;
        if amount then return 1+(#els-1)*result
        else return {toList(n:0_k)} | join flatten((v->(for a in els_{1..<#els} list a*v)) \ result);
    );
    -- Otherwise use elimination to get the possible values when degree is
    -- small. If degree is large, elimination might not give much useful
    -- information but takes a long time to compute.
    possibleValues := els;
    if (char k == 0 or degree I < #els or verbose) then (
        elim := eliminate(x_{1..n-1},I);
        if elim != 0 then (
            g := (gens elim)_(0,0);
            coeffs := apply((degree g)_0+1, i->coefficient(x_0^i, g));
            possibleValues = (if verbose then zeros else mzeros) coeffs;
        );
    );
    if n == 1 then (
        if amount then return #possibleValues
        else return (v->{v}) \ possibleValues;
    ) else (
        R' := k(monoid[vars(0..<n-1)]);
        x = gens R';
        for v in possibleValues do (
            I' := (map(R', R, {v}|x)) I;
            if amount then result = result + findPoints I'
            else result = result | (x->{v}|x) \ findPoints I';
        );
        return result;
    );
);
-------------------------------------------------------------------------------
-- Takes in a homogeneous ideal and enumerate points in the projective space
--
findProjPoints = I -> (
    R := ring I;
    k := coefficientRing R;
    x := gens R;
    n := #x;
    result := if amount then 0 else {};
    -- find all rational points of form (1,...), (0,1,...), (0,0,1,...) etc.
    for i in (1..n) do (
        R' := k(monoid[vars(0..<n-i)]);
        x = gens R';
        v := toList((i-1):0_k) | {1_k};
        I' := (map(R', R, v|x)) I;
        if amount then result = result + findPoints I'
        else result = result | (x->v|x) \ findPoints I';
    );
    return result;
);
-------------------------------------------------------------------------------
-- Main interface
-------------------------------------------------------------------------------
-- Takes in an ideal
-- Pre-processing: trim the ideal and reorder the variables
-- Enumeration of the points
-- Post-processing: reconstruct the points according to the original order
-- Returns the list of points or the number of points if `Amount` is set to
-- true
--
rationalPoints = method(Options => {Projective=>false, Amount=>false, Verbose=>false});
rationalPoints(Ideal) := opts -> I -> (
    (amount, projective, verbose) = (opts.Amount, opts.Projective, opts.Verbose);
    if verbose then listNonSplitPols = {};
    --
    R := ring I;
    if not isPolynomialRing R then error "expect I to be an ideal of a polynomial ring";
    k := coefficientRing R;
    if not isField k then error "the coefficient ring is not a field";
    if char k > 0 then els = listFieldElements k;
    x := gens R;
    n := #x;
    I = trim I;
    result := if amount then 0 else {};
    --
    if char k == 0 then ( -- the number field case
        if (not projective and dim I > 0) or (projective and dim I > 1)
        then error "over number fields, positive dimensional ideals are not implemented";
        if projective then result = findProjPoints I
        else result = findPoints I;
    ) else ( -- the finite field case
        -- we first get rid of the unused variables; they will be added at the end
        supp := set flatten(for p in first entries gens I list support p);
        ind := apply(n, i -> if member(x_i, supp) then 1 else 0);
        unused := n - sum ind;
        if unused > 0 then (
            R = k(monoid[vars(0..<n-unused)]);
            x = gens R;
            I = (map(R, ring I, shuffle(ind, (unused:0), x))) I;
        );
        -- enumeration of points and post-processing
        if projective then ( -- projective case
            if not isHomogeneous I then error "I is not a homogeneous ideal";
            if isTrivial I then return result; -- trivial case
            result = findProjPoints I;
            if unused > 0 then ( -- reconstruction
                if amount then result = result*(#els)^unused+((#els)^unused-1)//(#els-1)
                else (
                    -- shuffle and homogenize so that the first non-zero coordinate is 1
                    result = flatten table(pow_unused els, result, homogCoord @@ shuffle_ind);
                    -- extra points lying in a projective subspace
                    PP := findProjPoints(ideal 0_(k(monoid[vars(0..<unused)])));
                    result = result | PP / (x->homogCoord shuffle(ind, x, ((n-unused):0_k)));
                );
            );
        ) else ( -- affine case
            result = findPoints I;
            if unused > 0 then ( -- reconstruction
                if amount then result = result*(#els)^unused
                else result = flatten table(pow_unused els, result, shuffle_ind);
            );
        );
    );
    if verbose and #listNonSplitPols > 0 then (
        print "-- the following polynomials do not split";
        print \ unique \\ (s->"   -- "|s) @@ toExternalString \ listNonSplitPols;
    );
    return result;
);
-------------------------------------------------------------------------------
-- The interface to specify an extension field
--
rationalPoints(Ring, Ideal) := opts -> (k', I) -> (
    if not isField k' then error "the coefficient ring is not a field";
    R := ring I;
    if not isPolynomialRing R then error "expect I to be an ideal of a polynomial ring";
    k := coefficientRing R;
    if char k != char k' then error "the coefficient field has different characteristic than the ideal";
    x := gens R;
    R' := k'(monoid[x]);
    if k === QQ or isFinitePrimeField k or (instance(k, GaloisField) and k.degree == 1) then ( -- case where the base field is prime
        I = (map(R',R,gens R')) I;
    ) else ( -- otherwise try to lift a primitive element
        p := (gens ideal baseRing k)_(0,0); -- p is the minimal polynomial
        primeField := coefficientRing ring p;
        coeffs := apply((degree p)_0+1, i->(map(k',primeField,{})) coefficient((ring p)_0^i, p));
        verbose = false;
        lifts := zeros coeffs; -- a list of possible lifts
        if (#lifts == 0) then error "cannot make the field extension";
        R0 := (baseRing k)(monoid[x]);
        I = (map(R',R0,gens R'|{lifts_0})) sub(I, R0);
    );
    return rationalPoints(I, Amount=>opts.Amount, Projective=>opts.Projective, Verbose=>opts.Verbose);
);
-------------------------------------------------------------------------------
rationalPoints(AffineVariety) := opts -> X -> (
    return rationalPoints(ideal X, Amount=>opts.Amount, Verbose=>opts.Verbose);
);
-------------------------------------------------------------------------------
rationalPoints(Ring, AffineVariety) := opts -> (k, X) -> (
    return rationalPoints(k, ideal X, Amount=>opts.Amount, Verbose=>opts.Verbose);
);
-------------------------------------------------------------------------------
rationalPoints(ProjectiveVariety) := opts -> X -> (
    return rationalPoints(ideal X, Amount=>opts.Amount, Projective=>true, Verbose=>opts.Verbose);
);
-------------------------------------------------------------------------------
rationalPoints(Ring, ProjectiveVariety) := opts -> (k, X) -> (
    return rationalPoints(k, ideal X, Amount=>opts.Amount, Projective=>true, Verbose=>opts.Verbose);
);
-------------------------------------------------------------------------------
beginDocumentation()
-------------------------------------------------------------------------------
doc ///
Key
 RationalPoints2
Headline
 Find the rational points on a variety
Description
 Text
  {\em RationalPoints2} is a package for enumerating the rational points on a
  variety defined by an ideal of a polynomial ring. Over a finite field the
  ideal can be arbitrary; over a number field the ideal must be of dimension 0.
  The coefficient field can also be specified.
Subnodes
 rationalPoints
///
-------------------------------------------------------------------------------
doc ///
Key
 rationalPoints
 (rationalPoints, Ideal)
 (rationalPoints, AffineVariety)
 (rationalPoints, ProjectiveVariety)
 (rationalPoints, Ring, Ideal)
 (rationalPoints, Ring, AffineVariety)
 (rationalPoints, Ring, ProjectiveVariety)
 Amount
 Projective
 Verbose
 [rationalPoints, Amount]
 [rationalPoints, Projective]
 [rationalPoints, Verbose]
Headline
 Find the rational points on a variety
Usage
 rationalPoints I
 rationalPoints(F, I)
Inputs
 I:Ideal
  viewed as an affine variety; @ofClass AffineVariety@ / @ofClass
  ProjectiveVariety@ can also be used
 F:Ring
  the coefficient field; if not specified then it is taken to be the field of
  definition of {\tt I}
 Amount=>Boolean
  whether to only compute the number of rational points.
 Projective=>Boolean
  whether to treat the ideal as a homogeneous ideal and consider the
  corresponding projective variety
 Verbose=>Boolean
  whether to print the polynomials computed that do not split in the
  coefficient field, useful for finding the splitting field
Outputs
 :
  Either @ofClass List@ of points, each represented by @ofClass List@ of
  coordinates, or the number of such points if {\tt Amount} is set to true
Description
 Text
  Return a list of rational points contained in the affine variety given by the
  input ideal {\tt I}. Over a finite field ({\tt ZZ/p} or {\tt GF q}), the
  ideal can have arbitrary dimension.
 Example
  R = ZZ/5[x,y,z]; I = ideal(x^3-y*z, x+y);
  rationalPoints I
  rationalPoints(Spec(R/I), Amount=>true)
  #rationalPoints_(GF 25) I
 Text
  Over a number field the ideal must be of dimension 0. Set the option {\tt
  Verbose} to {\tt true} will print the polynomials found not splitting in the
  coefficient field. Note that a numerical version is implemented in the
  package @TO "EigenSolver::EigenSolver"@.
 Example
  R = QQ[x,y]; I = ideal(x^2+y^2-1,x^3+y^3-1);
  #rationalPoints I
  rationalPoints(I, Verbose=>true)
 Text
  Here we can consider the splitting field of the polynomial by either directly
  adding a zero, or by noticing that it has discriminant -8.
 Example
  F1 = toField(QQ[a]/(2*a^2+4*a+3));
  rationalPoints_F1 I
  F2 = toField(QQ[d]/(d^2+2));
  rationalPoints_F2 I
 Text
  If a projective variety is considered, the rational points are given in
  homogeneous coordinates. The first non-zero coordinate will be normalized to
  1.
 Text
  For example we can take the twisted cubic, which is a smooth rational curve
  so it has q+1 points over a finite field of q elements.
 Example
  R = ZZ/5[x,y,z,w]; I = ideal "xz-y2,xw-yz,yw-z2";
  rationalPoints(I, Projective => true)
  #rationalPoints variety I
  #rationalPoints_(GF 25) variety I
 Text
  Another nice example is the 27 lines on the Fermat cubic surface. Note that
  only 3 of them are defined over the rational numbers.
 Example
  R = QQ[a,b,c,d]; I = ideal(a^3+b^3+c^3+d^3);
  fano = variety Fano_1 I;
  rationalPoints(fano, Verbose=>true)
 Text
  So we should consider the splitting field for these two polynomials, which is
  the cyclotomic field of order 3.
 Example
  needsPackage "Cyclotomic"; F = cyclotomicField 3;
  time rationalPoints_F fano
  #oo
 Text
  If only the number of rational points is needed, set {\tt Amount} to {\tt
  true} can sometimes speed up the computation.
 Example
  R = ZZ/101[u_0..u_10];
  f = sum toList(u_0..u_10); I = ideal f
  time rationalPoints(I, Amount => true)
 Text
  This symbol is provided by the package @TO RationalPoints2@.
///
-------------------------------------------------------------------------------
TEST ///
    R = ZZ/13[t];
    assert(#rationalPoints ideal(0_R) == 13);
    assert(#rationalPoints variety ideal(0_R) == 1);
    assert(#rationalPoints ideal(1_R) == 0);
    assert(#rationalPoints variety ideal(1_R) == 0);
    assert(#rationalPoints ideal(t-2) == 1);
    R = ZZ/5[x_1..x_4];
    I = ideal(x_2^2+x_1*x_2+1, x_1*x_2*x_3*x_4+1);
    assert(#rationalPoints I == 8);
    R = GF 9[m,n,j]; I = ideal(m+a, n*j);
    assert(rationalPoints(I,Amount => true) == 17);
    R = QQ[x]; I = ideal (x^2*(x+1)*(x^2-2));
    assert(rationalPoints(I,Amount => true) == 2);
///
-------------------------------------------------------------------------------
TEST ///
    assert(#rationalPoints Grassmannian(1,3,CoefficientRing=>ZZ/2) == 36);
    R = ZZ/101[u_0..u_10]; I = ideal sum toList(u_0..u_10);
    assert(rationalPoints(I, Amount => true) == 101^10);
    R = ZZ/101[x,y,z]; I = homogenize(ideal(y^2-x^3-x^2), z);
    assert(#rationalPoints variety I == 101);
    R = QQ[x,y]; I = ideal(x^2+y^2-1,x^3+y^3-1);
    F = toField(QQ[q]/(q^2+2));
    assert(#rationalPoints_F I == 4);
    R = GF 4[x]; I = ideal(x^3 - a);
    assert all(rationalPoints_(GF 64) I / (p -> (x:=p_0; x^6+x^3+1)), zero);
///
-------------------------------------------------------------------------------
TEST ///
    R = QQ[a,b,c,d]; I = ideal(a^3+b^3+c^3+d^3);
    needsPackage "Cyclotomic"; F = cyclotomicField 3;
    assert(#rationalPoints_F variety Fano_1 I == 27);
///
-------------------------------------------------------------------------------
TEST ///
    needsPackage "Points";
    k = ZZ/101; (m, n) = (20, 2); R = k[x_1..x_n];
    M = random(k^n, k^m);
    mul = toList(1..m) / (i->random(1, 3));
    I = ideal last affineFatPoints(M, mul, R);
    S1 = entries transpose M;
    S2 = rationalPoints I;
    assert(set S1 === set S2);
///
-------------------------------------------------------------------------------
TEST ///
    F = ZZ/23; S = F[x_0..x_9];
    sigma0 = {(0,2,5),(1,3,6),(2,4,7),(3,0,8),(4,1,9),(0,9,7),(1,5,8),(2,6,9),(3,7,5),(4,8,6)};
    delta = (x,y,v) -> table(10,10,(i,j) -> if i==x and j==y then v else 0);
    skew = (i,j,k) -> sum(delta \ {(i,j,x_k),(j,k,x_i),(k,i,x_j),(j,i,-x_k),(k,j,-x_i),(i,k,-x_j)});
    I = trim pfaffians_6 matrix sum(skew \ sigma0);
    assert(#rationalPoints variety I == 55);
///
-------------------------------------------------------------------------------
endPackage "RationalPoints2"
-------------------------------------------------------------------------------
end
installPackage("RationalPoints2", RemakeAllDocumentation=>true)
