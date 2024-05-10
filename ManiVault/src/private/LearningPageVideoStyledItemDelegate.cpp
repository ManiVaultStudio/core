// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideoStyledItemDelegate.h"

#include <widgets/YouTubeVideoDialog.h>

#include <QDebug>
#include <QImage>

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
    return option.rect.size();
}

QWidget* LearningPageVideoStyledItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    return new EditorWidget(parent);
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

LearningPageVideoStyledItemDelegate::EditorWidget::EditorWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _textLayout(),
    _thumbnailLabel(),
    _titleLabel(),
    _summaryLabel(),
    _tagsLayout()
{
    _textLayout.addWidget(&_titleLabel);
    _textLayout.addWidget(&_summaryLabel);
    _textLayout.addLayout(&_tagsLayout);
    _textLayout.addStretch(1);

    _mainLayout.addWidget(&_thumbnailLabel);
    _mainLayout.addLayout(&_textLayout);

    setLayout(&_mainLayout);

    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        _thumbnailLabel.setPixmap(QPixmap::fromImage(QImage::fromData(_fileDownloader.downloadedData())));
    });
}

void LearningPageVideoStyledItemDelegate::EditorWidget::setEditorData(const QModelIndex& index)
{
    _index = index;

    const auto youTubeId            = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();
    const auto youtTubeThumbnailUrl = getYouTubeThumbnailUrl(youTubeId);

    _fileDownloader.download(QUrl(youtTubeThumbnailUrl));

    _titleLabel.setText(_index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Title)).data().toString());
    _summaryLabel.setText(_index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Summary)).data().toString());

    const auto tags = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Tags)).data(Qt::EditRole).toStringList();

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
            font-size: 7pt; \
            font-weight: 600; \
        }");

        _tagsLayout.addWidget(label);
    }
}

void LearningPageVideoStyledItemDelegate::EditorWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    const auto youTubeId = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();

    YouTubeVideoDialog::play(youTubeId);
}
