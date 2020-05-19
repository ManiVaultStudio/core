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
    virtual void createSubset() const override {};

public: // Functions for populating the set with images

    /**
     * Populates the set with a sequence of images (each image is a dimension)
     * @param images Sequence images
     * @param size Size of the images
     */
    void setSequence(const std::vector<Image>& images, const QSize& size);

    /**
     * Populates the set with a stack of images (each pixel is a dimension)
     * @param images Stack of images
     * @param size Size of the images
     */
    void setStack(const std::vector<Image>& images, const QSize& size);

public: // Image retrieval functions

    /**
     * Retrieves one or more images from a sequence of images
     * @param imageIds Identifiers of the images to retrieve
     * @return Average of images
     */
    QImage sequenceImage(const std::vector<std::uint32_t>& imageIds);

    /**
     * Retrieves a single image from a stack of images
     * @param imageId Identifier of the image to retrieve
     * @return Stack image
     */
    QImage stackImage(const std::uint32_t& imageId);

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

private:
    ImageData*  _imageData;     /** Pointer to raw image data */
    Points*     _points;        /** Pointer to the referenced points dataset */
};