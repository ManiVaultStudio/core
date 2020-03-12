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

class IMAGEDATA_EXPORT Images : public hdps::DataSet
{
public:
    Images(hdps::CoreInterface* core, QString dataName);

    virtual void createSubset() const override;
    
    void setSequence(const std::vector<Image>& images, const QSize& size);
    void setStack(const std::vector<Image>& images, const QSize& size);

    QImage sequenceImage(const std::vector<std::uint32_t>& imageIds);
    QImage stackImage(const std::uint32_t& imageId);
    QImage selectionImage() const;

    void selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier);
    QRect selectionBounds(const bool& relative = false) const;

    hdps::DataSet* copy() const override;

    ImageData::Type type() const;
    std::uint32_t noImages() const;
    QSize imageSize() const;
    std::uint32_t noComponents() const;
    std::vector<QString> imageFilePaths() const;
    std::vector<QString> dimensionNames() const;

    QRect roi() const;
    void setRoi(const QRect& roi);
    std::uint32_t noPixels() const;
    std::uint32_t noSelectedPixels() const;

    static std::uint32_t noChannelsPerPixel();

    QString pointsName();
    Points* points();
    void setPoints(Points* points);

    std::vector<std::uint32_t>& indices();
    void setIndices(const std::vector<std::uint32_t>& indices);

private:
    ImageData*  _imageData;
    Points*     _points;
    QRect       _roi;
};