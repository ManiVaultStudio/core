#pragma once

#include "Common.h"
#include "Image.h"
#include "ImageData.h"

#include <Set.h>

#include <QString>
#include <QImage>
#include <QColor>
#include <QRect>
#include <QSize>

#include <vector>

using namespace hdps::plugin;

class Points;

/**
 * Images dataset class
 *
 * Class for dealing with high-dimensional image data
 *
 * @author Thomas Kroes
 */
class IMAGEDATA_EXPORT Images : public hdps::DataSet
{
public: // Construction

    /**
     * Constructor
     * @param core Pointer to core interface
     * @param dataName Name of the dataset
     */
    Images(hdps::CoreInterface* core, QString dataName);

public: // Subsets

    /** Create an image subset (not implemented at this point) */
    QString createSubset(const QString subsetName = "subset", const QString parentSetName = "", const bool& visible = true) const override { return ""; };

public: // Image retrieval functions

    /** Obtain a copy of this dataset */
    hdps::DataSet* copy() const override;

    /** Returns the image collection type e.g. stack or sequence */
    ImageData::Type type() const;

    /** Returns the number of images in this dataset */
    std::uint32_t noImages() const;

    /** Returns the size of the images */
    QSize imageSize() const;

    /** Returns the number of components */
    std::uint32_t noComponents() const;

    /** Returns the (absolute) image file paths */
    std::vector<QString> imageFilePaths() const;

    /** Returns the dimension names (the image names) */
    std::vector<QString> dimensionNames() const;

    /** Returns the number of pixels in total */
    std::uint32_t noPixels() const;

    /** Returns the number of color channels per pixel */
    static std::uint32_t noChannelsPerPixel();

    /** Name of the referenced points set */
    QString pointsName();

    /** Returns a pointer to the referenced points set */
    Points* points();

    /** Sets the referenced points set */
    void setPoints(Points* points);

    /** Get icon for the dataset */
    QIcon getIcon() const override;

private:
    ImageData*  _imageData;     /** Pointer to raw image data */
    Points*     _points;        /** Pointer to the referenced points dataset */
};