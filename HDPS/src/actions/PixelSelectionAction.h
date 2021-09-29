#pragma once

#include "util/PixelSelectionTool.h"

#include "GroupAction.h"
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
class PixelSelectionAction : public GroupAction
{
public:

    /**
     * Constructor
     * @param targetWidget Pointer to target widget
     * @param pixelSelectionTool Reference to pixel selection tool
     * @param pixelSelectionTypes Allowed pixel selection types
     */
    PixelSelectionAction(QWidget* targetWidget, util::PixelSelectionTool& pixelSelectionTool, const util::PixelSelectionTypes& pixelSelectionTypes = util::defaultPixelSelectionTypes);

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

    /** Get allowed pixel selection types */
    util::PixelSelectionTypes getAllowedTypes() const;

    /** Set allowed pixel selection types */
    void setAllowedTypes(const util::PixelSelectionTypes& pixelSelectionTypes);

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
    TriggerAction& getRectangleAction() { return _rectangleAction; }
    TriggerAction& getBrushAction() { return _brushAction; }
    TriggerAction& getLassoAction() { return _lassoAction; }
    TriggerAction& getPolygonAction() { return _polygonAction; }
    TriggerAction& getSampleAction() { return _sampleAction; }
    ToggleAction& getModifierReplaceAction() { return _modifierReplaceAction; }
    ToggleAction& getModifierAddAction() { return _modifierAddAction; }
    ToggleAction& getModifierSubtractAction() { return _modifierSubtractAction; }
    TriggerAction& getClearSelectionAction() { return _clearSelectionAction; }
    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getInvertSelectionAction() { return _invertSelectionAction; }
    DecimalAction& getBrushRadiusAction() { return _brushRadiusAction; }
    ToggleAction& getNotifyDuringSelectionAction() { return _notifyDuringSelectionAction; }

protected:
    QWidget*                    _targetWidget;                      /** Pointer to target widget */
    util::PixelSelectionTool&   _pixelSelectionTool;                /** Reference to pixel selection tool */
    util::PixelSelectionTypes   _pixelSelectionTypes;               /** Pixel selection types */
    ColorAction                 _overlayColor;                      /** Selection overlay color action */
    DecimalAction               _overlayOpacity;                    /** Selection overlay opacity action */
    OptionAction                _typeAction;                        /** Selection type action */
    TriggerAction               _rectangleAction;                   /** Switch to rectangle selection action */
    TriggerAction               _brushAction;                       /** Switch to brush selection action */
    TriggerAction               _lassoAction;                       /** Switch to lasso selection action */
    TriggerAction               _polygonAction;                     /** Switch to polygon selection action */
    TriggerAction               _sampleAction;                      /** Switch to sample selection action */
    QActionGroup                _typeActionGroup;                   /** Type action group */
    ToggleAction                _modifierReplaceAction;             /** Replace current selection action */
    ToggleAction                _modifierAddAction;                 /** Add to current selection action */
    ToggleAction                _modifierSubtractAction;            /** Subtract from current selection action */
    QActionGroup                _modifierActionGroup;               /** Modifier action group */
    TriggerAction               _clearSelectionAction;              /** Clear selection action */
    TriggerAction               _selectAllAction;                   /** Select all action */
    TriggerAction               _invertSelectionAction;             /** Invert selection action */
    DecimalAction               _brushRadiusAction;                 /** Brush radius action */
    ToggleAction                _notifyDuringSelectionAction;       /** Notify during selection action */
};

}
}
