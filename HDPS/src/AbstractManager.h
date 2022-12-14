#pragma once

#include "util/Serializable.h"

#include <QObject>

namespace hdps
{

/**
 * Abstract manager
 * 
 * Base abstract manager class for the core.
 * 
 * @author Thomas Kroes
 */
class AbstractManager : public QObject, public util::Serializable
{
public:

    /** Perform manager startup initialization */
    virtual void initalize() = 0;

    /** Resets the contents of the manager */
    virtual void reset() = 0;

    /**
     * Get icon representing the manager
     * @return Manager icon
     */
    virtual QIcon getIcon() const = 0;
};

}
