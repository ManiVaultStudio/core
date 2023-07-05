// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "ColorPickerAction.h"

#include <QHBoxLayout>
#include <QToolButton>

namespace hdps::gui {

/**
 * Color widget action class
 *
 * Stores a color and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class ColorAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        Picker          = 0x00001,      /** The widget includes a color picker */
        ResetPushButton = 0x00002,      /** The widget includes a reset push button */

        Basic   = Picker,
        All     = Picker | ResetPushButton
    };

public:

    /** Color picker push button class for color action */
    class PushButtonWidget : public WidgetActionWidget
    {
    protected:

        /** Extended tool button widget class with display of current color */
        class ToolButton : public QToolButton
        {
        public:

            /**
             * Constructor
             * @param parent Parent widget
             * @param colorPickerAction Reference to color picker action
             */
            ToolButton(QWidget* parent, ColorPickerAction& colorPickerAction);

            /**
             * Paint event to override default paint
             * @param paintEvent Pointer to paint event
             */
            void paintEvent(QPaintEvent* paintEvent) override;

        protected:
            ColorPickerAction&  _colorPickerAction;     /** Reference to color picker action */
        };

    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorAction Pointer to color action
         */
        PushButtonWidget(QWidget* parent, ColorAction* colorAction);

    public: // Action getters

        ColorPickerAction& getColorPickerAction() { return _colorPickerAction; }

    protected:
        QHBoxLayout         _layout;                /** Widget layout */
        ColorPickerAction   _colorPickerAction;     /** Color picker action */
        ToolButton          _toolButton;            /** Tool button for the popup */

        friend class ColorAction;
    };

protected:

    /**
     * Get widget representation of the color action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param color Initial color
     */
    Q_INVOKABLE ColorAction(QObject* parent, const QString& title, const QColor& color = DEFAULT_COLOR);

    /** Gets the current color */
    QColor getColor() const;

    /**
     * Sets the current color
     * @param color Current color
     */
    void setColor(const QColor& color);

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
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the current color changed
     * @param color Current color that changed
     */
    void colorChanged(const QColor& color);

protected:
    QColor  _color;     /** Current color */

    /** Default default color */
    static const QColor DEFAULT_COLOR;

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(hdps::gui::ColorAction)

inline const auto colorActionMetaTypeId = qRegisterMetaType<hdps::gui::ColorAction*>("hdps::gui::ColorAction");