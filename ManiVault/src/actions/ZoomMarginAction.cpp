// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ZoomMarginAction.h"

#include "GroupSectionTreeItem.h"
#include "NavigationAction.h"

#include "util/StyledIcon.h"

#ifdef _DEBUG
    //#define ZOOM_MARGIN_ACTION_VERBOSE
#endif

//#define ZOOM_MARGIN_ACTION_VERBOSE

using namespace mv::util;

namespace mv::gui
{

const QStringList ZoomMarginAction::marginTypeOptions = { "Screen", "Data" };

ZoomMarginAction::ZoomMarginAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _marginTypeAction(this, "Margin type", marginTypeOptions, marginTypeOptions.last()),
    _zoomMarginScreenAction(this, "Screen", .0f, 100.f, 25.f),
    _zoomMarginDataAction(this, "Data", .0f, 100.f, 10.f)
{
    setIconByName("arrows-up-to-line");
    setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    setPopupSizeHint(QSize(300, 0));

	addAction(&_marginTypeAction);
	addAction(&_zoomMarginScreenAction);
	addAction(&_zoomMarginDataAction);

    _marginTypeAction.setToolTip("Zoom margin type");

    _zoomMarginScreenAction.setToolTip("Zoom margin in absolute widget pixels");
    _zoomMarginScreenAction.setSuffix("px");
    _zoomMarginScreenAction.setUpdateDuringDrag(true);

    _zoomMarginDataAction.setToolTip("Zoom margin in percentage of the data");
    _zoomMarginDataAction.setSuffix("%");
    _zoomMarginDataAction.setUpdateDuringDrag(true);

    const auto updateReadOnly = [this]() -> void {
        const auto currentTypeIndex = _marginTypeAction.getCurrentIndex();

        _zoomMarginScreenAction.setEnabled(currentTypeIndex == 0);
        _zoomMarginDataAction.setEnabled(currentTypeIndex == 1);
	};

    updateReadOnly();

    connect(&_marginTypeAction, &OptionAction::currentIndexChanged, this, updateReadOnly);
}

void ZoomMarginAction::fromVariantMap(const QVariantMap& variantMap)
{
    VerticalGroupAction::fromVariantMap(variantMap);

    _marginTypeAction.fromParentVariantMap(variantMap, true);
    _zoomMarginScreenAction.fromParentVariantMap(variantMap, true);
    _zoomMarginDataAction.fromParentVariantMap(variantMap, true);
}

QVariantMap ZoomMarginAction::toVariantMap() const
{
    auto variantMap = VerticalGroupAction::toVariantMap();

    _marginTypeAction.insertIntoVariantMap(variantMap);
    _zoomMarginScreenAction.insertIntoVariantMap(variantMap);
    _zoomMarginDataAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
