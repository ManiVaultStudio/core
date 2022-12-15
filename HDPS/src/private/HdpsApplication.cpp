#include "HdpsApplication.h"

#include <CoreInterface.h>

#include <QDebug>

#define _VERBOSE

using namespace hdps::util;

namespace hdps {

HdpsApplication::HdpsApplication(int& argc, char** argv) :
    Application(argc, argv)
{
}

}
