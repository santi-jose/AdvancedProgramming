// $Id: ubigint.h,v 1.5 2020-10-11 12:25:22-07 - - $

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
#include <cstdint> //include <cstdint> to use uint8_t
#include <vector> //include <vector> library
using namespace std;

#include "debug.h"

class ubigint { //ubigint class
   friend ostream& operator<< (ostream&, const ubigint&);
   private: //private members
      using ubigvalue_t = vector<uint8_t>; //ubigvalue_t defined as vector<uint8_t>
      ubigvalue_t ubig_value; //create ubig_value of type ubigvalue_t(vector<uint8_t>)
   public: //public members
      void multiply_by_2(); //multiplies ubigint by 2
      void divide_by_2(); //divides ubigint by 2

      ubigint() = default; // Need default constructor as well
      ubigint (unsigned long); //ubigint constructor from unsigned long
      ubigint (const string&); //ubigint constructor from string&(address/reference)

      ubigint operator+ (const ubigint&) const; //adds two ubigint's
      ubigint operator- (const ubigint&) const; //subtracts two ubigint's
      ubigint operator* (const ubigint&) const; //multiplies two ubigint's
      ubigint operator/ (const ubigint&) const; //divides two ubigint's
      ubigint operator% (const ubigint&) const; //finds remainder of division of two ubigint's

      bool operator== (const ubigint&) const; //checks for equality of two ubigint's
      bool operator<  (const ubigint&) const; //checks if one ubigint is less than other
};

#endif

