// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapAxisAction.h"
#include "ColorMapAction.h"
#include "ColorMapSettingsAction.h"

#include <Application.h>

#include <QGridLayout>
#include <QGroupBox>

using namespace mv::util;

namespace mv::gui {

ColorMapAxisAction::ColorMapAxisAction(ColorMapSettingsAction& colorMapSettingsAction, const QString& title) :
    WidgetAction(&colorMapSettingsAction, "Axis"),
    _rangeAction(this, "Range", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1),
    _mirrorAction(this, "Mirror")
{
    setText(title);
    setSerializationName("Axis");

    _rangeAction.setSerializationName("Range");
    _mirrorAction.setSerializationName("Mirror");

    _rangeAction.setToolTip("Range of the color map");
    _mirrorAction.setToolTip("Mirror the color map");
}

void ColorMapAxisAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapAxisAction = dynamic_cast<ColorMapAxisAction*>(publicAction);

    Q_ASSERT(publicColorMapAxisAction != nullptr);

    _mirrorAction.connectToPublicAction(&publicColorMapAxisAction->getMirrorAction());

    WidgetAction::connectToPublicAction(publicAction);
}

void ColorMapAxisAction::disconnectFromPublicAction()
{
    _mirrorAction.disconnectFromPublicAction();

    WidgetAction::disconnectFromPublicAction();
}

void ColorMapAxisAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _rangeAction.fromParentVariantMap(variantMap);
    _mirrorAction.fromParentVariantMap(variantMap);
}

QVariantMap ColorMapAxisAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _rangeAction.insertIntoVariantMap(variantMap);
    _mirrorAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ColorMapAxisAction::Widget::Widget(QWidget* parent, ColorMapAxisAction* colorMapAxisAction) :
    WidgetActionWidget(parent, colorMapAxisAction)
{
    auto layout = new QGridLayout();

    auto& rangeAction = colorMapAxisAction->getRangeAction();

    auto& rangeMinAction = rangeAction.getRangeMinAction();
    auto& rangeMaxAction = rangeAction.getRangeMaxAction();

    layout->addWidget(rangeMinAction.createLabelWidget(this), 0, 0);
    layout->addWidget(rangeMinAction.createWidget(this), 0, 1);

    layout->addWidget(rangeMaxAction.createLabelWidget(this), 1, 0);
    layout->addWidget(rangeMaxAction.createWidget(this), 1, 1);

    layout->addWidget(colorMapAxisAction->getMirrorAction().createWidget(this), 2, 1);

    layout->setColumnStretch(1, 1);

    setPopupLayout(layout);
}

}
