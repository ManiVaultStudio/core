// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageAction.h"

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

    void updateIcon();                      /** Updates the icon label pixmap */
    void updateTitle();                     /** Updates the title label text */
    void updateSubtitle();                  /** Updates the subtitle label text */
    void updateTooltip();                   /** Updates the tooltip */
    void updateMetadata();                  /** Updates the meta data label text */
    void updateSubActions();                /** Update the sub-actions section */
    void updateOverlayWidgetVisibility();   /** Update the visibility of the info widget (dependent on the mouse position) */
    void updateCustomStyle();               /** Update custom styles */

private:
    PageActionSharedPtr     _pageAction;                /** Shared pointer to page action */    
    QPersistentModelIndex   _index;                     /** Editor model index */
    QHBoxLayout             _mainLayout;                /** Main editor layout */
    QVBoxLayout             _iconLayout;                /** Left icon layout */
    QLabel                  _iconLabel;                 /** Left icon label */
    QVBoxLayout             _textLayout;                /** Right text layout */
    QHBoxLayout             _primaryTextLayout;         /** Primary layout with title and comments labels */
    QLabel                  _titleLabel;                /** Title label */
    QLabel                  _metaDataLabel;             /** Label with metadata (maybe empty) */
    QHBoxLayout             _secondaryTextLayout;       /** Secondary layout with subtitle and sub-actions */
    QLabel                  _subtitleLabel;             /** Subtitle label */
    QWidget                 _subActionsWidget;          /** Widget for sub-actions */
    QHBoxLayout             _subActionsLayout;          /** Layout for sub-actions */
};
