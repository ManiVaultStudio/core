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
class POINTDATA_EXPORT DimensionsPickerAction : public GroupAction
{
protected:

    /** Widget class for dimension selection action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dimensionSelectionAction Pointer to dimension selection action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, DimensionsPickerAction* dimensionSelectionAction, const std::int32_t& widgetFlags);
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
    DimensionsPickerAction(QObject* parent, const QString& title = "DimensionSelection");

    /** Destructor */
    ~DimensionsPickerAction();

    /**
     * Set dimensions
     * @param numDimensions Number of dimensions
     * @param names Dimension names
     */
    void setDimensions(const std::uint32_t numDimensions, const std::vector<QString>& names);

    /** Get list of enabled dimensions */
    std::vector<bool> getEnabledDimensions() const;

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
     * Clear the selection and select a single dimension by name
     * @param dimensionName Name of the dimension to select
     * @param clearExisiting Whether to clear the current selection
     */
    void selectDimension(const QString& dimensionName, bool clearExisiting = false);

    /**
     * Clear the selection and select a single dimension by index
     * @param dimensionIndex Index of the dimension to select
     * @param clearExisiting Whether to clear the current selection
     */
    void selectDimension(const std::int32_t& dimensionIndex, bool clearExisiting = false);

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

public: // Settings

    /**
     * Set action value from variant
     * @param value Value
     */
    void setValue(const QVariant& value) override final;

    /**
     * Convert value to variant
     * @return Value as variant
     */
    QVariant valueToVariant() const override final;

    /**
     * Convert default value to variant
     * @return Default value as variant
     */
    QVariant defaultValueToVariant() const override final;

protected:
    
    /** Compute dimension statistics */
    void computeStatistics();

    /** Update the slider */
    void updateSlider();

    /** Update the dimension selection summary */
    void updateSummary();

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
};