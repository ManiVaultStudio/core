// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ImageData.h"
#include "Images.h"
#include "Application.h"

#include "DataHierarchyItem.h"
#include "PointData/PointData.h"

#include "util/Exception.h"

#include <QDebug>

#include <stdexcept>

Q_PLUGIN_METADATA(IID "studio.manivault.ImageData")

using namespace mv;
using namespace mv::util;

ImageData::ImageData(const mv::plugin::PluginFactory* factory) :
    mv::plugin::RawData(factory, ImageType),
    _type(Type::Undefined),
    _numberOfImages(0),
    _imageOffset(),
    _imageSize(),
    _numberOfComponentsPerPixel(0),
    _imageFilePaths(),
    _dimensionNames()
{
}

void ImageData::init()
{
}

ImageData::Type ImageData::getType() const
{
    return _type;
}

void ImageData::setType(const ImageData::Type& type)
{
    _type = type;
}

QSize ImageData::getImageSize() const
{
    return _imageSize;
}

void ImageData::setImageSize(const QSize& imageSize)
{
    _imageSize = imageSize;
}

std::uint32_t ImageData::getNumberOfComponentsPerPixel() const
{
    return _numberOfComponentsPerPixel;
}

void ImageData::setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel)
{
    _numberOfComponentsPerPixel = numberOfComponentsPerPixel;
}

QStringList ImageData::getImageFilePaths() const
{
    return _imageFilePaths;
}

void ImageData::setImageFilePaths(const QStringList& imageFilePaths)
{
    _imageFilePaths = imageFilePaths;
}

std::uint32_t ImageData::getNumberOfImages() const
{
    return _numberOfImages;
}

void ImageData::setNumberImages(const std::uint32_t& numberOfImages)
{
    _numberOfImages = numberOfImages;
}

Dataset<DatasetImpl> ImageData::createDataSet(const QString& guid /*= ""*/) const
{
    return Dataset<DatasetImpl>(new Images(getName(), false, guid));
}

ImageDataFactory::ImageDataFactory()
{
    setIconByName("images");
}

QUrl ImageDataFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/ImageData/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/ImageData/README.md");
#endif
}

QUrl ImageDataFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

mv::plugin::RawData* ImageDataFactory::produce()
{
    return new ImageData(this);
}
