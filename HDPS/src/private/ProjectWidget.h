// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>

/**
 * Project widget class
 *
 * Widget class for showing the workspace of the current project.
 *
 * @author Thomas Kroes
 */
class ProjectWidget : public QWidget
{
public:

    /**
     * Construct project widget from \p parent widget
     * @param parent Pointer to parent widget
     */
    ProjectWidget(QWidget* parent = nullptr);
};
