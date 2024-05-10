// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStyledItemDelegate>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

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

    class EditorWidget : public QWidget
    {
    public:

        /**
         * Construct with pointer to \p parent widget
         * @param parent Pointer to parent widget
         */
        EditorWidget(QWidget* parent = nullptr);

        /**
         * Set editor data from model \p index
         * @param index Model index to fetch data from
         */
        void setEditorData(const QModelIndex& index);

    private:
        QPersistentModelIndex       _index;                     /** Editor model index */
        QHBoxLayout                 _mainLayout;                /** Main editor layout */
        QVBoxLayout                 _textLayout;                /** Right text layout */
        QLabel                      _thumbnailLabel;
        QLabel                      _titleLabel;
        QLabel                      _summaryLabel;
        mv::gui::FlowLayout         _tagsLayout;
        mv::util::FileDownloader    _fileDownloader;
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