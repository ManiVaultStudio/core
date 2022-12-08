#pragma once

#include "util/Serializable.h"

#include <QObject>

namespace hdps
{

class AbstractManager : public QObject, public util::Serializable
{
public:

    /** Resets the contents of the manager */
    virtual void reset() = 0;
};

}
