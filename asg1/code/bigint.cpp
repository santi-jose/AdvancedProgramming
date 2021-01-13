// $Id: bigint.cpp,v 1.3 2020-10-11 12:47:51-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) { //bigint constructor
   DEBUGF ('~', this << " -> " << uvalue) //causes problems until ubigint operator<< is implemented
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_): //bigint constructor
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) { //bigint constructor with string pointer
   is_negative = that.size() > 0 and that[0] == '_'; //check for negative token '_'
   uvalue = ubigint (that.substr (is_negative ? 1 : 0)); //convert string into ubigint
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const { //sum of bigint's
   if(that.is_negative == is_negative){ //if signs are equal
      ubigint result = uvalue + that.uvalue; //add ubigint's
	  return bigint(result, is_negative); //return bigint
   }else{ //operands are of different sign
	   if(uvalue > that.uvalue){ //if uvalue has more digits than that.uvalue
		   ubigint result = uvalue - that.uvalue; //subtract that.uvalue from uvalue
		   return bigint(result, is_negative); //return bigint with result and is_negative
	   }else if(that.uvalue > uvalue){ //else if that.uvalue has more digits than uvalue
		   ubigint result = that.uvalue - uvalue; //subtract uvalue from that.uvalue
		   return bigint(result, that.is_negative); //return bigint with result and that.is_negative 
	   }else{ //else uvalue and that.uvalue are the same
		   ubigint result = that.uvalue - uvalue;
		   return bigint(result, false);
	   }
   }
}

bigint bigint::operator- (const bigint& that) const { //difference of bigint's
   ubigint result = uvalue - that.uvalue;
   return result;
}


bigint bigint::operator* (const bigint& that) const { //multipication of bigint's
   bigint result = uvalue * that.uvalue;
   return result;
}

bigint bigint::operator/ (const bigint& that) const { //quotient of bigint's
   bigint result = uvalue / that.uvalue;
   return result;
}

bigint bigint::operator% (const bigint& that) const { //modulus of bigint's
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const { //equality boolean of bigint's
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const { //less than boolean of bigint's
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}

