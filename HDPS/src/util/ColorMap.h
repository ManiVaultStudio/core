#pragma once

#include <QImage>

namespace hdps {

namespace util {

/**
 * Color map class
 *
 * @author Thomas Kroes
 */
class ColorMap
{

public: // Enumerations

    /** Color map types */
    enum class Type {
        OneDimensional = 1,     /** One-dimensional color map */
        TwoDimensional          /** Two-dimensional color map */
    };

public: // Construction/destruction

    /** (Default) constructor
     * @param name Name of the color map
     * @param resourcePath Resource path of the color map image
     * @param name Color map type
     * @param image Color map image
     */
    ColorMap(const QString& name = "", const QString& resourcePath = "", const Type& type = Type::OneDimensional, const QImage& image = QImage());

public: // Getters

    /** Returns the color map name */
    QString getName() const;

    /** Returns the resource path */
    QString getResourcePath() const;

    /** Returns the color map type */
    Type getType() const;

    /** Returns the color map image */
    QImage getImage() const;

    /** Returns the number of dimensions */
    std::uint32_t getNoDimensions() const;

private:
    QString       _name;              /** Name in the user interface */
    QString       _resourcePath;      /** Resource path of the color image */
    Type          _type;              /** Color map type */
    QImage        _image;             /** Color map image */
};

}
}
