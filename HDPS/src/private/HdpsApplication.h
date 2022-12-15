#pragma once

#include <Application.h>

namespace hdps {

/**
 * HDPS application class
 * 
 * @author Thomas Kroes
 */
class HdpsApplication : public Application
{
   
public: // Construction

    /**
     * Constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     */
    HdpsApplication(int& argc, char** argv);
};

}