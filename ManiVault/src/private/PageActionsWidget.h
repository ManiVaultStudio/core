// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageActionsTreeModel.h"
#include "PageActionsFilterModel.h"

#include <util/Serializable.h>

#include <widgets/HierarchyWidget.h>

#include <QWidget>

/**
 * Page actions widget class
 *
 * Widget class for listing page actions.
 *
 * @author Thomas Kroes
 */
class PageActionsWidget : public QWidget, public mv::util::Serializable
{
public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     * @param title Title shown in the header
     * @param restyle Whether to change the style to seamlessly integrate with the page (false for use outside of page widget)
     */
    PageActionsWidget(QWidget* parent, const QString& title, bool restyle = true);

    /**
     * Get layout
     * @return Reference to main layout
     */
    QVBoxLayout& getLayout();

    /**
     * Get model
     * @return Reference to model
     */
    PageActionsTreeModel& getModel();

    /**
     * Get filter model
     * @return Reference to filter model
     */
    PageActionsFilterModel& getFilterModel();

    /**
     * Get hierarchy widget
     * @return Reference to hierarchy widget
     */
    mv::gui::HierarchyWidget& getHierarchyWidget();

private:

    /**
     * Open persistent editor for \p rowIndex
     * @param rowIndex Index of the row for which to open the persistent editor
     */
    void openPersistentEditor(int rowIndex);

    /**
     * Close persistent editor for \p rowIndex
     * @param rowIndex Index of the row for which to close the persistent editor
     */
    void closePersistentEditor(int rowIndex);
    
    /** Update all  custom style elements */
    void updateCustomStyle();

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save to variant map
     * @return Variant map
     */
    QVariantMap toVariantMap() const override;

private:
    QVBoxLayout                 _layout;            /** Main layout */
    PageActionsTreeModel        _model;             /** Model which contains start page actions */
    PageActionsFilterModel      _filterModel;       /** Model for filtering and sorting start page actions */
    mv::gui::HierarchyWidget    _hierarchyWidget;   /** Widget for displaying the actions */
    bool                        _restyle;           /** Remember whether the restyle flag was set upon creation */
};
