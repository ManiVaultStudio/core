// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "WidgetActionWidget.h"

#include <QIcon>

namespace mv::gui {

/**
 * Icon action class (WIP)
 *
 * For icon storage
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT IconAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Label widget class for icon action */
    class CORE_EXPORT Widget : public WidgetActionWidget {
    protected:

        /**
         * Construct with pointer to \p parent widget, pointer to \p iconAction and \p widgetFlags
         * @param parent Pointer to parent widget
         * @param iconAction Pointer to icon action
         * @param widgetFlags Widget flags
         */
        Widget(QWidget* parent, IconAction* iconAction, const std::int32_t& widgetFlags);

    protected:
        IconAction*     _iconAction;    /** Pointer to icon action */
        QLabel          _iconLabel;     /** Label which will display the icon */

        friend class IconAction;
    };

protected:

    /**
     * Get widget representation of the icon action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override
    {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE IconAction(QObject* parent, const QString& title);

    /**
     * Get the current icon
     * @return Icon
     */
    QIcon getIcon() const;

    /**
     * Set the current icon to \p icon
     * @param icon Current icon
     */
    void setIcon(const QIcon& icon);

    /**
     * Set the current icon from \p image
     * @param image Image to convert to icon
     */
    void setIconFromImage(const QImage& image);

public: // Serialization

    /**
     * Load image action from variant
     * @param Variant representation of the image action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save image action to variant
     * @return Variant representation of the image action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    //StringAction& getFilePathAction() { return _filePathAction; }
    //StringAction& getFileNameAction() { return _fileNameAction; }
    //FilePickerAction& getFilePickerAction() { return _filePickerAction; }
    //TriggerAction& getPreviewAction() { return _previewAction; }

signals:

    /**
     * Signals that the current icon changed
     * @param icon Current icon that changed
     */
    void iconChanged(const QIcon& icon);

protected:
    QIcon              _icon;             /** Current icon */
    //StringAction        _filePathAction;    /** String action which contains the last loaded file path */
    //StringAction        _fileNameAction;    /** String action which contains the last loaded file name */
    //FilePickerAction    _filePickerAction;  /** Action for loading the image from disk */
    //TriggerAction       _previewAction;     /** Action for previewing the image (via tooltip) */
};

}

Q_DECLARE_METATYPE(mv::gui::IconAction)

inline const auto iconActionMetaTypeId = qRegisterMetaType<mv::gui::IconAction*>("mv::gui::IconAction");
