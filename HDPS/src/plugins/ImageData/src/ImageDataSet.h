#pragma once

#include "Common.h"
#include "Image.h"

#include <Set.h>

#include <QString>
#include <QImage>
#include <QColor>
#include <QRect>
#include <QSize>

#include <vector>

using namespace hdps::plugin;

class Points;
class ImageData;

class IMAGEDATA_EXPORT ImageDataSet : public hdps::DataSet
{
public:
    ImageDataSet(hdps::CoreInterface* core, QString dataName);
    ~ImageDataSet() override { }

    virtual void createSubset() const override;
    
    void setSequence(const std::vector<Image>& images, const QSize& size);
    void setStack(const std::vector<Image>& images, const QSize& size);

    std::shared_ptr<QImage> sequenceImage(const std::vector<std::uint32_t>& imageIds);
    std::shared_ptr<QImage> stackImage(const std::uint32_t& imageId);
    std::shared_ptr<QImage> selectionImage(const QColor& color = QColor(255, 0, 0, 255)) const;

    void selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier);
    QRect selectionBounds(const bool& relative = false) const;

    hdps::DataSet* copy() const override;

    QRect roi() const;
    void setRoi(const QRect& roi);
    QSize imageSize() const;
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