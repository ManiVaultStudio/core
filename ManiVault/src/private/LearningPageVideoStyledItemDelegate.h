// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStyledItemDelegate>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextBrowser>

#include "LearningPageVideosModel.h"

#include "util/FileDownloader.h"

#include "widgets/FlowLayout.h"

/**
 * Learning page video styled item delegate class
 *
 * Delegate class for showing video information
 *
 * @author Thomas Kroes
 */
class LearningPageVideoStyledItemDelegate : public QStyledItemDelegate
{
private:

    class VideoOverlayWidget : public QWidget {
    public:
        explicit VideoOverlayWidget(QWidget* parent);

        void setIndex(const QModelIndex& index);

    private:
        QPersistentModelIndex       _index;
        QVBoxLayout                 _mainLayout;                        /** Main vertical layout */
        QLabel                      _dateLabel;
        QLabel                      _tagsLabel;
        mv::util::WidgetOverlayer   _widgetOverlayer;       /** Synchronizes the size with the source widget */

    };
    class EditorWidget : public QWidget
    {
    public:

        /**
         * Construct with pointer to \p parent widget
         * @param delegate Pointer to delegate item
         * @param parent Pointer to parent widget
         */
        EditorWidget(LearningPageVideoStyledItemDelegate* delegate, QWidget* parent = nullptr);

        /**
         * Set editor data from model \p index
         * @param index Model index to fetch data from
         */
        void setEditorData(const QModelIndex& index);

        void mousePressEvent(QMouseEvent* event);

        /**
         * Triggered on mouse hover
         * @param enterEvent Pointer to enter event
         */
        void enterEvent(QEnterEvent* enterEvent) override;

        /**
         * Triggered on mouse leave
         * @param leaveEvent Pointer to leave event
         */
        void leaveEvent(QEvent* leaveEvent) override;

    private:
        LearningPageVideoStyledItemDelegate*    _delegate;
        QPersistentModelIndex                   _index;                     /** Editor model index */
        QHBoxLayout                             _mainLayout;                /** Main editor layout */
        QVBoxLayout                             _thumbnailLayout;                /**  */
        QVBoxLayout                             _textLayout;                /** Right text layout */
        QLabel                                  _thumbnailLabel;
        QTextBrowser                            _propertiesTextBrowser;
        mv::util::FileDownloader                _fileDownloader;
        QPixmap                                 _thumbnailPixmap;
        VideoOverlayWidget                      _overlayWidget;             /** Overlay widget which shows the associated plugin actions */
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    LearningPageVideoStyledItemDelegate(QObject* parent = nullptr);

    /**
     * Gives the size hint for \p option and model \p index
     * @param option Style option
     * @param index Model index to compute the size hint for
     * @return Size hint
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};