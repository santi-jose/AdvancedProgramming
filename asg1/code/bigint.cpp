// $Id: bigint.cpp,v 1.3 2020-10-11 12:47:51-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

//bigint constructor w/ long input
bigint::bigint (long that): uvalue (that), is_negative (that < 0) { 
   DEBUGF ('~', this << " -> " << uvalue)
}

//bigint constructor w/ uvalue & is_negative input
bigint::bigint (const ubigint& uvalue_, bool is_negative_): 
                uvalue(uvalue_), is_negative(is_negative_) {
}

//bigint constructor w/ string reference input
bigint::bigint (const string& that) { 
   //check for negative token '_'
   is_negative = that.size() > 0 and that[0] == '_'; 
   //convert string into ubigint
   uvalue = ubigint (that.substr (is_negative ? 1 : 0)); 
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

//operator+ adds two bigint's this+that
bigint bigint::operator+ (const bigint& that) const {
   if(that.is_negative == is_negative){ //if signs are equal
      ubigint result = uvalue + that.uvalue; 
      return bigint(result, is_negative); 
   }else{ //operands are of different sign
      if(uvalue > that.uvalue){ 
         ubigint result = uvalue - that.uvalue; 
         return bigint(result, is_negative); 
      }else if(that.uvalue > uvalue){
         ubigint result = that.uvalue - uvalue; 
         return bigint(result, that.is_negative);  
      }else{ //else uvalue and that.uvalue are equal
         ubigint result = that.uvalue - uvalue; //result = 0
         return bigint(result, false);
      }
   }
}

//operator- subtracts two bigint's this-that
bigint bigint::operator- (const bigint& that) const {
   if(that.is_negative == is_negative){ //if signs are equal
      if(uvalue > that.uvalue){ //if(uvalue > that.uvalue)
         ubigint result = uvalue - that.uvalue;
         return bigint(result, is_negative);
      }else if (that.uvalue > uvalue){ //if(that.uvalue > than uvalue)
         ubigint result = that.uvalue - uvalue; 
         return bigint(result, not(that.is_negative));
      }else{ //uvalue == that.uvalue
         ubigint result = uvalue - that.uvalue; //result = 0
         return bigint(result, false); 
      }
   }else{ // signs are different 
      ubigint result = uvalue + that.uvalue;
      return bigint(result, not(that.is_negative)); 
   }
}


//operator* multiplies two bigint's this*that
bigint bigint::operator* (const bigint& that) const {
   if((uvalue == 0) or (that.uvalue == 0)){
      return bigint(0, false);
   }else if(that.is_negative == is_negative){ // equal signs
      ubigint result = uvalue * that.uvalue; 
      return bigint(result, false);
   }else{ //unequal signs
      ubigint result = uvalue * that.uvalue;
      return bigint(result, true);
   }
}

//operator/ divides two bigint's this/that
bigint bigint::operator/ (const bigint& that) const { 
   if(that.uvalue > uvalue){
      ubigint result = uvalue / that.uvalue; //result = 0
      return bigint(result, false);
   }else if(that.is_negative == is_negative){
      ubigint result = uvalue / that.uvalue;
      return bigint(result, false);
   }else{
      ubigint result = uvalue / that.uvalue;
      return bigint(result, true);
   }
}

//operator& takes modulus of bigint's this%that
bigint bigint::operator% (const bigint& that) const {
   ubigint result = uvalue % that.uvalue;
   if(result == 0){
      return bigint(result, false);
   }else{
      return bigint(result, is_negative);
   }
}

//operator== checks for equality of bigint's this == that
bool bigint::operator== (const bigint& that) const {
   return ((is_negative == that.is_negative) 
           and (uvalue == that.uvalue));
}

//operator< checks for inequality of bigint's this < that
bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   out << (that.is_negative ? "-" : "");
   out << that.uvalue;
   return out;
}

