/****************************************
* Russell Yanofsky                      *
* rey4@columbia.edu                     *
* ry_help.h                             *
* Helper functions, classes, and macros *
****************************************/

#ifndef ry_help_h
#define ry_help_h

#include <iostream>
#include <string>
#include <map>
//#include <limits>

using std::cerr;
using std::endl;
using std::string;
using std::map;
//using std::numeric_limits;

// gcc does not currently support numeric_limits
const double EPSILON = 0.00001;

// return true if c is in [a,b]
template<typename T>
bool inline isbetween(const T a, const T b, const T c)
{
  return (a < b) ? (a <= c) && (c <= b) : (b <= c) && (a <= b);
}

// simple exception class
class Exception
{
public:  
  Exception(char const * message, char const * file, int line) :
    _message(message), _file(file), _line(line)
  { }
  
  Exception(string const message, char const * file, int line) :
    _message(message), _file(file), _line(line)
  { }
  
  void show()
  {
    cerr << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n"
         << "Exception! (\n" << _file << ':' << _line << ")\n" 
         << _message.c_str() << endl
         << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
  }

private:
  string _message;
  char const * _file;
  int _line;
};

#define BARF(message) throw Exception(message, __FILE__, __LINE__)

template<typename Key, typename T, typename U, typename V>
bool inline map_get(map<Key,T,U,V> const & m, const Key & key, T & t)
{
  map<Key,T,U,V>::const_iterator i = m.find(key);
  if (i == m.end())
    return false;
  else
  {
    t = i->second;
    return true;
  }  
};

// get a slice of a string
string inline string_slice(string str, int start, int end)
{
  return str.substr(start, end - start);
};

// returns true if two floating point numbers a,b can be considered equal
template<typename T>
double fequals(T a, T b)
{
  return (a>b ? (a-b) : (b-a)) <= EPSILON;
}

// return true for two floating points a,b if a <= b
template<typename T>
double lessequals(double a, double b)
{
  return a <= b + EPSILON;
}

// returns the size of a single dimensionalal, not dynamically allocated array
#define DIM(a) (sizeof(a)/sizeof(a[0]))

#endif
