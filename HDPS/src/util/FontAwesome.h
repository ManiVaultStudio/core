#pragma once

#include "IconFont.h"

namespace hdps {

/**
 * Font Awesome icon font class
 *
 * Follow the steps below to install a Font Awesome icon font
 * 1. Download a version of Font Awesome from this location: https://fontawesome.com/download
 * 2. Extract the zip file and note the major and minor version ([MAJOR_VERSION], [MINOR_VERSION])
 * 3. Copy the font from /otfs/Font Awesome x Free-Solid-900.otf to /core/HDPS/res/iconfonts and rename the file to: FontAwesome-[MAJOR_VERSION].[MINOR_VERSION].otf
 * 4. Copy the meta data file from /metadata/icons.json to core/HDPS/res/iconfonts and rename the file to: FontAwesome-[MAJOR_VERSION].[MINOR_VERSION].json
 * 5. Open /core/HDPS/res/resources.qrc in a text editor and add two entries:
 * 6. <qresource prefix="/IconFonts">
          ...
          <file alias="FontAwesome-[MAJOR_VERSION].[MINOR_VERSION].otf">iconfonts/FontAwesome-[MAJOR_VERSION].[MINOR_VERSION].otf</file>
          <file alias="FontAwesome-[MAJOR_VERSION].[MINOR_VERSION].json">iconfonts/FontAwesome-[MAJOR_VERSION].[MINOR_VERSION].json</file>
 *    </qresource>
 * 7. Add the font in the hdsp::Application constructor:
      _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome([MAJOR_VERSION], [MINOR_VERSION])));
 * 
 * @author Thomas Kroes
 */
class FontAwesome : public IconFont
{
public: // Construction/destruction

    /**
     * Constructor
     * @param majorVersion Major version number
     * @param minorVersion Minor version number
     */
    FontAwesome(const std::uint32_t& majorVersion = 5, const std::uint32_t& minorVersion = 14);

    /** Destructor */
    ~FontAwesome() {};

public: // Initialization

    /**
     * Initializes the icon font
     * A typical implementation builds a mapping from icon name to icon character (unicode)
     */
    void initialize() override;

public: // Conversion

    /**
     * Converts human readable \p icon name to icon character (unicode)
     * @param iconName Name of the icon
     * @param return Icon character (unicode)
     */
    QString getIconCharacter(const QString& iconName) const override;

private:
    QString     _metaDataResource;      /** Location of the meta data resource JSON */
};

}