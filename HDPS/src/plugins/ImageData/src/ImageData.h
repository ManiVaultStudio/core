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

class IMAGEDATA_EXPORT ImageData : public hdps::RawData
{
public:
    enum Type
    {
        Undefined,
        Sequence,
        Stack,
        MultiPartSequence
    };

    static QString typeName(const Type& type)
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
    ImageData();

    void init() override;

public:
    Type type() const;
    void setType(const Type& type);
    std::uint32_t noImages() const;
    void setNoImages(const std::uint32_t& noImages);
    QSize imageSize() const;
    void setImageSize(const QSize& imageSize);
    std::uint32_t noComponents() const;
    void setNoComponents(const std::uint32_t& noComponents);
    std::vector<QString> imageFilePaths() const;
    void setImageFilePaths(const std::vector<QString>& imageFilePaths);
    std::vector<QString> dimensionNames() const;
    void setDimensionNames(const std::vector<QString>& dimensionNames);

    QString pointsName() const;
    Points* points() const;

public:
    std::uint32_t noPoints() const;
    std::uint32_t noDimensions() const;
    
    float pointValue(const std::uint32_t& index) const;
    float pointValue(const std::uint32_t& x, const std::uint32_t& y) const;

    hdps::DataSet* createDataSet() const override;

private:
    Type                    _type;
    std::uint32_t           _noImages;
    QSize                   _imageSize;
    std::uint32_t           _noComponents;
    std::vector<QString>    _imageFilePaths;
    std::vector<QString>    _dimensionNames;
    QString                 _pointsName;
    Points*                 _points;
};

class ImageDataFactory : public hdps::plugin::RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ImageData"
                      FILE  "ImageData.json")
    
public:
    ImageDataFactory() {}
    ~ImageDataFactory() override {}
    
    hdps::RawData* produce() override;
};