/*
LNP
Copyright (C) 2008 Benjamin Menküc

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/
*/

#pragma once
#include<string>

using namespace std;

class GraphvizNodeAttributes
{
public:
    GraphvizNodeAttributes(void) : fillcolor_(""), fontsize_("14"), rank_(0)
    {};
    ~GraphvizNodeAttributes(void){};
    void dotCode(string &code);
    void setFillcolor(string fillcolor)
    {
        fillcolor_ = fillcolor;
    }
    void setFontsize(string fontsize)
    {
        fontsize_ = fontsize;
    }
    void setDisplayText(string displayText)
    {
        displayText_ = displayText;
    }
    void setRank(unsigned int rank)
    {
        rank_ = rank;
    }
    unsigned int rank(void)
    {
        return rank_;
    }
private:
    string fillcolor_;
    string fontsize_;
    string displayText_;
    unsigned int rank_;
};
