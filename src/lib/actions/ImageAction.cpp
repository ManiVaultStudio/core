// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ImageAction.h"

#include <QHBoxLayout>
#include <QBuffer>

namespace mv::gui {

ImageAction::ImageAction(QObject* parent, const QString& title, bool populateFilePickerCompleter /*= true*/) :
    WidgetAction(parent, title),
    _filePathAction(this, "File Path"),
    _fileNameAction(this, "File Name"),
    _filePickerAction(parent, "File Picker", QString(), populateFilePickerCompleter),
    _previewAction(this, "Preview")
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Preview);

    _filePathAction.setEnabled(false);
    _filePathAction.setTextElideMode(Qt::ElideMiddle);
    _filePathAction.setStretch(1);

    _fileNameAction.setEnabled(false);
    _fileNameAction.setTextElideMode(Qt::ElideMiddle);
    _fileNameAction.setStretch(1);

    _filePickerAction.setPlaceHolderString("Pick image...");
    _filePickerAction.setFileType("Image");
    _filePickerAction.setNameFilters({ "Images (*.png *.bmp *.jpg)" });
    _filePickerAction.setDefaultWidgetFlags(FilePickerAction::PushButton);
        
    _filePickerAction.getPickAction().setDefaultWidgetFlags(TriggerAction::Icon);

    _previewAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _previewAction.setIconByName("eye");

    connect(&_filePickerAction, &FilePickerAction::filePathChanged, this, [this](const QString& filePath) -> void {
        loadImage(filePath);
    });

    connect(&_filePathAction, &StringAction::stringChanged, this, [this](const QString& string) -> void {
        _fileNameAction.setString(QFileInfo(string).fileName());
        _fileNameAction.setToolTip(string);
    });
}

const QImage ImageAction::getImage() const
{
    return _image;
}

void ImageAction::setImage(const QImage& image)
{
    _image = image;

    emit imageChanged(_image);
}

void ImageAction::loadImage(const QString& filePath)
{
    if (!QFileInfo(filePath).exists())
        return;

    setImage(QImage(filePath));

    _filePathAction.setString(filePath);
}

ImageAction::PreviewWidget::PreviewWidget(QWidget* parent, ImageAction& imageAction) :
    QLabel("No image loaded...", parent),
    _imageAction(imageAction)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAlignment(Qt::AlignCenter);
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    setScaledContents(true);

    const auto update = [this]() {

        const auto image = _imageAction.getImage();

        if (image.isNull())
            return;

        setPixmap(QPixmap::fromImage(_imageAction.getImage()));

        const auto aspectRatio  = static_cast<float>(image.height()) / static_cast<float>(image.width());
        const auto fixedSize    = QSize(width(), width() * aspectRatio);

        setFixedSize(fixedSize);

        adjustSize();
        updateGeometry();
    };

    connect(&imageAction, &ImageAction::imageChanged, this, update);

    update();
}

ImageAction::LoaderWidget::LoaderWidget(QWidget* parent, ImageAction& imageAction) :
    QWidget(parent),
    _imageAction(imageAction),
    _groupAction(this, "Group")
{
    _groupAction.setShowLabels(false);

    _groupAction.addAction(&_imageAction.getFileNameAction());
    _groupAction.addAction(&_imageAction.getFilePickerAction());

    auto layout = new QHBoxLayout();
    
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(_groupAction.createWidget(this));

    setLayout(layout);
}

QWidget* ImageAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::Preview)
        layout->addWidget(new ImageAction::PreviewWidget(parent, *this));

    if (widgetFlags & WidgetFlag::Loader)
        layout->addWidget(new ImageAction::LoaderWidget(parent, *this));

    widget->setLayout(layout);

    return widget;
}

void ImageAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
    
    QImage image;

    image.loadFromData(QByteArray::fromBase64(variantMap["Value"].toByteArray()));

    setImage(image);

    _filePathAction.fromParentVariantMap(variantMap);
    _fileNameAction.fromParentVariantMap(variantMap);
}

QVariantMap ImageAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    QByteArray previewImageByteArray;
    QBuffer previewImageBuffer(&previewImageByteArray);

    _image.save(&previewImageBuffer, "PNG");

    variantMap.insert({
        { "Value", QVariant::fromValue(previewImageByteArray.toBase64()) }
    });

    _filePathAction.insertIntoVariantMap(variantMap);
    _fileNameAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
