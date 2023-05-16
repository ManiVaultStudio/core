#pragma once

#include "WidgetAction.h"
#include "HorizontalGroupAction.h"
#include "NumericalRangeAction.h"

#include <QVBoxLayout>

namespace hdps::gui {

/**
 * Rectangle action class
 *
 * Base rectangle class for storing a rectangle and interfacing with it
 *
 * @author Thomas Kroes
 */
template<typename RectangleType, typename NumericalRangeActionType>
class RectangleAction : public WidgetAction
{
public:

    /** Templated classes with Q_OBJECT macro are not allowed, so use function pointers in stead */
    using RectangleChangedCB        = std::function<void()>;

    /** Axis enum for distinguishing between x- and y axis range of the rectangle */
    enum class Axis {
        X = 0,  /** Along x-axis */
        Y,      /** Along y-axis */

        Count
    };

    /** Describes the widget settings */
    enum WidgetFlag {
        LineEdit = 0x00001,      /** Widget includes a line edit */

        Default = LineEdit
    };

    /** Widget class for editing the rectangle action */
    class EditWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param rectangleAction Pointer to rectangle action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        EditWidget(QWidget* parent, RectangleAction* rectangleAction, const std::int32_t& widgetFlags) :
            WidgetActionWidget(parent, rectangleAction, widgetFlags),
            _groupAction(this, "Group")
        {
            _groupAction.setShowLabels(false);
            
            _groupAction.addAction(&rectangleAction->getRangeAction(Axis::X));
            _groupAction.addAction(&rectangleAction->getRangeAction(Axis::Y));

            auto layout = new QVBoxLayout();

            layout->setContentsMargins(0, 0, 0, 0);
            layout->addWidget(_groupAction.createWidget(this));

            setLayout(layout);
        }

    private:
        HorizontalGroupAction   _groupAction;       /** For displaying the ranges */

        friend class RectangleAction;
    };

protected:

    /**
     * Get widget representation of the rectangle action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new EditWidget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param rectangle Rectangle
     * @param defaultRectangle Default rectangle
     */
    RectangleAction(QObject * parent, const QString& title, const RectangleType& rectangle = RectangleType()) :
        WidgetAction(parent, title),
        _rangeAction{ NumericalRangeActionType(this, "X-range"), NumericalRangeActionType(this, "Y-range") }
    {
        setDefaultWidgetFlags(WidgetFlag::Default);

        getRangeAction(Axis::X).setDefaultWidgetFlags(NumericalRangeActionType::MinimumSpinBox | NumericalRangeActionType::MaximumSpinBox);
        getRangeAction(Axis::Y).setDefaultWidgetFlags(NumericalRangeActionType::MinimumSpinBox | NumericalRangeActionType::MaximumSpinBox);

        getRangeAction(Axis::X).getRangeMinAction().setPrefix("xMin: ");
        getRangeAction(Axis::X).getRangeMaxAction().setPrefix("xMax: ");

        getRangeAction(Axis::Y).getRangeMinAction().setPrefix("yMin: ");
        getRangeAction(Axis::Y).getRangeMaxAction().setPrefix("yMax: ");
    }

    /**
     * Get rectangle
     * @return Rectangle
     */
    RectangleType getRectangle() const
    {
        RectangleType rectangle;

        return rectangle;
    }

    /**
     * Set rectangle to \p rectangle
     * @param rectangle Rectangle
     */
    void setRectangle(const RectangleType& rectangle)
    {
        /*
        if (rectangle == _rectangle)
            return;

        _rectangle = rectangle;

        _rectangleChanged();
        */
    }

public: // Action getters

    NumericalRangeActionType& getRangeAction(const Axis& axis) { return _rangeAction[static_cast<int>(axis)]; }

    const NumericalRangeActionType& getRangeAction(const Axis& axis) const { return _rangeAction[static_cast<int>(axis)]; }

private:
    NumericalRangeActionType    _rangeAction[static_cast<int>(Axis::Count)];    /** Range actions for the x- and y axis of the rectangle */

protected:
    RectangleChangedCB          _rectangleChanged;                              /** Callback which is called when the rectangle changed */
};

}
