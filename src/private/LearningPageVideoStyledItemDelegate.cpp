// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideoStyledItemDelegate.h"
#include "LearningPageVideoWidget.h"

#include <widgets/YouTubeVideoDialog.h>

#include <QAbstractTextDocumentLayout>

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
    auto editorWidget = std::make_unique<LearningPageVideoWidget>(index);

    editorWidget->adjustSize();

    return editorWidget->sizeHint();
}

QWidget* LearningPageVideoStyledItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    return new LearningPageVideoWidget(index, parent);
}

void LearningPageVideoStyledItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}
