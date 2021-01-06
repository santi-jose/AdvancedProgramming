// $Id: ubigint.h,v 1.5 2020-10-11 12:25:22-07 - - $

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"

class ubigint { //ubigint class
   friend ostream& operator<< (ostream&, const ubigint&);
   private: //private members
      using ubigvalue_t = vector<uint8_t>; //ubigvalue_t declared as vector of uint8_t
      ubigvalue_t ubig_value; //create ubig_value of type ubigvalue_t(vector<uint8_t>)
   public: //public members
      void multiply_by_2(); //ubigint.multiply_by_2(): multiplies ubigint by 2
      void divide_by_2(); //ubigint.divide_by_2(): divides ubigint by 2

      ubigint() = default; // Need default ctor as well.
      ubigint (unsigned long); //ubigint constructor with input of unsigned long
      ubigint (const string&); //ubigint constructor with input of string pointer

      ubigint operator+ (const ubigint&) const; //operator+ function prototype that adds two ubigint's
      ubigint operator- (const ubigint&) const; //operator- function prototype that subtracts two ubigint's
      ubigint operator* (const ubigint&) const; //operator* function prototype that multiplies two ubigint's
      ubigint operator/ (const ubigint&) const; //operator/ function prototype that divides two ubigint's
      ubigint operator% (const ubigint&) const; //operator% function prototype that finds the remainder of the division of two ubigint's

      bool operator== (const ubigint&) const; //operator== function prototype that checks for equality of two ubigint's
      bool operator<  (const ubigint&) const; //operator< function prototype that checks if one ubigint is less than another
};

#endif

