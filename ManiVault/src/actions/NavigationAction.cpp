// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NavigationAction.h"

#ifdef _DEBUG
    //#define NAVIGATION_ACTION_VERBOSE
#endif

//#define NAVIGATION_ACTION_VERBOSE

namespace mv::gui
{

NavigationAction::NavigationAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _zoomOutAction(this, "Zoom out"),
    _zoomPercentageAction(this, "Zoom Percentage", 10.0f, 1000.0f, 100.0f, 1),
    _zoomInAction(this, "Zoom In"),
    _zoomExtentsAction(this, "Zoom All"),
    _zoomSelectionAction(this, "Zoom Around Selection"),
    _zoomRegionAction(this, "Zoom Region")
{
    setShowLabels(false);

    _zoomOutAction.setToolTip("Zoom out by 10% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 10%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of the scene (z)");
    _zoomSelectionAction.setToolTip("Zoom to the boundaries of the current selection (d)");

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

	addAction(&_zoomOutAction, gui::TriggerAction::Icon);
	addAction(&_zoomPercentageAction);
	addAction(&_zoomInAction, gui::TriggerAction::Icon);
	addAction(&_zoomExtentsAction, gui::TriggerAction::Icon);
	addAction(&_zoomSelectionAction, gui::TriggerAction::Icon);
}

}
