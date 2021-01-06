#include "Polynom.h"
#include "boost/spirit.hpp"

using namespace std;
using namespace boost::spirit;

Polynome::Polynome()
{
    val_ = "";
    leadingFactor_ = 0;
}

Polynome::Polynome(const std::string& val)
{
    val_ = val;
    leadingFactor_ = 0;
}


Polynome::~Polynome(void)
{

}

void Polynome::clear(void)
{
    val_.clear();
    leadingFactor_ = 0;
}

Polynome& Polynome::operator=(const std::string &s)
{
    double temp;
    string tempString;
    
    if(parse(s.c_str(), real_p[assign_a(temp)]).full)
    {
        leadingFactor_ = temp;
        val_.clear();
    }
    else if(parse(s.c_str(), real_p[assign_a(temp)] >> ch_p('*') >> (+anychar_p)[assign_a(tempString)]).full)
    {
        if(tempString[0] != '(')
            val_ = "(" + tempString + ")";
        else
            val_ = tempString;
        leadingFactor_ = temp;
    }
    else
    {
        val_ = tempString;
        leadingFactor_ = 1;
    }
    
    return *this;
}

Polynome Polynome::operator+(const string &s)
{
    Polynome result = *this;
    result += s;
    return result;
}

Polynome& Polynome::operator+=(const string &s)
{
    double temp;
    string tempString;

    if(parse(s.c_str(), real_p[assign_a(temp)]).full)
    {
        if(val_.empty())
            leadingFactor_ += temp;
        else
        {
            char buf[20];
            sprintf(buf, "%f", temp);

            char buf2[20];
            sprintf(buf2, "%f", leadingFactor_);

            if(leadingFactor_ != 0)
                val_ = "(" + string(buf2) + "*" + val_ + " + "  + string(buf) + ")"; 
            else
                val_ = "(" + val_ + " + "  + string(buf) + ")"; 

            leadingFactor_ = 1;
            
        }
    }
    else if(parse(s.c_str(), real_p[assign_a(temp)] >> ch_p('*') >> *ch_p('(')>> (+anychar_p)[assign_a(tempString)] >> *ch_p(')')).full)
    {
        // add temp to current leadingNumber but () around the whole thing
        char buf[20];
        sprintf(buf, "%f", temp);

        char buf2[20];
        sprintf(buf2, "%f", leadingFactor_);

        if(val_.empty())
        {
            if(leadingFactor_ == 0)
            {
                val_ = string("(") + tempString + ")";
                leadingFactor_ = temp;
            }
            else
            {
                if(leadingFactor_ != 1)
                    val_ = string("(") + string(buf2) + "*" + val_ + " + " + string(buf) + "*" + tempString + ")";
                else
                    val_ = string("(") + val_ + " + " + string(buf) + "*" + tempString + ")";
                leadingFactor_ = 1;
            }
        }
        else
        {
            val_ += string("(") + val_ + " + (" + tempString + "))";
        }
        leadingFactor_ = 1;
    }
    else
    {
        char buf2[20];
        sprintf(buf2, "%f", leadingFactor_);

        if(leadingFactor_ != 0)
            val_ = "(" + string(buf2) + "*" + val_ + " + " + s + ")"; 
        else
            val_ = "(" + val_ + ")"; 
        
        leadingFactor_ = 1;
    }
    return *this;
}

Polynome Polynome::operator+(const Polynome &p)
{
    Polynome result = *this;
    result += p.text();
    return result;
}


Polynome& Polynome::operator+=(const Polynome &p)
{
    return this->operator +(p.text());
}

Polynome Polynome::operator*(const std::string &s)
{
    Polynome result = *this;
    result *= s;
    return result;
}

Polynome& Polynome::operator*=(const std::string &s)
{
    double temp;
    string tempString;
    
    if(parse(s.c_str(), real_p[assign_a(temp)]).full)
    {
        leadingFactor_ *= temp;
    }
    else if(parse(s.c_str(), real_p[assign_a(temp)] >> ch_p('*') >> (+anychar_p)[assign_a(tempString)]).full)
    {
        val_ += "*(" + tempString + ")";
        leadingFactor_ *= temp;
    }
    else
    {
        val_ += "*(" + s + ")";
    }

    return *this;
}

Polynome Polynome::operator*(const Polynome &p)
{
    Polynome result = *this;
    result *= p.text();
    return result;
}

Polynome& Polynome::operator*=(const Polynome &p)
{
    return this->operator *(p.text());
}

const std::string Polynome::text(void) const
{
    char buf[20];

    if(leadingFactor_ == 0)
        return string("");
    if(leadingFactor_ == 1)
        if(val_.empty())
            return string("1");
        else
            return val_;

    sprintf(buf, "%f", leadingFactor_);

    if(val_.empty())
        return string(buf);

    return string(buf) + "*" + val_;
}

std::ostream& operator <<(std::ostream &os,const Polynome &poly)
{
    os << poly.text();
    return os;
}