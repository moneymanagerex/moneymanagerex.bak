//%PRINT OFF
#ifndef MONEY             /* Inhibit repeated inclusion                   */
#include <math.h>         /* For fmod, modf, floor, log10                 */
#include <iostream.h>
#ifndef DOUBLE
   #define DOUBLE const double
#endif
//%PRINT ON
//%TITLE Money Class (freeware version)  (copyright 1994, Information Disciplines, Inc)
//  This special limited version of IDI's Money class is packaged for 
//  unrestricted  distribution.  Unlike the full version, it:

//  -  is self sufficient, not depending on any macros, functions, or other 
//     classes in IDI's library (but it uses standard C++ library routines).

//  -  supports only a single (but customizable) internal representation.
//%SPACE 2
//  User organizations may use it freely in applications they develop for
//  their own internal use.  Contact IDI at (773) 624-3670:

//     - for permission to incorporate all or part of this class or its
//       related functions into commercial software products, commercial
//       courses, or published material, or

//     - for information about the full version or other IDI classes.
//%SPACE 3
#define MONEY const Money /* Conventional keyword for constant declaration */
class   Money {
//%SPACE 3
//   Internal representation:   Always an integer, scaled so that unity is
//   -----------------------      the smallest measurable quantity

#ifndef   MoneyIType       /*  User may override with #define before      */
  #define MoneyIType double/*  #include, e.g. if a class is available     */
#endif                     /*  for multi-word integers or packed decimal  */
			   /*  (However, some constructors and accessors  */
			   /*     are limited to double float precision)  */
public:
static  MoneyIType scale;   //  Smallest fraction of monetary unit
			    //    represented e.g. 100 = cents, 1000 = mills
			    //    (User may override when no Money
			    //       items are instantiated.)                            

private:                    //  Internal integer value =
	MoneyIType value;   //    amount in monetary units * scale
//%SPACE 3
//   External representation: Constants used in output and input functions
//   ----------------------- (initialized in MONEYFW.CPP -- user can override)
public:
static char   pfx_symbol[];   // Leading currency symbol (U.S.: '$')
static char   sfx_symbol[];   // Trailing currency symbol
static char   decimal_point;  // Character for 100ths  (U.S.:  period)
static char   group_separator;// Character for 1000nds (U.S.:  comma)
static char   unit_name[];    // Name of monetary unit (U.S.:  "dollar")
static char   cent_name[];    // Name of fraction unit (U.S.:  "cent")
//%EJECT
private: static double round(DOUBLE x); // (in MONEYFW.CPP)
public:
//%SPACE 3
// Constructors:  To support literal constants, we allow conversion from
// ------------   float.  This inhibits detection of some mixed expressions.

   Money(DOUBLE x) : value(round(x * scale)) {}
   Money() {}                 //  Default constructor, for efficiency

//  The compiler will supply appropriate versions of:
//    -  the destructor,
//    -  the copy constructor,
//    -  the assignment operator.
//%SPACE 3
//  Accessor functions to separate whole and fractional parts:
//  ---------------------------------------------------------
public:
short      MoneyCents()   const
     {double dummy; return short(modf((value + (value < 0 ? -.5 :.5))
					      / scale, &dummy) * 100);}
MoneyIType MoneyInt()     const
     {double dummy; return modf(double(value) / scale, &dummy),dummy;}
//%SPACE 3
//  Primitive arithmetic member operators:
//  -------------------------------------

//  For efficiency we follow Scott Myers ("More Effective C++", Addison
//  Wesley) in defining the compound assignment operators as primitive.

 Money& operator+= (MONEY        rs) {value += rs.value;      return *this;}
 Money& operator-= (MONEY        rs) {value -= rs.value;      return *this;}
 Money& operator*= (DOUBLE       rs) {value =round(value*rs); return *this;}
 Money& operator/= (DOUBLE       rs) {value =round(value/rs); return *this;}

 Money  operator- ()   const
		   {Money  result;  result.value = - value;  return result;}
 Money  operator+ ()   const              {return *this; }
//%EJECT
//  Additional arithmetic member operators:
//  --------------------------------------
//     (mostly defined in terms of the primitive ones)

 Money  operator+ (MONEY  rs)        const {return Money(*this) += rs;}
 Money  operator- (MONEY  rs)        const {return Money(*this) -= rs;}
 Money  operator* (DOUBLE rs)        const {return Money(*this) *= rs;}
 Money  operator/ (DOUBLE rs)        const {return Money(*this) /= rs;}
double  operator/ (MONEY rs)         const {return value / rs.value;}

 Money& operator++()                {value += scale; return *this;}
 Money& operator--()                {value -= scale; return *this;}
 Money  operator++(int)             {++*this; return *this-1;}
 Money  operator--(int)             {--*this; return *this+1;}
//%SPACE 3
//  Relational member operators:
//  ---------------------------

 bool operator== (MONEY rs)   const  {return value == rs.value;}
 bool operator<  (MONEY rs)   const  {return value <  rs.value;}
 bool operator<= (MONEY rs)   const  {return value <= rs.value;}
 bool operator!= (MONEY rs)   const  {return value != rs.value;}
 bool operator>  (MONEY rs)   const  {return value >  rs.value;}
 bool operator>= (MONEY rs)   const  {return value >= rs.value;}

//  For efficiency the following redundant relational operators
//  avoid creating temporary Money objects and invoking constructors:

 bool operator== (DOUBLE rs)  const  {return value == rs * scale;}
 bool operator<  (DOUBLE rs)  const  {return value <  rs * scale;}
 bool operator<= (DOUBLE rs)  const  {return value <= rs * scale;}
 bool operator!= (DOUBLE rs)  const  {return value != rs * scale;}
 bool operator>  (DOUBLE rs)  const  {return value >  rs * scale;}
 bool operator>= (DOUBLE rs)  const  {return value >= rs * scale;}
};                               //  ********  End of class definition
//%SPACE 2
//  Additional (non-member) Money operators
//  ---------------------------------------

inline Money  operator+  (DOUBLE ls, MONEY  rs) {return rs + ls;}
inline Money  operator*  (DOUBLE ls, MONEY  rs) {return rs * ls;}
inline Money  operator-  (DOUBLE ls, MONEY  rs) {return Money(rs)-ls;}

inline bool  operator== (DOUBLE ls, MONEY rs)  {return rs == ls;}
inline bool  operator<  (DOUBLE ls, MONEY rs)  {return rs >  ls;}
inline bool  operator<= (DOUBLE ls, MONEY rs)  {return rs >= ls;}
inline bool  operator!= (DOUBLE ls, MONEY rs)  {return rs != ls;}
inline bool  operator>  (DOUBLE ls, MONEY rs)  {return rs <  ls;}
inline bool  operator>= (DOUBLE ls, MONEY rs)  {return rs <= ls;}

 ostream&  operator<< (ostream&  ls, MONEY rs); // (in MONEYFW.CPP)
//%PRINT OFF
#endif
//%PRINT ON
