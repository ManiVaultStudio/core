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

class InfoAction;

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

    /** Initializes the dataset */
    void init() override;

public: // Subsets

    /** Create an image subset (not implemented at this point) */
    QString createSubset(const QString subsetName = "subset", const QString parentSetName = "", const bool& visible = true) const override;;

public: // Image retrieval functions

    /** Obtain a copy of this dataset */
    hdps::DataSet* copy() const override;

    /** Gets the image collection type e.g. stack or sequence */
    ImageData::Type getType() const;

    /**
     * Sets the image collection type e.g. stack or sequence
     * @param type Image collection type
     */
    void setType(const ImageData::Type& type);

    /** Gets the number of images in this dataset */
    std::uint32_t getNumberOfImages() const;

    /**
     * Sets the number of images
     * @param numberOfImages Number of images
     */
    void setNumberOfImages(const std::uint32_t& numberOfImages);

    /** Gets the size of the images */
    QSize getImageSize() const;

    /**
     * Sets the image size
     * @param imageSize Size of the image(s)
     */
    void setImageSize(const QSize& imageSize);

    /** Gets the number of components per pixel */
    std::uint32_t getNumberOfComponentsPerPixel() const;

    /**
     * Sets the number of components per pixel
     * @param numberOfComponentsPerPixel Number of components per pixel
     */
    void setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel);

    /** Gets the image file paths */
    QStringList imageFilePaths() const;

    /**
     * Sets the image file paths
     * @param imageFilePaths Image file paths
     */
    void setImageFilePaths(const QStringList& imageFilePaths);

    /** Gets the dimension names (the image names) */
    QStringList getDimensionNames() const;

    /**
     * Sets the dimension names
     * @param dimensionNames Dimension names
     */
    void setDimensionNames(const QStringList& dimensionNames);

    /** Returns the number of pixels in total */
    std::uint32_t getNumberOfPixels() const;

    /** Returns the number of color channels per pixel */
    static std::uint32_t noChannelsPerPixel();

    /**
     * Get image at index
     * 
     */
    QImage getImage(const std::uint32_t& index);

    /** Get icon for the dataset */
    QIcon getIcon() const override;

private:
    ImageData*                      _imageData;     /** Pointer to raw image data */
    QSharedPointer<InfoAction>      _infoAction;    /** Shared pointer to info action */
};