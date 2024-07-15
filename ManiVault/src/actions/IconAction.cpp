// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IconAction.h"

using namespace mv::util;

namespace mv::gui {

IconAction::IconAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _icon()
{
}

QIcon IconAction::getIcon() const
{
    return _icon;
}

void IconAction::setIcon(const QIcon& icon)
{
    _icon = icon;

    emit iconChanged(_icon);
}

void IconAction::setIconFromImage(const QImage& image)
{
    setIcon(createIcon(QPixmap::fromImage(image)));
}

void IconAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    const auto rawDataMap = variantMap["RawData"].toMap();

    variantMapMustContain(rawDataMap, "RawData");

    QByteArray iconByteArray;

    QDataStream iconDataStream(&iconByteArray, QIODevice::ReadOnly);

    const auto clustersRawDataSize = rawDataMap["ClustersRawDataSize"].toInt();

    iconByteArray.resize(clustersRawDataSize);

    populateDataBufferFromVariantMap(rawDataMap["ClustersRawData"].toMap(), (char*)iconByteArray.data());

    //QImage image;

    //image.loadFromData(QByteArray::fromBase64(variantMap["Value"].toByteArray()));

    //setImage(image);

    //_filePathAction.fromParentVariantMap(variantMap);
    //_fileNameAction.fromParentVariantMap(variantMap);
}

QVariantMap IconAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    /*QByteArray previewImageByteArray;
    QBuffer previewImageBuffer(&previewImageByteArray);

    _image.save(&previewImageBuffer, "PNG");

    variantMap.insert({
        { "Value", QVariant::fromValue(previewImageByteArray.toBase64()) }
        });

    _filePathAction.insertIntoVariantMap(variantMap);
    _fileNameAction.insertIntoVariantMap(variantMap);*/

    return variantMap;
}

IconAction::Widget::Widget(QWidget* parent, IconAction* iconAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, iconAction, widgetFlags)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(&_iconLabel);

    setLayout(layout);

    _iconLabel.setPixmap(iconAction->getIcon().pixmap(12, 12));
}

}
