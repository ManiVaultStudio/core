// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QHBoxLayout>
#include <QWidget>

/**
 * Page content widget class
 *
 * Base widget class for showing page content
 *
 * @author Thomas Kroes
 */
class PageContentWidget : public QWidget
{
protected:

    /**
     * Construct with \p orientation and pointer to \p parent widget
     * @param orientation Orientation of the content
     * @param parent Pointer to parent widget
     */
    PageContentWidget(const Qt::Orientation& orientation, QWidget* parent = nullptr);

public:

    /**
     * Create header label
     * @param title Title of the header
     * @param tooltip Tooltip of the header
     * @return Pointer to newly created header label
     */
    static QLabel* createHeaderLabel(const QString& title, const QString& tooltip);

protected:

    /**
     * Get main layout
     * @return Main vertical box layout
     */
    QVBoxLayout& getMainLayout() {
        return _mainLayout;
    }

    /**
     * Get columns layout
     * @return Columns horizontal box layout
     */
    QHBoxLayout& getColumnsLayout() {
        return _columnsLayout;
    }

    /**
     * Get rows layout
     * @return Rows horizontal box layout
     */
    QVBoxLayout& getRowsLayout() {
        return _rowsLayout;
    }

private:
    QVBoxLayout     _mainLayout;        /** Main layout */
    QHBoxLayout     _columnsLayout;     /** Columns layout */
    QVBoxLayout     _rowsLayout;        /** Rows layout */
};