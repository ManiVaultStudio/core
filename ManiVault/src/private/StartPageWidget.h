// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageWidget.h"

#include "StartPageContentWidget.h"

#include <QWidget>

/**
 * Start page widget class
 *
 * Shows various project-related operations.
 *
 * @author Thomas Kroes
 */
class StartPageWidget : public PageWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    StartPageWidget(QWidget* parent = nullptr);

    /**
     * Override show event to update start page actions
     * @param showEvent Pointer to show event which occurred
     */
    void showEvent(QShowEvent* showEvent) override;

protected:
    StartPageContentWidget  _startPageContentWidget;    /** Widget which contains all the actions */
};
