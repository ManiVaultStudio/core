// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "FilePickerAction.h"
#include "HorizontalGroupAction.h"
#include "TriggerAction.h"
#include "WidgetAction.h"

#include <QImage>
#include <QLabel>

namespace mv::gui {

/**
 * Image widget action class (WIP)
 *
 * For image display and storage
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ImageAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Preview = 0x00001,   /** Image preview with a label widget */
        Loader  = 0x00002,   /** Widget for loading an image into the action */
    };

public:

    /** Preview widget class for image action */
    class CORE_EXPORT PreviewWidget : public QLabel
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param imageAction Reference to image action
         */
        PreviewWidget(QWidget* parent, ImageAction& imageAction);

        QSize sizeHint() const override {
            return QSize(100, 100);
        }

    protected:
        ImageAction& _imageAction;       /** Reference to owning image action */

        friend class ImageAction;
    };

    /** Loader widget class for image action */
    class CORE_EXPORT LoaderWidget : public QWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param imageAction Reference to image action
         */
        LoaderWidget(QWidget* parent, ImageAction& imageAction);

    protected:
        ImageAction&            _imageAction;       /** Pointer to owning image action */
        HorizontalGroupAction   _groupAction;       /** Group action for the preview and picker action */

        friend class ImageAction;
    };

protected:

    /**
     * Get widget representation of the image action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param populateFilePickerCompleter Whether to create a QCompleter for _filePathAction in _filePickerAction
     */
    Q_INVOKABLE ImageAction(QObject* parent, const QString& title, bool populateFilePickerCompleter = true);

    /** Get the current image */
    const QImage getImage() const;

    /**
     * Set the current image
     * @param image Current image
     */
    void setImage(const QImage& image);

    /**
     * Load image from disk
     * @param filePath Path of the image on disk
     */
    void loadImage(const QString& filePath);

public: // Serialization

    /**
     * Load image action from variant
     * @param variantMap Variant representation of the image action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save image action to variant
     * @return Variant representation of the image action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    StringAction& getFilePathAction() { return _filePathAction; }
    StringAction& getFileNameAction() { return _fileNameAction; }
    FilePickerAction& getFilePickerAction() { return _filePickerAction; }
    TriggerAction& getPreviewAction() { return _previewAction; }

signals:

    /**
     * Signals that the current image changed
     * @param image Current image that changed
     */
    void imageChanged(const QImage& image);

protected:
    QImage              _image;             /** Current image */
    StringAction        _filePathAction;    /** String action which contains the last loaded file path */
    StringAction        _fileNameAction;    /** String action which contains the last loaded file name */
    FilePickerAction    _filePickerAction;  /** Action for loading the image from disk */
    TriggerAction       _previewAction;     /** Action for previewing the image (via tooltip) */
};

}

Q_DECLARE_METATYPE(mv::gui::ImageAction)

inline const auto imageActionMetaTypeId = qRegisterMetaType<mv::gui::ImageAction*>("mv::gui::ImageAction");
