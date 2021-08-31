#pragma once

#include "Visitor.h"

namespace hdps {

namespace util {

/**
 * Visitable class
 *
 * Base class for components that can be visited by a visitor
 *
 * @author Thomas Kroes
 */
template<typename VisitorType>
class Visitable {
public:

    /** Destructor */
    virtual ~Visitable() {}

    /** Accept visitor for visiting
     * @param visitor Reference to visitor that will visit this component
     */
    virtual void accept(VisitorType& visitor) const = 0;
};

}
}
