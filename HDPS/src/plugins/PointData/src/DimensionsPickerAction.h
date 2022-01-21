#pragma once

#include "pointdata_export.h"

#include "DimensionsPickerHolder.h"
#include "DimensionsPickerItemModel.h"
#include "DimensionsPickerProxyModel.h"

#include "ModelResetter.h"

#include "actions/Actions.h"

using namespace hdps;
using namespace hdps::gui;

class QMenu;
class Points;

/**
 * Dimensions picker action class
 *
 * Action class for picking one or more data dimensions
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
     */
    DimensionsPickerAction(QObject* parent);

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

    /** Get selection proxy model */
    hdps::DimensionsPickerProxyModel* getProxyModel();

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

protected:
    
    /** Compute dimension statistics */
    void computeStatistics();

    /** Update the slider */
    void updateSlider();

    /** Update the dimension selection summary */
    void updateSummary();

    /** Select dimensions based on visibility */
    template <bool selectVisible>
    void selectDimensionsBasedOnVisibility()
    {
        const auto n = _selectionHolder.getNumberOfDimensions();

        if (n > INT_MAX)
        {
            assert(!"Number of dimensions too large -- Qt uses int!");
        }
        else
        {
            for (unsigned i{}; i < n; ++i)
                _selectionHolder.setDimensionEnabled(i, _selectionProxyModel->filterAcceptsRow(i, QModelIndex()) == selectVisible);

            const ModelResetter modelResetter(_selectionProxyModel.get());
        }
    }

protected:
    Dataset<Points>                                 _points;                                /** Smart pointer to points set */
    DimensionsPickerHolder                          _selectionHolder;                       /** Selection holder */
    std::unique_ptr<DimensionsPickerItemModel>      _selectionItemModel;                    /** Selection item model */
    std::unique_ptr<DimensionsPickerProxyModel>     _selectionProxyModel;                   /** Selection proxy model for filtering etc. */
    StringAction                                    _nameFilterAction;                      /** Name filter action */
    ToggleAction                                    _showOnlySelectedDimensionsAction;      /** Show only selected dimensions action */
    ToggleAction                                    _applyExclusionListAction;              /** Apply exclusion list action */
    ToggleAction                                    _ignoreZeroValuesAction;                /** Ignore zero values for statistics action */
    IntegralAction                                  _selectionThresholdAction;              /** Selection threshold action */
    StringAction                                    _summaryAction;                         /** Summary action */
    TriggerAction                                   _computeStatisticsAction;               /** Compute statistics action */
    TriggerAction                                   _selectVisibleAction;                   /** Select visible dimensions action */
    TriggerAction                                   _selectNonVisibleAction;                /** Select non visible dimensions action */
    TriggerAction                                   _loadSelectionAction;                   /** Load selection action */
    TriggerAction                                   _saveSelectionAction;                   /** Save selection action */
    TriggerAction                                   _loadExclusionAction;                   /** Load exclusion action */
    QMetaObject::Connection                         _summaryUpdateAwakeConnection;          /** Update summary view when idle */

    friend class Widget;
};