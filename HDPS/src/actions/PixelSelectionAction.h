#pragma once

#include "util/PixelSelectionTool.h"

#include "GroupAction.h"
#include "ColorAction.h"
#include "DecimalAction.h"
#include "ToggleAction.h"

#include "PixelSelectionTypeAction.h"
#include "PixelSelectionModifierAction.h"
#include "PixelSelectionOperationsAction.h"

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
     */
    PixelSelectionAction(QWidget* targetWidget, util::PixelSelectionTool& pixelSelectionTool);

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
    PixelSelectionTypeAction& getPixelSelectionTypeAction() { return _pixelSelectionTypeAction; }
    PixelSelectionModifierAction& getPixelSelectionModifierAction() { return _pixelSelectionModifierAction; }
    PixelSelectionOperationsAction& getPixelSelectionOperationsAction() { return _pixelSelectionOperationsAction; }
    DecimalAction& getBrushRadiusAction() { return _brushRadiusAction; }
    ToggleAction& getNotifyDuringSelectionAction() { return _notifyDuringSelectionAction; }

protected:
    QWidget*                            _targetWidget;                      /** Pointer to target widget */
    ColorAction                         _overlayColor;                      /** Selection overlay color action */
    DecimalAction                       _overlayOpacity;                    /** Selection overlay opacity action */
    util::PixelSelectionTool&           _pixelSelectionTool;                /** Reference to pixel selection tool */
    PixelSelectionTypeAction            _pixelSelectionTypeAction;          /** Pixel selection type action */
    PixelSelectionModifierAction        _pixelSelectionModifierAction;      /** Pixel selection modifier action */
    PixelSelectionOperationsAction      _pixelSelectionOperationsAction;    /** Pixel selection operations action */
    DecimalAction                       _brushRadiusAction;                 /** Brush radius action */
    ToggleAction                        _notifyDuringSelectionAction;       /** Notify during selection action */
};

}
}
