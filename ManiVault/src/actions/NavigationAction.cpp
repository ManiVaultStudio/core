// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NavigationAction.h"

#include "GroupSectionTreeItem.h"

#include "util/StyledIcon.h"
#include "util/Icon.h"

#ifdef _DEBUG
    //#define NAVIGATION_ACTION_VERBOSE
#endif

//#define NAVIGATION_ACTION_VERBOSE

using namespace mv::util;

namespace mv::gui
{

NavigationAction::NavigationAction(QObject* parent, const QString& title) :
    HorizontalToolbarAction(parent, title),
    _zoomOutAction(this, "Zoom out"),
    _zoomPercentageAction(this, "Zoom Percentage", 0.01f, 1000.0f, 100.0f, 1),
    _zoomInAction(this, "Zoom In"),
    _zoomExtentsAction(this, "Zoom All"),
    _zoomSelectionAction(this, "Zoom Selection"),
    _zoomRegionAction(this, "Zoom Region"),
    _freezeNavigation(this, "Freeze Navigation"),
    _zoomRectangleAction(this, "Zoom Rectangle"),
    _zoomCenterAction(this, "Zoom Center"),
    _zoomFactorAction(this, "Zoom Factor"),
    _zoomMarginAction(this, "Zoom margin"),
    _zoomGroupAction(this, "Zoom group")
{
    //setShowLabels(false);

    _zoomOutAction.setToolTip("Zoom out by 10% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 10%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of the scene (o)");
    _zoomSelectionAction.setToolTip("Zoom to the boundaries of the current selection (b)");
    _zoomRegionAction.setToolTip("Zoom to a picked region");
    _freezeNavigation.setToolTip("Freeze the navigation");

    _freezeNavigation.setIconByName("icicles");
    _freezeNavigation.setDefaultWidgetFlags(ToggleAction::WidgetFlag::CheckBox);

    _zoomPercentageAction.setOverrideSizeHint(QSize(300, 0));
    _zoomPercentageAction.setConnectionPermissionsToAll();

    _zoomOutAction.setIconByName("search-minus");
    _zoomInAction.setIconByName("search-plus");
    _zoomExtentsAction.setIconByName("compress");

    _zoomSelectionAction.setIcon(StyledIcon("compress").withModifier("mouse-pointer"));
    _zoomSelectionAction.setEnabled(false);

    _zoomRegionAction.setIcon(StyledIcon("compress").withModifier("search"));

    _zoomPercentageAction.setSuffix("%");
    _zoomPercentageAction.setUpdateDuringDrag(false);

    _zoomRectangleAction.setToolTip("Extents of the current view");
    _zoomRectangleAction.setIcon(combineIcons(StyledIcon("expand"), StyledIcon("ellipsis-h")));
    _zoomRectangleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _zoomCenterAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _zoomCenterAction.setIconByName("ruler");
    _zoomCenterAction.setDefaultWidgetFlags(GroupAction::WidgetFlag::Vertical);
    _zoomCenterAction.setPopupSizeHint(QSize(250, 0));
    _zoomCenterAction.setConnectionPermissionsToAll(true);

    _zoomCenterAction.getXAction().setDefaultWidgetFlags(DecimalAction::WidgetFlag::SpinBox);
    _zoomCenterAction.getYAction().setDefaultWidgetFlags(DecimalAction::WidgetFlag::SpinBox);

    _zoomMarginAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _zoomGroupAction.setShowLabels(false);
    _zoomGroupAction.setIconByName("magnifying-glass");

    _zoomGroupAction.addAction(&_zoomOutAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_zoomPercentageAction);
    _zoomGroupAction.addAction(&_zoomInAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_zoomCenterAction);
    _zoomGroupAction.addAction(&_zoomExtentsAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_zoomSelectionAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_zoomRegionAction, TriggerAction::Icon);
    
    addAction(&_zoomGroupAction, 1);
    addAction(&_freezeNavigation, 50);
    addAction(&_zoomMarginAction);

    const auto updateReadOnly = [this]() -> void {
        const auto notFrozen = _freezeNavigation.isChecked();

        _zoomOutAction.setEnabled(!notFrozen);
        _zoomPercentageAction.setEnabled(!notFrozen);
        _zoomInAction.setEnabled(!notFrozen);
        _zoomExtentsAction.setEnabled(!notFrozen);
        _zoomSelectionAction.setEnabled(!notFrozen);
        _zoomRegionAction.setEnabled(!notFrozen);
        _zoomCenterAction.setEnabled(!notFrozen);
    };

    updateReadOnly();

    connect(&_freezeNavigation, &ToggleAction::toggled, this, updateReadOnly);
}

void NavigationAction::setShortcutsEnabled(bool shortcutsEnabled)
{
    _zoomOutAction.setShortcut(shortcutsEnabled ? QKeySequence("-") : QKeySequence());
    _zoomInAction.setShortcut(shortcutsEnabled ? QKeySequence("+") : QKeySequence());
    _zoomExtentsAction.setShortcut(shortcutsEnabled ? QKeySequence("O") : QKeySequence());
    _zoomSelectionAction.setShortcut(shortcutsEnabled ? QKeySequence("H") : QKeySequence());
    _zoomRegionAction.setShortcut(shortcutsEnabled ? QKeySequence("F") : QKeySequence());
}

void NavigationAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalToolbarAction::fromVariantMap(variantMap);

    _zoomCenterAction.fromParentVariantMap(variantMap);
    _zoomFactorAction.fromParentVariantMap(variantMap);
    _zoomPercentageAction.fromParentVariantMap(variantMap);
    _zoomMarginAction.fromParentVariantMap(variantMap, true);
}

QVariantMap NavigationAction::toVariantMap() const
{
    auto variantMap = HorizontalToolbarAction::toVariantMap();

    _zoomCenterAction.insertIntoVariantMap(variantMap);
    _zoomFactorAction.insertIntoVariantMap(variantMap);
    _zoomPercentageAction.insertIntoVariantMap(variantMap);
    _zoomMarginAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
