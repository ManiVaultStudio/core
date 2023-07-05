// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

/**
 * Logo widget class
 *
 * Widget class for displaying the application logo
 *
 * @author Thomas Kroes
 */
class LogoWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    LogoWidget(QWidget* parent = nullptr);

protected:
    QVBoxLayout     _layout;        /** Main layout */
    QLabel          _headerLabel;   /** Header label */
};

