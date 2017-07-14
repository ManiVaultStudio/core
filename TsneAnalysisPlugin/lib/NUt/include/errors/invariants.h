#ifndef INVARIANTS_H
#define INVARIANTS_H

#include "assert_by_exception.h"

#define PRECONDITION(v)	{::nut::CheckAndThrowLogic(v,"A precondition has been violated!",__FILE__,__LINE__);}
#define POSTCONDITION(v){::nut::CheckAndThrowLogic(v,"A postcondition has been violated!",__FILE__,__LINE__);}

#endif //INVARIANTS_H