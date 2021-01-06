// $Id: bigint.h,v 1.2 2020-01-06 13:39:55-08 - - $

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h" //include ubigint.h header file

class bigint { //bigint class
   friend ostream& operator<< (ostream&, const bigint&);
   private: //private members
      ubigint uvalue; //declare uvalue as a ubigint
      bool is_negative {false}; //set is_negative boolean to false
   public: //public members

      bigint() = default; // Need default constructor or will be suppressed.
      bigint (long); //bigint constructor with long input
      bigint (const ubigint&, bool is_negative = false); //bigint constructor with ubigint pointer input and is_negative boolean set to false
      explicit bigint (const string&); //explicit bigint constructor with string pointer input

      bigint operator+() const;
      bigint operator-() const;

      bigint operator+ (const bigint&) const; //operator+ function prototype: adds two bigint's 
      bigint operator- (const bigint&) const; //operator- function prototype: subtracts two bigint's
      bigint operator* (const bigint&) const; //operator* function prototype: multiplies two bigint's
      bigint operator/ (const bigint&) const; //operator/ function prototype: divides two bigint's
      bigint operator% (const bigint&) const; //operator% function prototype: find the remainder of the quotient of two bigint's

      bool operator== (const bigint&) const; //operator== function prototype: checks equality of two bigint's
      bool operator<  (const bigint&) const; //operator< function prototype: checks if one bigint is less than the other
};

#endif

