// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"
#include "actions/ToggleAction.h"
#include "actions/DecimalAction.h"
#include "actions/ColorAction.h"

namespace hdps::gui {

class ColorMapEditor1DNode;
class ColorMapEditor1DAction;

/**
 * Color map editor 1D node action class
 *
 * Action class for editing one-dimensional color map editor node parameters
 *
 * @author Mitchell M. de Boer and Thomas Kroes
 */
class ColorMapEditor1DNodeAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for one-dimensional color map editor node action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapEditor1DNodeAction Pointer to one-dimensional color map editor node action
         */
        Widget(QWidget* parent, ColorMapEditor1DNodeAction* colorMapEditor1DNodeAction);
    };

    /**
     * Get widget representation of the color map editor action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * title Title of the action
     */
    Q_INVOKABLE ColorMapEditor1DNodeAction(QObject* parent, const QString& title);

    /**
     * Connect to a node
     * @param node Pointer to node
     */
    void connectToNode(ColorMapEditor1DNode* node);

    /**
     * Disconnect from a node
     * @param node Pointer to node
     */
    void disconnectFromNode(ColorMapEditor1DNode* node);

    /** Invoked when the node changes one of its parameters and updates the user interface accordingly */
    void nodeChanged();

public: // Action getters

    ColorAction& getColorAction() { return _colorAction; }
    DecimalAction& getValueAction() { return _valueAction; }
    DecimalAction& getOpacityAction() { return _opacityAction; }

protected:
    ColorMapEditor1DAction&     _colorMapEditor1DAction;    /** Reference to owning one-dimensional color map action */
    ColorMapEditor1DNode*       _currentNode;               /** Pointer to current node (if any) */
    ColorAction                 _colorAction;               /** Node color action */
    DecimalAction               _valueAction;               /** Value action */
    DecimalAction               _opacityAction;             /** Opacity action */
};

}

Q_DECLARE_METATYPE(hdps::gui::ColorMapEditor1DNodeAction)

inline const auto colorMapEditor1DNodeActionMetaTypeId = qRegisterMetaType<hdps::gui::ColorMapEditor1DNodeAction*>("hdps::gui::ColorMapEditor1DNodeAction");