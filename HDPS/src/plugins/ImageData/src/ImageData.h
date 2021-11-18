#pragma once

#include "Common.h"
#include "Image.h"

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QSize>
#include <QDebug>

#include <vector>

class Points;

const hdps::DataType ImageType = hdps::DataType(QString("Images"));

class IMAGEDATA_EXPORT ImageData : public hdps::plugin::RawData
{
public:
    enum Type
    {
        Undefined = -1,
        Sequence,
        Stack,
        MultiPartSequence
    };

    static QString getTypeName(const Type& type)
    {
        switch (type)
        {
            case Type::Sequence:
                return "Sequence";

            case Type::Stack:
                return "Stack";

            case Type::MultiPartSequence:
                return "MultiPartSequence";

            default:
                break;
        }

        return "";
    }

public:
    ImageData(const hdps::plugin::PluginFactory* factory);

    void init() override;

public:

    /** Gets the image collection type e.g. stack or sequence */
    Type getType() const;

    /**
     * Sets the image collection type e.g. stack or sequence
     * @param type Image collection type
     */
    void setType(const Type& type);

    /** Gets the number of images */
    std::uint32_t getNumberOfImages() const;

    /**
     * Sets the number of images
     * @param numberOfImages Number of images
     */
    void setNumberImages(const std::uint32_t& numberOfImages);

    /** Get the image size */
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
     * @param numberOfPixelComponents Number of pixel components
     */
    void setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel);

    /** Gets the image file paths */
    QStringList getImageFilePaths() const;

    /**
     * Sets the image file paths
     * @param imageFilePaths Image file paths
     */
    void setImageFilePaths(const QStringList& imageFilePaths);

    /**
     * Create images dataset
     * @return Smart pointer to dataset
     */
    hdps::Dataset<hdps::DatasetImpl> createDataSet() const override;

private:
    Type                _type;                          /** Image collection type e.g. stack or sequence */
    std::uint32_t       _numberOfImages;                /** Number of images in the set */
    QPoint              _imageOffset;                   /** Image offset */
    QRect               _sourceRectangle;               /** Source image rectangle */
    QRect               _targetRectangle;               /** Target image rectangle */
    std::uint32_t       _numberOfComponentsPerPixel;    /** Number of components per pixel */
    QStringList         _imageFilePaths;                /** Image file paths */
    QStringList         _dimensionNames;                /** Dimension names */
};

class ImageDataFactory : public hdps::plugin::RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.ImageData" FILE "ImageData.json")

public:
    ImageDataFactory() {}
    ~ImageDataFactory() override {}

    /** Returns the plugin icon */
    QIcon getIcon() const override;

    hdps::plugin::RawData* produce() override;
};
