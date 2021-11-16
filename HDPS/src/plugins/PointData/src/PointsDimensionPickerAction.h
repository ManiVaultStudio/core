#pragma once

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"
#include "util/DatasetRef.h"

#include "PointData.h"

#include "pointdata_export.h"

using namespace hdps::gui;
using namespace hdps::util;

/**
 * Points dimension picker action class
 *
 * Action class for picking a dimension from a points dataset
 * Also support searching for a dimension
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointsDimensionPickerAction : public WidgetAction
{
Q_OBJECT

public:

    /** Widget class for points dimension picker action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param pointsDimensionPickerAction Pointer to points dimension picker action
         */
        Widget(QWidget* parent, PointsDimensionPickerAction* pointsDimensionPickerAction);

    protected:
        friend class PointsDimensionPickerAction;
    };

protected:

    /**
     * Get widget representation of the points dimension action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /** 
     * Constructor
     * @param parent Pointer to parent object
     */
    PointsDimensionPickerAction(QObject* parent);

    /**
     * Set the points dataset from which the dimension will be picked
     * @param points Dataset reference to points dataset
     */
    void setPointsDataset(const DatasetRef<Points>& points);

    /** Get the names of the dimensions */
    QStringList getDimensionNames() const;

    /**
     * Set the current dimension by name
     * @param dimensionName Current dimension name
     */
    void setCurrentDimensionName(const QString& dimensionName);

public: /** Action getters */

    OptionAction& getDimensionSelectAction() { return _dimensionSelectAction; }
    OptionAction& getDimensionSearchAction() { return _dimensionSearchAction; }

protected:
    DatasetRef<Points>  _points;                    /** Dataset reference to points dataset from which the dimension will be picked */
    OptionAction        _dimensionSelectAction;     /** Pick a dimension by selecting */
    OptionAction        _dimensionSearchAction;     /** Pick a dimension by searching */
};
