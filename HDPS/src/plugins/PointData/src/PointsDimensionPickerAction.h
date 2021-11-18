#pragma once

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"

#include "PointData.h"

#include "pointdata_export.h"

using namespace hdps;
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
     * @param title Title of the action
     */
    PointsDimensionPickerAction(QObject* parent, const QString& title);

    /**
     * Set the points dataset from which the dimension will be picked
     * @param points Dataset reference to points dataset
     */
    void setPointsDataset(const Dataset<Points>& points);

    /** Get the names of the dimensions */
    QStringList getDimensionNames() const;

    /** Get the number of currently loaded dimensions */
    std::uint32_t getNumberOfDimensions() const;

    /** Get the current dimension index */
    std::int32_t getCurrentDimensionIndex() const;

    /** Get the current dimension name */
    QString getCurrentDimensionName() const;

    /**
     * Set the current dimension by index
     * @param dimensionIndex Index of the current dimension
     */
    void setCurrentDimensionIndex(const std::int32_t& dimensionIndex);

    /**
     * Set the current dimension by name
     * @param dimensionName Name of the current dimension
     */
    void setCurrentDimensionName(const QString& dimensionName);

    /**
     * Set the default dimension index
     * @param defaultDimensionIndex Default dimension index
     */
    void setDefaultDimensionIndex(const std::int32_t& defaultDimensionIndex);

    /**
     * Set the default dimension name
     * @param defaultDimensionName Default dimension name
     */
    void setDefaultDimensionName(const QString& defaultDimensionName);

signals:

    /**
     * Signals that the current dimension index changed
     * @param currentDimensionIndex Index of the current dimension
     */
    void currentDimensionIndexChanged(const std::int32_t& currentDimensionIndex);

    /**
     * Signals that the current dimension name changed
     * @param currentDimensionName Name of the current dimension
     */
    void currentDimensionNameChanged(const QString& currentDimensionName);

public: /** Action getters */

    OptionAction& getCurrentDimensionAction() { return _currentDimensionAction; }

protected:
    Dataset<Points>     _points;                    /** Dataset reference to points dataset from which the dimension will be picked */
    OptionAction        _currentDimensionAction;    /** Current dimension action */
};
