#pragma once
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <vector>
#include <string>
#include <iostream>


class Polynome
{
public:
    Polynome();
    Polynome(const std::string& );
    ~Polynome(void);
    const std::string text(void) const;

    Polynome operator*(const Polynome &p);
    Polynome& operator*=(const Polynome &p);
    Polynome operator*(const std::string &s);
    Polynome& operator*=(const std::string &s);
    
    Polynome operator+(const Polynome &p);
    Polynome& operator+=(const Polynome &p);
    Polynome operator+(const std::string &s);
    Polynome& operator+=(const std::string &s);
    
    Polynome& operator=(const std::string &s);
    void clear(void);
private:
    std::string val_;
    double leadingFactor_;
};


std::ostream& operator <<(std::ostream &os,const Polynome &);