#pragma once

#include <QApplication>

#include "util/IconFonts.h"

namespace hdps {

/**
 * HDPS application class
 * 
 * Its intended use is application-wide sharing of resources such as icons and fonts
 *
 * @author Thomas Kroes
 */
class Application : public QApplication
{
public: // Construction/destruction

    /**
     * Constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     */
    Application(int& argc, char** argv);

public: // Miscellaneous

    /** Returns a pointer to the current HDPS application (if the current application derives from hdps::Application) */
    static Application* current();

public: // Static resource access functions

    /**
     * Returns an icon font by \p name \p majorVersion and \p minorVersion
     * @param name Name of the icon font (currently FontAwesome is supported)
     * @param majorVersion Major version number
     * @param minorVersion Minor version number
     */
    static const IconFont& getIconFont(const QString& name, const std::int32_t& majorVersion = -1, const std::int32_t& minorVersion = -1);

protected:
    IconFonts       _iconFonts;     /** Icon fonts resource */
};

}