#pragma once

#include <Application.h>
#include <QTemporaryDir.h>

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

public: // Serialization

    /** Load application from disk */
    void loadAnalysis() override final;

    /** Save application to disk */
    void saveAnalysis() override final;

protected:
    QSharedPointer<QTemporaryDir>   _temporaryDir;      /** Temporary dir for compression */
};

}