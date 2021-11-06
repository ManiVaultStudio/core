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

    /** Get the source rectangle */
    QRect getSourceRectangle() const;

    /** Get the target rectangle */
    QRect getTargetRectangle() const;

    /**
     * Set the image geometry
     * @param sourceImageSize Size of the source image(s)
     * @param targetImageSize Size of the target image(s)
     * @param imageOffset Offset of the image(s)
     */
    void setImageGeometry(const QSize& sourceImageSize, const QSize& targetImageSize = QSize(), const QPoint& imageOffset = QPoint());

    /** Gets the number of components per pixel */
    std::uint32_t getNumberOfComponentsPerPixel() const;

    /**
     * Sets the number of components per pixel
     * @param numberOfComponentsPerPixel Number of components per pixel
     */
    void setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel);

    /** Get the image file paths */
    QStringList getImageFilePaths() const;

    /**
     * Sets the image file paths
     * @param imageFilePaths Image file paths
     */
    void setImageFilePaths(const QStringList& imageFilePaths);

    /** Returns the number of pixels in total */
    std::uint32_t getNumberOfPixels() const;

    /** Returns the number of color channels per pixel */
    static std::uint32_t noChannelsPerPixel();

    /** Get icon for the dataset */
    QIcon getIcon() const override;

public:

    /**
     * Get scalar image data
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarData(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get selection data
     * @param selectionImageData Image data for the selection
     * @param selectedIndices Selected pixel indices
     * @param selectionBoundaries Boundaries of the selection (in image coordinates)
     */
    void getSelectionData(std::vector<std::uint8_t>& selectionImageData, std::vector<std::uint32_t>& selectedIndices, QRect& selectionBoundaries);

protected:

    /**
     * Get scalar data for image sequence
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarDataForImageSequence(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get scalar data for image stack
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarDataForImageStack(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get target pixel index from two-dimensional image coordinate
     * @param coordinate Two-dimensional image coordinate
     * @return Target pixel index
     */
    std::uint32_t getTargetPixelIndex(const QPoint& coordinate) const;

    /**
     * Get scalar data index from two-dimensional image coordinate
     * @param coordinate Two-dimensional image coordinate
     * @return Scalar data index
     */
    std::uint32_t getSourceDataIndex(const QPoint& coordinate) const;

private:
    ImageData*                      _imageData;     /** Pointer to raw image data */
    QSharedPointer<InfoAction>      _infoAction;    /** Shared pointer to info action */
};