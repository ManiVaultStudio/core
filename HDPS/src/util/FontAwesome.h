#pragma once

#include "IconFont.h"

namespace hdps {

/**
 * Font Awesome icon font class
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
    QString getIconCharacter(const QString& iconName) const;

private:
    QString     _metaDataResource;      /** Location of the meta data resource JSON */
};

}