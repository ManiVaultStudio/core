// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

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
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    PageContentWidget(QWidget* parent = nullptr);

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
    };

    /**
     * Get columns layout
     * @return Columns horizontal box layout
     */
    QHBoxLayout& getColumnsLayout() {
        return _columnsLayout;
    };

    /**
     * Get rows layout
     * @return Rows horizontal box layout
     */
    QVBoxLayout& getRowsLayout() {
        return _rowsLayout;
    };

private slots:
    
    /** Update custom theme parts not caught by the system itself */
    void updateCustomStyle() ;

private:
    QVBoxLayout     _mainLayout;        /** Main layout */
    QHBoxLayout     _columnsLayout;     /** Columns layout */
    QVBoxLayout     _rowsLayout;        /** Rows layout */
};