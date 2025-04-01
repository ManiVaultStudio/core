// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NavigationAction.h"

#ifdef _DEBUG
    //#define NAVIGATION_ACTION_VERBOSE
#endif

//#define NAVIGATION_ACTION_VERBOSE

using namespace mv::util;

namespace mv::gui
{

NavigationAction::NavigationAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _zoomOutAction(this, "Zoom out"),
    _zoomPercentageAction(this, "Zoom Percentage", 0.01f, 1000.0f, 100.0f, 1),
    _zoomInAction(this, "Zoom In"),
    _zoomExtentsAction(this, "Zoom All"),
    _zoomSelectionAction(this, "Zoom Selection"),
    _zoomRegionAction(this, "Zoom Region"),
    _zoomRectangleAction(this, "Zoom Rectangle")
{
    setShowLabels(false);

    _zoomOutAction.setToolTip("Zoom out by 10% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 10%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of the scene (z)");
    _zoomSelectionAction.setToolTip("Zoom to the boundaries of the current selection (d)");

    _zoomPercentageAction.setOverrideSizeHint(QSize(500, 0));

    _zoomOutAction.setIconByName("search-minus");
    _zoomInAction.setIconByName("search-plus");
    _zoomExtentsAction.setIconByName("compress");
    _zoomSelectionAction.setIconByName("search-location");

    _zoomOutAction.setShortcut(QKeySequence("-"));
    _zoomInAction.setShortcut(QKeySequence("+"));
    _zoomExtentsAction.setShortcut(QKeySequence("z"));
    _zoomSelectionAction.setShortcut(QKeySequence("d"));

    _zoomSelectionAction.setEnabled(false);

    _zoomPercentageAction.setSuffix("%");
    _zoomPercentageAction.setUpdateDuringDrag(false);

    _zoomRectangleAction.setToolTip("Extents of the current view");
    _zoomRectangleAction.setIcon(combineIcons(StyledIcon("expand"), StyledIcon("ellipsis-h")));
    _zoomRectangleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

	addAction(&_zoomOutAction, gui::TriggerAction::Icon);
	addAction(&_zoomPercentageAction);
	addAction(&_zoomInAction, gui::TriggerAction::Icon);
	addAction(&_zoomExtentsAction);
	//addAction(&_zoomSelectionAction);
	//addAction(&_zoomRegionAction);
}

void NavigationAction::fromVariantMap(const QVariantMap& variantMap)
{
	HorizontalGroupAction::fromVariantMap(variantMap);

    _zoomRectangleAction.fromParentVariantMap(variantMap);
}

QVariantMap NavigationAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _zoomRectangleAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
