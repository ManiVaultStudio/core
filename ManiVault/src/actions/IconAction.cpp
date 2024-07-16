// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IconAction.h"

#include <QBuffer>

using namespace mv::util;

namespace mv::gui {

IconAction::IconAction(QObject* parent, const QString& title) :
    TriggerAction(parent, title)
{
    setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    setIconByName("exclamation-circle");
}

void IconAction::setIconFromImage(const QImage& image)
{
    setIcon(createIcon(QPixmap::fromImage(image)));
}

void IconAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    QPixmap pixmap;

    pixmap.loadFromData(QByteArray::fromBase64(variantMap["Value"].toByteArray()));

    setIcon(createIcon(pixmap));
}

QVariantMap IconAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    QByteArray previewImageByteArray;
    QBuffer previewImageBuffer(&previewImageByteArray);

    icon().pixmap(QSize(32, 32)).save(&previewImageBuffer, "PNG");

    variantMap.insert({
    { "Value", QVariant::fromValue(previewImageByteArray.toBase64()) }
    });

    return variantMap;
}

}
