// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

ubigint::ubigint (unsigned long that) { //ubigint constructor
   //DEBUGF ('~', this << " -> " << ubig_value)
   while(that != 0){ //while that is non-zero
      ubig_value.push_back(that%10); //push least significant digit from 0-9 into ubig_value
	  that = that/10; //divide that by 10 to evaluate next significant digit in that
   }
}

ubigint::ubigint (const string& that): ubig_value(0) { //ubigint constructor
   //DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) { //iterate through chars in string& that
      if (not isdigit (digit)) { //if the char is not a digit
         throw invalid_argument ("ubigint::ubigint(" + that + ")"); //throw error
      } //otherwise
      ubig_value.push_back(digit - '0'); //push conversion of char to ubig_value(digit - '0')
   }
}
/*
ubigint ubigint::operator+ (const ubigint& that) const {
   DEBUGF ('u', *this << "+" << that);
   ubigint result (ubig_value + that.ubig_value);
   DEBUGF ('u', result);
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   return ubigint (ubig_value - that.ubig_value);
}

ubigint ubigint::operator* (const ubigint& that) const {
   return ubigint (ubig_value * that.ubig_value);
}

void ubigint::multiply_by_2() {
   ubig_value *= 2;
}

void ubigint::divide_by_2() {
   ubig_value /= 2;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   DEBUGF ('/', "quotient = " << quotient);
   DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
   return ubig_value < that.ubig_value;
}
*/
ostream& operator<< (ostream& out, const ubigint& that) { 
   out << "ubigint(";
   for(auto it = that.rbegin(); it != that.rend(); ++it){ //iterate elements of ubigint from end to begin
      out << (*it); //send element from ubigint at iterator to ostream& out
   }
   out << ")"; //close bracket for current ubigint
   return out;
}

