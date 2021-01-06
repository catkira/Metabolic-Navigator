#include "ImportCompoundsFromKegg.h"
#include "pool.h"
#include "MyAlgorithms.h"


#include "spirit_include.h"
#include <boost/spirit/actor/increment_actor.hpp>

ImportCompoundsFromKegg::ImportCompoundsFromKegg(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count)
: count(count)
{
}

ImportCompoundsFromKegg::~ImportCompoundsFromKegg(void)
{
}
