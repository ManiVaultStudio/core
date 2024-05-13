// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideoStyledItemDelegate.h"

#include <widgets/YouTubeVideoDialog.h>

#include <QDebug>
#include <QImage>
#include <QAbstractTextDocumentLayout>

using namespace mv::util;
using namespace mv::gui;

/**
 * Get youTube thumbnail for \p videoId with \p quality
 * @param videoId Globally unique identifier of the video
 * @param quality String determining the quality: "default", "hqdefault", "mqdefault", "sddefault", "maxresdefault"
 */
QString getYouTubeThumbnailUrl(const QString& videoId, const QString& quality = "default") {
    return QString("https://img.youtube.com/vi/%1/%2.jpg").arg(videoId, quality);
}

LearningPageVideoStyledItemDelegate::LearningPageVideoStyledItemDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent)
{
}

QSize LearningPageVideoStyledItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto rectangle = option.rect.size();

    auto editorWidget = std::make_unique<EditorWidget>(const_cast<LearningPageVideoStyledItemDelegate*>(this));

    editorWidget->setEditorData(index);
    editorWidget->adjustSize();

    return editorWidget->sizeHint();
}

QWidget* LearningPageVideoStyledItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    return new EditorWidget(const_cast<LearningPageVideoStyledItemDelegate*>(this), parent);
}

void LearningPageVideoStyledItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    static_cast<EditorWidget*>(editor)->setEditorData(index);
}

void LearningPageVideoStyledItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}

LearningPageVideoStyledItemDelegate::EditorWidget::EditorWidget(LearningPageVideoStyledItemDelegate* delegate, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _delegate(delegate),
    _mainLayout(),
    _thumbnailLayout(),
    _textLayout(),
    _thumbnailLabel(),
    _propertiesTextBrowser(),
    _tagsLayout(),
    _thumbnailPixmap()
{
    //_mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setAlignment(Qt::AlignTop);

    _thumbnailLayout.addWidget(&_thumbnailLabel);
    _thumbnailLayout.setAlignment(Qt::AlignTop);

    _textLayout.setContentsMargins(0, 0, 0, 0);
    _textLayout.setAlignment(Qt::AlignTop);
    _textLayout.addWidget(&_propertiesTextBrowser);
    _textLayout.addStretch(1);
    //_textLayout.addLayout(&_tagsLayout);

    _thumbnailLabel.setStyleSheet("border: 2px solid red;");

    _propertiesTextBrowser.setReadOnly(true);
    _propertiesTextBrowser.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.setStyleSheet("background-color: yellow; border: none; margin: 0px; padding: 0px;");//transparent
    _propertiesTextBrowser.setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    //_propertiesTextBrowser.document()->setDefaultStyleSheet("div { margin: 0px; }");
    _propertiesTextBrowser.document()->setDocumentMargin(0);

    connect(_propertiesTextBrowser.document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, [this]() -> void {
        _propertiesTextBrowser.setFixedHeight(_propertiesTextBrowser.document()->size().height());

        emit _delegate->sizeHintChanged(_index);
    });

    _mainLayout.addLayout(&_thumbnailLayout);
    _mainLayout.addLayout(&_textLayout);

    setLayout(&_mainLayout);

    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        _thumbnailPixmap = QPixmap::fromImage(QImage::fromData(_fileDownloader.downloadedData()));

        const auto marginToRemove   = 10;
        const auto rectangleToCopy  = QRect(0, marginToRemove,_thumbnailPixmap.width(), _thumbnailPixmap.height() - (2 * marginToRemove));

        _thumbnailPixmap = _thumbnailPixmap.copy(rectangleToCopy);

        _thumbnailLabel.setFixedSize(_thumbnailPixmap.size());
        _thumbnailLabel.setPixmap(_thumbnailPixmap.copy());
    });
}

void LearningPageVideoStyledItemDelegate::EditorWidget::setEditorData(const QModelIndex& index)
{
    _index = index;

    if (_thumbnailPixmap.isNull()) {
        const auto youTubeId            = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();
        const auto youtTubeThumbnailUrl = getYouTubeThumbnailUrl(youTubeId);

        _fileDownloader.download(QUrl(youtTubeThumbnailUrl));
    }

    const auto title    = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Title)).data().toString();
    const auto summary  = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Summary)).data().toString();

    _propertiesTextBrowser.setHtml(QString(" \
        <div> \
            <p style='margin: 0px; padding: 0px; background-color: red;'><b>%1</b></p> \
            <p style='margin: 0px; padding: 10px; background-color: blue;'>%2</p> \
        </div> \
    ").arg(title, summary));

    /*const auto tags = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Tags)).data(Qt::EditRole).toStringList();

    for (const auto& tag : tags) {
        auto label = new QLabel(tag);

        label->setObjectName("Tag");
        label->setStyleSheet("QLabel#Tag { \
            background-color: rgb(180, 180, 180); \
            border-radius: 5px; \
            padding-left: 4px; \
            padding-right: 4px; \
            padding-top: 1px; \
            padding-bottom: 1px; \
            font-size: 8pt; \
            font-weight: 600; \
        }");

        _tagsLayout.addWidget(label);
    }*/
}

void LearningPageVideoStyledItemDelegate::EditorWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    const auto youTubeId = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();

    YouTubeVideoDialog::play(youTubeId);
}
