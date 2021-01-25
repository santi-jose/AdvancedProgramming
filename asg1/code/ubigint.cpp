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

//ubigint constructor w/ unsigned long input
ubigint::ubigint (unsigned long that) { 
   DEBUGF ('~', this << " -> " /*<< ubig_value*/)
   //while that is non-zero
   if(that!=0){
      while(that != 0){ 
         //push least significant digit
         ubig_value.push_back(that%10); 
         //divide that by 10 to evaluate next significant digit
         that = that/10;
      }
      del_zeroes(); //delete leading zeroes
   }
}

//ubigint constructor w/ string input
ubigint::ubigint (const string& that) {
   DEBUGF ('~', "that = \"" << that << "\"");   
   //iterate through chars in string& that
   for (char digit: that) { 
      //if the char is not a digit
      if (not isdigit (digit)) { 
         //throw error
         throw invalid_argument ("ubigint::ubigint(" + that + ")"); 
      } //otherwise
      //push conversion of char to ubig_value(digit - '0')
      ubig_value.insert(ubig_value.begin() ,digit - '0'); 
   }
   del_zeroes(); //delete leading zeroes
}

ubigint ubigint::operator+ (const ubigint& that) const {
   //DEBUGF ('u', *this << "+" << that);
   ubigint result; //define ubigint to store sum
   uint8_t digit1; //declare digit1
   uint8_t digit2; //declare digit2
   uint8_t sum; //declare sum
   uint8_t carry = 0; //initialize carry to zero
   
   //iterate through this and that ubig_value
   for(long unsigned int i = 0;
      (i<ubig_value.size())or(i<that.ubig_value.size());
      ++i){
      //assign digit1 and digit2
      digit1 = i < ubig_value.size() ? ubig_value[i] : 0;
      digit2 = i < that.ubig_value.size() ? that.ubig_value[i] : 0;
      //assign sum
      sum = (digit1 + digit2 + carry)%10; 
      //assign carry
      carry = (digit1 + digit2 + carry)/10; 
      //push sum into ubig_value
      result.ubig_value.push_back(sum); 
   }
   
   //if we have any other carry values lingering
   if(carry > 0){ 
      //push remaining carry on stack
      result.ubig_value.push_back(carry); 
   }
   result.del_zeroes(); //delete leading zeroes
   return result; //return result
   //DEBUGF ('u', result);
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint result; //define ubigint to store difference
   int digit1; //declare digit1
   int digit2; //declare digit2
   int diff; //declare diff
   int borrow = 0; //initialize borrow
   
   //iterate through this and that ubig_value
   for(long unsigned int i = 0;
      (i<ubig_value.size())or(i<that.ubig_value.size());
      ++i){
      digit1 = i < ubig_value.size() ? ubig_value[i] : 0;
      digit2 = i < that.ubig_value.size() ? that.ubig_value[i] : 0;
      if((digit1 - borrow) < digit2){
         diff = (digit1 + 10) - digit2 - borrow;
         borrow = 1; 
      }else{
         diff = digit1 - digit2 - borrow;
         borrow = 0;
      }
      result.ubig_value.push_back(uint8_t(diff));
   }
   result.del_zeroes(); //delete leading zeroes
   return result; //return result
}

ubigint ubigint::operator* (const ubigint& that) const {
   //initialize variables
   ubigint result; //define ubigint to store product
   //initialize result.ubig_value to size of n*m
   for(long unsigned int k = 0; 
      k < ((ubig_value.size()+that.ubig_value.size()));
      ++k){
      result.ubig_value.push_back(0);
   }
   uint8_t prod; //declare digit to store into result
   uint8_t carry; //declare carry
   
   //multiply operands if they are non-zero
   for(long unsigned int i = 0;i<ubig_value.size();++i){
      carry = 0;
      for(long unsigned int j = 0;j<that.ubig_value.size();++j){
         prod = result.ubig_value[i+j]
              + ubig_value[i]*that.ubig_value[j]
              + carry;
         result.ubig_value[i+j] = prod % 10;
         carry = prod / 10;
      }
      result.ubig_value[i+(that.ubig_value.size())] = carry;
   }
   result.del_zeroes(); //trim leading zeroes
   return result;
}

void ubigint::multiply_by_2() {
   (*this)=(*this) * 2;
}

void ubigint::divide_by_2() {
   for(long unsigned int i = 0;i < ubig_value.size(); ++i){
      if((i+1)<ubig_value.size()){ //there is a next sig dig
         //check if next digit is odd or even
         if((ubig_value[i+1] % 2) == 1){ //next digit is odd
            ubig_value[i] = (ubig_value[i] / 2) + 5;
         }else{ //next digit is even
            ubig_value[i] = ubig_value[i] / 2;
         }
      }else{ //this is the most significant digit
         ubig_value[i] = ubig_value[i] / 2;
      }
   }
   del_zeroes(); //trim leading zeroes
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
   //DEBUGF ('/', "quotient = " << quotient);
   //DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if(ubig_value == that.ubig_value){ //vectors are equal
      return true; //return true
   }else{ //vectors are not equal
      return false; //return false
   }
}

bool ubigint::operator< (const ubigint& that) const {
   if(ubig_value.size() < that.ubig_value.size()){ 
      return true;
   }else if(ubig_value.size() > that.ubig_value.size()){ 
      return false;
   }else{ //ubig_value.size() == that.ubig_value.size()
      //iterate through vectors from back to front
      for(int i = ubig_value.size() - 1; i >= 0; --i){ 
         if(ubig_value[i] < that.ubig_value[i]){ 
            return true;
         }else if(ubig_value[i] > that.ubig_value[i]){
            return false;
         }
         //elements must be equal, check next less significant element
      } //vectors must be equal
      return false; //checked all members of vectors, therefore false
   }
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   //iterate ubigint from end to begin
   int char_count = 0;

   if(that.ubig_value.size()==0){
      out << 0;
   }else{
      for(auto it = that.ubig_value.rbegin(); 
         it != that.ubig_value.rend(); 
         ++it){
         out << int(*it); //send element from ubigint at iterator to out
         char_count++; //increment char count
         if(char_count%69 == 0){
            out<<"\\"<<endl;
         }
      }
   }
   return out;
}

//del_zeroes helper function that deletes leading zeroes
void ubigint::del_zeroes(){
   while((ubig_value.back()==0) and (ubig_value.size()>0)){
      ubig_value.pop_back(); //delete leading zero
   }
}

