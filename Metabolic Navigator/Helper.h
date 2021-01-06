#pragma once
#include <string>
#include <iostream>
#include <functional>
#include <algorithm>
#include <map>
#include "Compound.h"
#include "boost/shared_ptr.hpp"
#include "MetaboliteObject.h"
#include "reaction.h"

bool splitCompound(std::string Compound, std::string *CompoundName, std::string *CompartmentName);

struct lowercase_func {
void operator()(std::string::value_type& v) { v = tolower(v); }
};

std::string make_lowercase(std::string s);

/*
MetaboliteObject* getOrCreateMetaboliteObject(Compound *compound, 
    string compartment,  
    map<string,MetaboliteObject*> *metabolitesPool);
    */



class sortByFirst {
  public:
  bool operator()(const std::pair<std::string,Reaction*> p1, const std::pair<std::string,Reaction*> p2) {
    return (p1<p2);
  }
};

template<typename T_first, typename T_second>
class sortBySecond {
  public:
      bool operator()(const std::pair<T_first,T_second> p1, const std::pair<T_first,T_second> p2) {
          return (p1.second<p2.second);
  }
};
