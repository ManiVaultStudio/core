// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VariantAction.h"

using namespace hdps::util;

namespace hdps::gui {

VariantAction::VariantAction(QObject* parent, const QString& title, const QVariant& variant /*= QVariant()*/) :
    WidgetAction(parent, title),
    _variant()
{
    setText(title);
    initialize(variant);
}

void VariantAction::initialize(const QVariant& variant /*= QVariant()*/)
{
    setVariant(variant);
}

QVariant VariantAction::getVariant() const
{
    return _variant;
}

void VariantAction::setVariant(const QVariant& variant)
{
    if (variant == _variant)
        return;

    _variant = variant;

    emit variantChanged(_variant);

    saveToSettings();
}

void VariantAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicVariantAction = dynamic_cast<VariantAction*>(publicAction);

    Q_ASSERT(publicVariantAction != nullptr);

    connect(this, &VariantAction::variantChanged, publicVariantAction, &VariantAction::setVariant);
    connect(publicVariantAction, &VariantAction::variantChanged, this, &VariantAction::setVariant);

    setVariant(publicVariantAction->getVariant());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void VariantAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicVariantAction = dynamic_cast<VariantAction*>(getPublicAction());

    Q_ASSERT(publicVariantAction != nullptr);

    if (publicVariantAction == nullptr)
        return;

    disconnect(this, &VariantAction::variantChanged, publicVariantAction, &VariantAction::setVariant);
    disconnect(publicVariantAction, &VariantAction::variantChanged, this, &VariantAction::setVariant);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void VariantAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setVariant(variantMap["Value"]);
}

QVariantMap VariantAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getVariant() }
    });

    return variantMap;
}

}
