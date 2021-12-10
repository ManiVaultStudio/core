#pragma once

#include "util/PixelSelectionTool.h"

#include "WidgetAction.h"
#include "ColorAction.h"
#include "DecimalAction.h"
#include "OptionAction.h"
#include "TriggerAction.h"
#include "ToggleAction.h"

class QWidget;

namespace hdps {

namespace gui {

/**
 * Pixel selection action class
 *
 * Action class for pixel selection
 *
 * @author Thomas Kroes
 */
class PixelSelectionAction : public WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param targetWidget Pointer to target widget
     * @param pixelSelectionTool Reference to pixel selection tool
     * @param pixelSelectionTypes Allowed pixel selection types
     */
    PixelSelectionAction(QObject* parent, QWidget* targetWidget, util::PixelSelectionTool& pixelSelectionTool, const util::PixelSelectionTypes& pixelSelectionTypes = util::defaultPixelSelectionTypes);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu();

    /** Get pointer to the target on which the pixel selection tool operates */
    QWidget* getTargetWidget();

    /** Get reference to pixel selection tool */
    util::PixelSelectionTool& getPixelSelectionTool();

    /**
     * Enables/disables shortcuts
     * @param shortcutsEnabled Whether shortcuts are enabled or not
     */
    void setShortcutsEnabled(const bool& shortcutsEnabled);

protected:

    /** Perform selection overlay initialization */
    void initOverlay();

    /** Perform selection type initialization */
    void initType();

    /** Perform selection modifier initialization */
    void initModifier();

    /** Perform selection operations initialization */
    void initOperations();

    /** Perform miscellaneous initialization */
    void initMiscellaneous();

public: // Event handling

    /**
     * Listens to the events of target \p object
     * @param object Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* object, QEvent* event) override;

public: // Action getters

    ColorAction& getOverlayColor() { return _overlayColor; }
    DecimalAction& getOverlayOpacity() { return _overlayOpacity; }
    OptionAction& getTypeAction() { return _typeAction; }
    ToggleAction& getRectangleAction() { return _rectangleAction; }
    ToggleAction& getBrushAction() { return _brushAction; }
    ToggleAction& getLassoAction() { return _lassoAction; }
    ToggleAction& getPolygonAction() { return _polygonAction; }
    ToggleAction& getSampleAction() { return _sampleAction; }
    ToggleAction& getRoiAction() { return _roiAction; }
    ToggleAction& getModifierReplaceAction() { return _modifierReplaceAction; }
    ToggleAction& getModifierAddAction() { return _modifierAddAction; }
    ToggleAction& getModifierSubtractAction() { return _modifierSubtractAction; }
    TriggerAction& getClearSelectionAction() { return _clearSelectionAction; }
    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getInvertSelectionAction() { return _invertSelectionAction; }
    DecimalAction& getBrushRadiusAction() { return _brushRadiusAction; }
    ToggleAction& getNotifyDuringSelectionAction() { return _notifyDuringSelectionAction; }

protected:
    QWidget*                        _targetWidget;                      /** Pointer to target widget */
    util::PixelSelectionTool&       _pixelSelectionTool;                /** Reference to pixel selection tool */
    util::PixelSelectionTypes       _pixelSelectionTypes;               /** Pixel selection types */
    ColorAction                     _overlayColor;                      /** Selection overlay color action */
    DecimalAction                   _overlayOpacity;                    /** Selection overlay opacity action */
    util::PixelSelectionTypeModel   _typeModel;                         /** Selection type model */
    OptionAction                    _typeAction;                        /** Selection type action */
    ToggleAction                    _rectangleAction;                   /** Switch to rectangle selection action */
    ToggleAction                    _brushAction;                       /** Switch to brush selection action */
    ToggleAction                    _lassoAction;                       /** Switch to lasso selection action */
    ToggleAction                    _polygonAction;                     /** Switch to polygon selection action */
    ToggleAction                    _sampleAction;                      /** Switch to sample selection action */
    ToggleAction                    _roiAction;                         /** Switch to ROI selection action */
    QActionGroup                    _typeActionGroup;                   /** Type action group */
    ToggleAction                    _modifierReplaceAction;             /** Replace current selection action */
    ToggleAction                    _modifierAddAction;                 /** Add to current selection action */
    ToggleAction                    _modifierSubtractAction;            /** Subtract from current selection action */
    QActionGroup                    _modifierActionGroup;               /** Modifier action group */
    TriggerAction                   _clearSelectionAction;              /** Clear selection action */
    TriggerAction                   _selectAllAction;                   /** Select all action */
    TriggerAction                   _invertSelectionAction;             /** Invert selection action */
    DecimalAction                   _brushRadiusAction;                 /** Brush radius action */
    ToggleAction                    _notifyDuringSelectionAction;       /** Notify during selection action */
};

}
}
