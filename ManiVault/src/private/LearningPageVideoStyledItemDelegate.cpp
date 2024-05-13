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

    return editorWidget->sizeHint();// dynamic_cast<QWidget*>(parent())->sizeHint().width());
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
    _textLayout(),
    _thumbnailLabel(),
    _propertiesLabel(),
    _tagsLayout(),
    _thumbnailPixmap()
{
    _mainLayout.setContentsMargins(0, 0, 0, 0);

    _textLayout.addWidget(&_propertiesLabel);
    _textLayout.addLayout(&_tagsLayout);
    //_textLayout.addStretch(1);

    _thumbnailLabel.setStyleSheet("border: 1px solid red;");

    _propertiesLabel.setReadOnly(true);
    _propertiesLabel.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesLabel.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesLabel.setStyleSheet("background-color: yellow; border: none; margin: 0; padding: 0;");
    _propertiesLabel.setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    

    //_mainLayout.addWidget(&_thumbnailLabel);
    _mainLayout.addWidget(&_propertiesLabel);
    //_mainLayout.addLayout(&_textLayout);

    setLayout(&_mainLayout);

    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        _thumbnailPixmap = QPixmap::fromImage(QImage::fromData(_fileDownloader.downloadedData()));

        _thumbnailLabel.setFixedSize(_thumbnailPixmap.size());
        _thumbnailLabel.setPixmap(_thumbnailPixmap);
        _thumbnailLabel.setAlignment(Qt::AlignTop);
    });
}

void LearningPageVideoStyledItemDelegate::EditorWidget::setEditorData(const QModelIndex& index)
{
    _index = index;

    if (_thumbnailPixmap.isNull()) {
        const auto youTubeId = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();
        const auto youtTubeThumbnailUrl = getYouTubeThumbnailUrl(youTubeId);

        _fileDownloader.download(QUrl(youtTubeThumbnailUrl));
    }

    const auto title    = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Title)).data().toString();
    const auto summary  = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Summary)).data().toString();

    connect(_propertiesLabel.document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, [this]() -> void {
        qDebug() << "QAbstractTextDocumentLayout::documentSizeChanged()";
        _propertiesLabel.setFixedHeight(_propertiesLabel.document()->size().height() + 100);

        emit _delegate->sizeHintChanged(_index);

        if (parent()->parent())
            dynamic_cast<QAbstractItemView*>(parent()->parent())->update();
        
        updateGeometry();

        disconnect(_propertiesLabel.document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, nullptr);
    }, Qt::DirectConnection);

    qDebug() << "----B----";

    _propertiesLabel.setHtml(QString(" \
        <p><b>%1</b></p> \
        <p>%2</p> \
    ").arg(title, summary));

    

    //const auto tags = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Tags)).data(Qt::EditRole).toStringList();

    //for (const auto& tag : tags) {
    //    auto label = new QLabel(tag);

    //    label->setObjectName("Tag");
    //    label->setStyleSheet("QLabel#Tag { \
    //        background-color: rgb(180, 180, 180); \
    //        border-radius: 5px; \
    //        padding-left: 4px; \
    //        padding-right: 4px; \
    //        padding-top: 1px; \
    //        padding-bottom: 1px; \
    //        font-size: 7pt; \
    //        font-weight: 600; \
    //    }");

    //    _tagsLayout.addWidget(label);
    //}

    update();

    

    
}

void LearningPageVideoStyledItemDelegate::EditorWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    const auto youTubeId = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();

    YouTubeVideoDialog::play(youTubeId);
}
