// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <widgets/IconLabel.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPersistentModelIndex>
#include <QString>
#include <QWidget>

/**
 * Page action delegate editor widget class
 *
 * For custom page action display in a view.
 *
 * @author Thomas Kroes
 */
class PageActionDelegateEditorWidget : public QWidget
{
    Q_OBJECT

public:
    
    /**
     * Construct delegate editor widget with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    PageActionDelegateEditorWidget(QWidget* parent = nullptr);

    /**
     * Set editor data from model \p index
     * @param index Model index to fetch data from
     */
    void setEditorData(const QModelIndex& index);

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Triggered on mouse hover
     * @param enterEvent Pointer to enter event
     */
    void enterEvent(QEnterEvent* enterEvent) override;

    /**
     * Triggered on mouse leave
     * @param event Pointer to event
     */
    void leaveEvent(QEvent* event) override;

private:

    /** Update all text labels and takes care of elidation */
    void updateTextLabels();
    
    /** Show the info widget when mouse hovered */
    void updateInfoWidgetVisibility();
    
    /** update custom styles */
    void updateCustomStyle();

private:
    QPersistentModelIndex       _index;                     /** Editor model index */
    QHBoxLayout                 _mainLayout;                /** Main editor layout */
    QVBoxLayout                 _iconLayout;                /** Left icon layout */
    QLabel                      _iconLabel;                 /** Left icon label */
    QVBoxLayout                 _textLayout;                /** Right text layout */
    QHBoxLayout                 _primaryTextLayout;         /** Primary layout with title and comments labels */
    QLabel                      _titleLabel;                /** Title label */
    QLabel                      _metaDataLabel;             /** Label with metadata (maybe empty) */
    QHBoxLayout                 _secondaryTextLayout;       /** Secondary layout with subtitle and info labels */
    QLabel                      _subtitleLabel;             /** Subtitle label */
    QWidget                     _infoWidget;                /** Widget for info labels */
    QHBoxLayout                 _infoLayout;                /** Layout for preview, tags and meta info popups */
    mv::gui::IconLabel          _previewIconLabel;          /** Icon label for preview image */
    mv::gui::IconLabel          _metaDataIconLabel;         /** Icon label for meta data */
    mv::gui::IconLabel          _tagsIconLabel;             /** Icon label for tags */
    mv::gui::IconLabel          _downloadUrls;     /** Icon label for requires download */
    mv::gui::IconLabel          _contributorsIconLabel;     /** Icon label for contributors */
};
