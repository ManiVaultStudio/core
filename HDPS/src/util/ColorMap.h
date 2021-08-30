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

    /**
     * Color map type
     * Defines the color map types
     */
    enum class Type {
        ZeroDimensional,    /** Zero-dimensional color map (not in use at the moment) */
        OneDimensional,     /** One-dimensional color map */
        TwoDimensional      /** Two-dimensional color map */
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
    int getNoDimensions() const;

private:
    const QString       _name;              /** Name in the user interface */
    const QString       _resourcePath;      /** Resource path of the color image */
    const Type          _type;              /** Color map type */
    const QImage        _image;             /** Color map image */
};

}
}
