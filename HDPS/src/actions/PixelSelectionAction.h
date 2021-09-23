#pragma once

#include "actions/Actions.h"
#include "util/PixelSelectionTool.h"

#include "PixelSelectionTypeAction.h"
#include "PixelSelectionOverlayAction.h"
#include "PixelSelectionModifierAction.h"
#include "PixelSelectionOperationsAction.h"

class QWidget;

using namespace hdps::gui;

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
    PixelSelectionAction(QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu();

    /** Get pointer to the target on which the pixel selection tool operates */
    QWidget* getTargetWidget();

    /** Get reference to pixel selection tool */
    PixelSelectionTool& getPixelSelectionTool();

public: // Event handling

    /**
     * Listens to the events of target \p object
     * @param object Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* object, QEvent* event) override;

public: /** Action getters */

protected:
    QWidget*                            _targetWidget;                      /** Pointer to target widget */
    PixelSelectionTool&                 _pixelSelectionTool;                /** Reference to pixel selection tool */
    PixelSelectionTypeAction            _pixelSelectionTypeAction;          /** Pixel selection type action */
    PixelSelectionOverlayAction         _pixelSelectionOverlayAction;       /** Pixel selection overlay action */
    PixelSelectionModifierAction        _pixelSelectionModifierAction;      /** Pixel selection modifier action */
    PixelSelectionOperationsAction      _pixelSelectionOperationsAction;    /** Pixel selection operations action */
    DecimalAction                       _brushRadiusAction;                 /** Brush radius action */
    ToggleAction                        _notifyDuringSelectionAction;       /** Notify during selection action */
};
