// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "pointdata_export.h"

#include "DimensionsPickerHolder.h"
#include "DimensionsPickerItemModel.h"
#include "DimensionsPickerProxyModel.h"

#include "DimensionsPickerFilterAction.h"
#include "DimensionsPickerSelectAction.h"
#include "DimensionsPickerMiscellaneousAction.h"

#include "ModelResetter.h"

#include "actions/Actions.h"

#include <QTableView>

using namespace hdps;
using namespace hdps::gui;

class QMenu;
class Points;

/**
 * Dimensions picker action class
 *
 * Action class for picking one or more points dimensions
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT DimensionsPickerAction : public WidgetAction
{

    Q_OBJECT

protected:

    /** Widget class for dimension selection action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dimensionsPickerAction Pointer to dimensions picker action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, DimensionsPickerAction* dimensionsPickerAction, const std::int32_t& widgetFlags);

    protected:

        /**
         * Update the table view source model
         * @param model Pointer to table view source model
         */
        void updateTableViewModel(QAbstractItemModel* model);

    protected:
        QTableView      _tableView;
    };

    /**
     * Get widget representation of the dimension selection action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE DimensionsPickerAction(QObject* parent, const QString& title);

    /** Destructor */
    ~DimensionsPickerAction();

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public:

    /**
     * Set dimensions
     * @param numDimensions Number of dimensions
     * @param names Dimension names
     */
    void setDimensions(const std::uint32_t numDimensions, const std::vector<QString>& names);

    /**
     * Get indices of the currently selected dimensions
     * @return Indices of the currently selected dimensions
     */
    const QVector<std::int32_t> getSelectedDimensions() const;

    /**
     * Get enabled dimension
     * @return Vector of booleans where each boolean indicates whether the dimension is enabled or not
     */
    std::vector<bool> getEnabledDimensions() const;

    /**
     * Get the input points dataset
     * @param Input points dataset
     */
    Dataset<Points> getPointsDataset() const;

    /**
     * Set the input points dataset
     * @param points Smart pointer to points dataset
     */
    void setPointsDataset(const Dataset<Points>& points);

    /**
     * Get selection picker holder
     * @return Reference to dimensions picker holder
     */
    DimensionsPickerHolder& getHolder();

    /**
     * Get item model
     * @return Reference to item model
     */
    DimensionsPickerItemModel& getItemModel();

    /**
     * Get selection proxy model
     * @return Reference to selection proxy model
     */
    DimensionsPickerProxyModel& getProxyModel();

public:

    /**
     * Set name filter
     * @param nameFilter Name filter
     */
    void setNameFilter(const QString& nameFilter);

    /**
     * Set show only selected dimensions
     * @param showOnlySelectedDimensions Show only selected dimensions
     */
    void setShowOnlySelectedDimensions(const bool& showOnlySelectedDimensions);

    /**
     * Set apply exclusion list
     * @param applyExclusionList Apply exclusion list
     */
    void setApplyExclusionList(const bool& applyExclusionList);

    /**
     * Set ignore zero values
     * @param ignoreZeroValues Ignore zero values
     */
    void setIgnoreZeroValues(const bool& ignoreZeroValues);

    /**
     * Load selection from file
     * @param fileName Selection file name
     */
    void loadSelectionFromFile(const QString& fileName);

    /**
     * Load exclusion from file
     * @param fileName Exclusion file name
     */
    void loadExclusionFromFile(const QString& fileName);

    /**
     * Save selection from file
     * @param fileName Selection file name
     */
    void saveSelectionToFile(const QString& fileName);

    /**
     * Select a single dimension by name (and possibly clear the selection)
     * @param dimensionName Name of the dimension to select
     * @param clearExisiting Whether to clear the current selection
     */
    void selectDimension(const QString& dimensionName, bool clearExisiting = false);

    /**
     * Select a single dimension by index (and possibly clear the selection)
     * @param dimensionIndex Index of the dimension to select
     * @param clearExisiting Whether to clear the current selection
     */
    void selectDimension(const std::int32_t& dimensionIndex, bool clearExisiting = false);

    /**
     * Select one or more dimensions by index (and possibly clear the selection)
     * @param dimensionIndices Indices of the dimensions to select
     * @param clearExisiting Whether to clear the current selection
     */
    void selectDimensions(const QVector<std::int32_t>& dimensionIndices, bool clearExisiting = true);

    /** Select dimensions based on visibility */
    template <bool selectVisible>
    void selectDimensionsBasedOnVisibility()
    {
        const auto n = _holder.getNumberOfDimensions();

        if (n > INT_MAX)
        {
            assert(!"Number of dimensions too large -- Qt uses int!");
        }
        else
        {
            for (unsigned i{}; i < n; ++i)
                _holder.setDimensionEnabled(i, _proxyModel->filterAcceptsRow(i, QModelIndex()) == selectVisible);

            const ModelResetter modelResetter(_proxyModel.get());
        }
    }

protected:
    
    /** Compute dimension statistics */
    void computeStatistics();

    /** Update the slider */
    void updateSlider();

    /** Update the dimension selection summary */
    void updateSummary();

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

signals:

    /**
     * Signals that the proxy model changed
     * @param dimensionsPickerProxyModel Pointer to dimensions picker proxy model
     */
    void proxyModelChanged(DimensionsPickerProxyModel* dimensionsPickerProxyModel);

    /**
     * Signals that the selected dimensions changed
     * @param selectedDimensionIndices Indices of the currently selected dimensions
     */
    void selectedDimensionsChanged(const QVector<std::int32_t>& selectedDimensionIndices);

protected: // Action getters

    DimensionsPickerFilterAction& getFilterAction() { return _filterAction; }
    DimensionsPickerSelectAction& getSelectAction() { return _selectAction; }
    StringAction& getSummaryAction() { return _summaryAction; }
    DimensionsPickerMiscellaneousAction& getMiscellaneousAction() { return _miscellaneousAction; }

protected:
    Dataset<Points>                                 _points;                            /** Smart pointer to points set */
    DimensionsPickerHolder                          _holder;                            /** Selection holder */
    std::unique_ptr<DimensionsPickerItemModel>      _itemModel;                         /** Selection item model */
    std::unique_ptr<DimensionsPickerProxyModel>     _proxyModel;                        /** Selection proxy model for filtering etc. */
    StringAction                                    _summaryAction;                     /** Summary action */
    DimensionsPickerFilterAction                    _filterAction;                      /** Filter action */
    DimensionsPickerSelectAction                    _selectAction;                      /** Select action */
    DimensionsPickerMiscellaneousAction             _miscellaneousAction;               /** Miscellaneous settings action */
    QMetaObject::Connection                         _summaryUpdateAwakeConnection;      /** Update summary view when idle */

    friend class Widget;
    friend class AbstractActionsManager;
};

Q_DECLARE_METATYPE(DimensionsPickerAction)

inline const auto dimensionsPickerActionMetaTypeId = qRegisterMetaType<DimensionsPickerAction*>("DimensionsPickerAction");