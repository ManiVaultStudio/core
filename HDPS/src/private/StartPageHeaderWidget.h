// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QVBoxLayout>
#include <QLabel>

/**
 * Header widget class
 *
 * Widget class for header with logo.
 *
 * @author Thomas Kroes
 */
class StartPageHeaderWidget : public QWidget
{
public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageHeaderWidget(QWidget* parent = nullptr);

protected:
    /**
     * Override resize event to resize header icon when main window is resized
     * @param event Pointer to resize event which occurred
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * Override show event to resize header icon on first window opening
     * @param event Pointer to show event which occurred
     */
    void showEvent(QShowEvent* event) override;

private:
    /**
     * Resize the main icon shown in the header to newSize x newSize pixels
     * @param newSize new icon width and height
     */
    void resizeIcon(const QSize& newSize);

protected:
    QVBoxLayout     _layout;        /** Main layout */
    QLabel          _headerLabel;   /** Header label */

private:
    QString         _iconName;      /** Name of background icon */
    int             _previousHeight;/** Height of main header icon */
};
