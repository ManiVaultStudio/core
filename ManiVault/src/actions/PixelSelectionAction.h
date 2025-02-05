// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"
#include "ColorAction.h"
#include "DecimalAction.h"
#include "OptionAction.h"
#include "TriggerAction.h"
#include "ToggleAction.h"
#include "GroupAction.h"

#include "util/PixelSelectionTool.h"

#include <QActionGroup>

class QWidget;

namespace mv::gui {

/**
 * Pixel selection action class
 *
 * Action class for pixel selection
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PixelSelectionAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE PixelSelectionAction(QObject* parent, const QString& title);

    /**
     * Initialize the pixel selection action
     * @param targetWidget Pointer to target widget (pixel selection tool will be drawn on top of it)
     * @param pixelSelectionTool Pointer to pixel selection tool
     * @param pixelSelectionTypes Allowed pixel selection types
     */
    void initialize(QWidget* targetWidget, util::PixelSelectionTool* pixelSelectionTool, const util::PixelSelectionTypes& pixelSelectionTypes = util::defaultPixelSelectionTypes);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu();

    /** Get pointer to the target on which the pixel selection tool operates */
    QWidget* getTargetWidget();

    /** Get pointer to pixel selection tool */
    util::PixelSelectionTool* getPixelSelectionTool();

    /**
     * Enables/disables shortcuts
     * @param shortcutsEnabled Whether shortcuts are enabled or not
     */
    void setShortcutsEnabled(const bool& shortcutsEnabled);

private:

    /** Perform selection type initialization */
    void initType();

    /** Perform selection modifier initialization */
    void initModifier();

    /** Perform miscellaneous initialization */
    void initMiscellaneous();

    /**
     * Get whether the pixel selection tool is initialized with a target widget and pixel selection tool
     * @return Boolean determining whether the pixel selection tool is initialized with a target widget and pixel selection tool
     */
    bool isInitialized() const;

public: // Event handling

    /**
     * Listens to the events of target \p object
     * @param object Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* object, QEvent* event) override;

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load selection action from variant
     * @param Variant representation of the selection action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save selection action to variant
     * @return Variant representation of the selection action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    ColorAction& getOverlayColorAction() { return _overlayColorAction; }
    DecimalAction& getOverlayOpacityAction() { return _overlayOpacityAction; }
    OptionAction& getTypeAction() { return _typeAction; }
    ToggleAction& getRectangleAction() { return _rectangleAction; }
    ToggleAction& getLineAction() { return _lineAction; }
    ToggleAction& getBrushAction() { return _brushAction; }
    ToggleAction& getLassoAction() { return _lassoAction; }
    ToggleAction& getPolygonAction() { return _polygonAction; }
    ToggleAction& getSampleAction() { return _sampleAction; }
    ToggleAction& getRoiAction() { return _roiAction; }
    OptionAction& getModifierAction() { return _modifierAction; }
    ToggleAction& getModifierReplaceAction() { return _modifierReplaceAction; }
    ToggleAction& getModifierAddAction() { return _modifierAddAction; }
    ToggleAction& getModifierSubtractAction() { return _modifierSubtractAction; }
    GroupAction& getSelectAction() { return _selectAction; }
    TriggerAction& getClearSelectionAction() { return _clearSelectionAction; }
    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getInvertSelectionAction() { return _invertSelectionAction; }
    DecimalAction& getBrushRadiusAction() { return _brushRadiusAction; }
    DecimalAction& getLineWidthAction() { return _lineWidthAction; }
    DecimalAction& getLineAngleAction() { return _lineAngleAction; }
    ToggleAction& getNotifyDuringSelectionAction() { return _notifyDuringSelectionAction; }

private:
    bool                            _initialized;                       /** Whether the pixel selection tool is initialized with a target widget and pixel selection tool */
    QWidget*                        _targetWidget;                      /** Pointer to target widget */
    util::PixelSelectionTool*       _pixelSelectionTool;                /** Pointer to pixel selection tool */
    util::PixelSelectionTypes       _pixelSelectionTypes;               /** Pixel selection types */
    ColorAction                     _overlayColorAction;                /** Selection overlay color action */
    DecimalAction                   _overlayOpacityAction;              /** Selection overlay opacity action */
    util::PixelSelectionTypeModel   _typeModel;                         /** Selection type model */
    OptionAction                    _typeAction;                        /** Selection type action */
    ToggleAction                    _rectangleAction;                   /** Switch to rectangle selection action */
    ToggleAction                    _lineAction;                        /** Switch to line selection action */
    ToggleAction                    _brushAction;                       /** Switch to brush selection action */
    ToggleAction                    _lassoAction;                       /** Switch to lasso selection action */
    ToggleAction                    _polygonAction;                     /** Switch to polygon selection action */
    ToggleAction                    _sampleAction;                      /** Switch to sample selection action */
    ToggleAction                    _roiAction;                         /** Switch to ROI selection action */
    QActionGroup                    _typeActionGroup;                   /** Type action group */
    OptionAction                    _modifierAction;                    /** Modifier action */
    ToggleAction                    _modifierReplaceAction;             /** Replace current selection action */
    ToggleAction                    _modifierAddAction;                 /** Add to current selection action */
    ToggleAction                    _modifierSubtractAction;            /** Subtract from current selection action */
    GroupAction                     _selectAction;                      /** Select actions group */
    TriggerAction                   _clearSelectionAction;              /** Clear selection action */
    TriggerAction                   _selectAllAction;                   /** Select all action */
    TriggerAction                   _invertSelectionAction;             /** Invert selection action */
    DecimalAction                   _brushRadiusAction;                 /** Brush radius action */
    ToggleAction                    _notifyDuringSelectionAction;       /** Notify during selection action */
    DecimalAction                   _lineWidthAction;                   /** Line width action */
    DecimalAction                   _lineAngleAction;                   /** Line angle action */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::PixelSelectionAction)

inline const auto pixelSelectionActionMetaTypeId = qRegisterMetaType<mv::gui::PixelSelectionAction*>("mv::gui::PixelSelectionAction");
