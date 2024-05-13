// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideoStyledItemDelegate.h"

#include <widgets/YouTubeVideoDialog.h>

#include <QDebug>
#include <QImage>
#include <QAbstractTextDocumentLayout>
#include <QLocale>

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
    _thumbnailPixmap(),
    _overlayWidget(this)
{
    //_mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setSpacing(10);
    _mainLayout.setAlignment(Qt::AlignTop);

    _thumbnailLayout.addWidget(&_thumbnailLabel);
    _thumbnailLayout.setAlignment(Qt::AlignTop);

    _textLayout.setContentsMargins(0, 0, 0, 0);
    _textLayout.setAlignment(Qt::AlignTop);
    _textLayout.addWidget(&_propertiesTextBrowser);

    _thumbnailLabel.setStyleSheet("");

    _propertiesTextBrowser.setReadOnly(true);
    _propertiesTextBrowser.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.setStyleSheet("background-color: transparent; border: none; margin: 0px; padding: 0px;");
    _propertiesTextBrowser.setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
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

        const auto marginToRemove   = 9;
        const auto rectangleToCopy  = QRect(0, marginToRemove,_thumbnailPixmap.width(), _thumbnailPixmap.height() - (2 * marginToRemove));

        _thumbnailPixmap = _thumbnailPixmap.copy(rectangleToCopy);

        _thumbnailLabel.setStyleSheet("margin-top: 3px; border: 1px solid rgb(150, 150, 150);");
        _thumbnailLabel.setFixedSize(_thumbnailPixmap.size());
        _thumbnailLabel.setPixmap(_thumbnailPixmap.copy());
    });

    _overlayWidget.hide();
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
    const auto summary  = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Summary)).data().toString().replace("\n", "").replace("<p>", "").replace("</p>", "");

    _propertiesTextBrowser.setHtml(QString(" \
        <div style='margin: 0px;'> \
            <p style='margin-top: -3px;'><b>%1</b></p> \
            <p style='margin-top: 1px;'>%2</p> \
        </div> \
    ").arg(title, summary));

    _overlayWidget.setIndex(index);
}

void LearningPageVideoStyledItemDelegate::EditorWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    const auto youTubeId = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();

    YouTubeVideoDialog::play(youTubeId);
}

void LearningPageVideoStyledItemDelegate::EditorWidget::enterEvent(QEnterEvent* enterEvent)
{
    QWidget::enterEvent(enterEvent);

    _overlayWidget.show();
}

void LearningPageVideoStyledItemDelegate::EditorWidget::leaveEvent(QEvent* leaveEvent)
{
    QWidget::leaveEvent(leaveEvent);

    _overlayWidget.hide();
}

LearningPageVideoStyledItemDelegate::VideoOverlayWidget::VideoOverlayWidget(QWidget* parent) :
    QWidget(parent),
    _mainLayout(),
    _dateLabel(),
    _tagsLabel(),
    _widgetOverlayer(this, this, parent)
{
    setObjectName("VideoOverlayWidget");

    _mainLayout.addWidget(&_dateLabel);
    _mainLayout.addStretch(1);
    _mainLayout.addWidget(&_tagsLabel);

    setLayout(&_mainLayout);

    const auto backgroundColor = QApplication::palette().color(QPalette::Normal, QPalette::Midlight);

    setStyleSheet(QString("QWidget#VideoOverlayWidget { \
        background: qlineargradient( x1:0 y1:0, x2:1 y2:0, stop: 0.2 rgba(%1, %2, %3, 0), stop: 1.0 rgba(%1, %2, %3, 255)); \
    }").arg(QString::number(backgroundColor.red()), QString::number(backgroundColor.green()), QString::number(backgroundColor.blue())));
}

void LearningPageVideoStyledItemDelegate::VideoOverlayWidget::setIndex(const QModelIndex& index)
{
    if (index == _index)
        return;

    if (!index.isValid())
        return;

    _index = index;

    const auto date = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Date)).data(Qt::EditRole).toString();

    QLocale locale;

    const auto dateTime = QDateTime::fromString(date, Qt::ISODate);

    _dateLabel.setText(QString("<div style='text-align: right; color: rgb(60, 60, 60);'>%1</div>").arg(locale.toString(dateTime.date())));

    const auto tags = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Tags)).data(Qt::EditRole).toStringList();

    QString tagsHtml;

    tagsHtml += "<div style='text-align: right; color: rgb(60, 60, 60)'>";
    tagsHtml += tags.join(", ");
    tagsHtml += "</div>";

    _tagsLabel.setText(tagsHtml);

    _mainLayout.addWidget(&_tagsLabel);
}
