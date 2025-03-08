// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Common.h"
#include "Image.h"
#include "ImageData.h"

#include <Set.h>

#include <QColor>
#include <QRect>
#include <QSize>
#include <QString>

#include <tuple>
#include <vector>

using namespace mv::plugin;

class InfoAction;

/**
 * Images dataset class
 *
 * Class for dealing with high-dimensional image data
 *
 * @author Thomas Kroes
 */
class IMAGEDATA_EXPORT Images : public mv::DatasetImpl
{
public: // Construction

    /**
     * Constructor
     * @param dataName Name of the dataset
     * @param mayUnderive Whether the dataset may be un-derived, if not it should always co-exist with its source
     * @param guid Globally unique dataset identifier (use only for deserialization)
     */
    Images(QString dataName, bool mayUnderive = false, const QString& guid = "");

    /** Initializes the dataset */
    void init() override;

    /**
     * Creates a dataset and derives an image dataset from it:
     *
     *      auto [pointDataset, imageDatset] = Images::addImageDataset("My Image", parent);
     *      auto [clusterDataset, imageDatset] = Images::addImageDataset("My Image", parent, "Cluster");
     *
     * This function sends the core event notifications "notifyDatasetAdded" for both pointDataset and imageDatset
     *
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataSet Smart pointer to the parent dataset in the data hierarchy (root if smart pointer is not valid)
     * @param pluginKind Either "Points" (default) or "Cluster" to define the data type of the raw value data set
     */
    static std::tuple<mv::Dataset<mv::DatasetImpl>, mv::Dataset<Images>> addImageDataset(QString datasetGuiName, const mv::Dataset<mv::DatasetImpl>& parentDataSet = mv::Dataset<mv::DatasetImpl>(), QString pluginKind = "Points");

public: // Subsets

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    mv::Dataset<mv::DatasetImpl> createSubsetFromSelection(const QString& guiName, const mv::Dataset<mv::DatasetImpl>& parentDataSet = mv::Dataset<mv::DatasetImpl>(), const bool& visible = true) const override;

public: // Image retrieval functions

    /** Obtain a copy of this dataset */
    mv::Dataset<mv::DatasetImpl> copy() const override;

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
     * Set the image size
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

    /** Get the rectangle of the entire image */
    QRect getRectangle() const;

    /** Get the rectangle which bounds the visible pixels */
    QRect getVisibleRectangle() const;

public: // Selection

    /**
     * Get selection indices
     * @return Selection indices
     */
    std::vector<std::uint32_t>& getSelectionIndices() override;

    /**
     * Select by indices
     * @param indices Selection indices
     */
    void setSelectionIndices(const std::vector<std::uint32_t>& indices) override;

    /** Determines whether items can be selected */
    bool canSelect() const override;

    /** Determines whether all items can be selected */
    bool canSelectAll() const override;

    /** Determines whether there are any items which can be deselected */
    bool canSelectNone() const override;

    /** Determines whether the item selection can be inverted (more than one) */
    bool canSelectInvert() const override;

    /** Select all items */
    void selectAll() override;

    /** Deselect all items */
    void selectNone() override;

    /** Invert item selection */
    void selectInvert() override;

public:

    /**
     * Get scalar image data for \p dimensionIndex, populates \p scalarData and \p establishes the \p scalarDataRange
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension (assumes enough elements are allocated by the caller)
     * @param scalarDataRange Scalar data range
     */
    void getScalarData(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get scalar image data for \p dimensionIndices, populates \p scalarData and \p establishes the \p scalarDataRange
     * @param dimensionIndices Dimension indices to retrieve the scalar data for
     * @param scalarData Scalar data for the specified dimension (assumes enough elements are allocated by the caller)
     * @param scalarDataRange Scalar data range
     */
    void getScalarData(const std::vector<std::uint32_t>& dimensionIndices, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get scalar image data for \p imageIndex
     * @param imageIndex Index of the image to retrieve the scalar data for
     * @param scalarData Scalar data for the specified dimension (assumes enough elements are allocated by the caller)
     * @param scalarDataRange Scalar data range
     */
    void getImageScalarData(std::uint32_t imageIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get mask image data (for subsets)
     * @param maskData Mask scalar data
     */
    void getMaskData(std::vector<std::uint8_t>& maskData);

    /**
     * Set mask image data
     * @param maskData Mask scalar data
     */
    void setMaskData(const std::vector<std::uint8_t>& maskData);

    /**
     * Set mask image data
     * @param maskData Mask scalar data
     */
    void setMaskData(std::vector<std::uint8_t>&& maskData);

    /**
     * Clears mask data
     */
    void resetMaskData();

    /**
     * Get selection data
     * @param selectionImageData Image data for the selection
     * @param selectedIndices Selected pixel indices
     * @param selectionBoundaries Boundaries of the selection (in image coordinates)
     */
    void getSelectionData(std::vector<std::uint8_t>& selectionImageData, std::vector<std::uint32_t>& selectedIndices, QRect& selectionBoundaries);

protected:

    /**
     * Get image sequence scalar data for \p dimensionIndex, populate \p scalarData and establish the \p scalarDataRange
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarDataForImageSequence(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get image stack scalar data for \p dimensionIndex, populate \p scalarData and establish the \p scalarDataRange
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarDataForImageStack(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /** Computes and caches the mask data, if mask data is not set by setMaskData, based on linked data set to parent's points */
    void computeMaskData();

    /**
     * Get pixel coordinate from pixel index
     * @param pixelIndex Pixel index
     */
    QPoint getPixelCoordinateFromPixelIndex(const std::int32_t& pixelIndex) const;

    /**
     * Get pixel index from pixel coordinate
     * @param pixelCoordinate Pixel coordinate
     * @return Pixel index
     */
    std::int32_t getPixelIndexFromPixelCoordinate(const QPoint& pixelCoordinate) const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private: // Internal Helper
    /**
     * Sets _visibleRectangle based on _maskData
     */
    void updateVisibleRectangle();

private:
    std::vector<std::uint32_t>      _indices;               /** Selection indices */
    ImageData*                      _imageData;             /** Pointer to raw image data */
    QSharedPointer<InfoAction>      _infoAction;            /** Shared pointer to info action */
    QRect                           _visibleRectangle;      /** Rectangle which bounds the visible pixels */
    std::vector<std::uint8_t>       _maskData;              /** Mask data */
    bool                            _maskDataGiven;         /** Whether mask data was set externally */
};
