// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMap.h"

namespace mv {

namespace util {

ColorMap::ColorMap(const QString& name /*= ""*/, const QString& resourcePath /*= ""*/, const Type& type /*= Type::OneDimensional*/, const QImage& image /*= QImage()*/) :
    _name(name),
    _resourcePath(resourcePath),
    _type(type),
    _image(image)
{
}

QString ColorMap::getName() const
{
    return _name;
}

QString ColorMap::getResourcePath() const
{
    return _resourcePath;
}

ColorMap::Type ColorMap::getType() const
{
    return _type;
}

QImage ColorMap::getImage() const
{
    return _image;
}

std::uint32_t ColorMap::getNoDimensions() const
{
    return static_cast<int>(_type);
}


}
}
