// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>

namespace mv::gui {

/**
 * Widget action example widget class
 *
 * Provides basic routines to assemble an action example widget
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT WidgetActionExampleWidget : public QWidget
{
public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    WidgetActionExampleWidget(QWidget* parent = nullptr);

    /**
     * Add a markdown section with \p markdown text to the action example widget
     * @param markdown Markdown formatted string
     */
    void addMarkDownSection(const QString& markdown);

    /**
     * Add \p widget
     * @param widget Pointer to widget
     */
    void addWidget(QWidget* widget);

private:
    QVBoxLayout     _layout;    /** Main layout */
};

}
