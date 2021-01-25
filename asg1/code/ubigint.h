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
      using ubigvalue_t = vector<uint8_t>;
      ubigvalue_t ubig_value; //create ubig_value of type ubigvalue_t
      void del_zeroes(); //helper function to delete leading zeroes

   public: //public members
      void multiply_by_2(); //multiplies ubigint by 2
      void divide_by_2(); //divides ubigint by 2

      ubigint() = default; // Need default constructor as well
      ubigint (unsigned long); //ubigint constructor from unsigned long
      ubigint (const string&); //ubigint constructor from string

      ubigint operator+ (const ubigint&) const; //adds ubigint's
      ubigint operator- (const ubigint&) const; //subtracts ubigint's
      ubigint operator* (const ubigint&) const; //multiplies ubigint's
      ubigint operator/ (const ubigint&) const; //divides ubigint's
      ubigint operator% (const ubigint&) const; //modulus ubigint's

      bool operator== (const ubigint&) const; //checks equality
      bool operator<  (const ubigint&) const; //checks this<that
};

#endif

