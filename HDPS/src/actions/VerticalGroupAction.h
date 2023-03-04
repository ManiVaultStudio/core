#pragma once

#include "GroupAction.h"

#include <QGridLayout>

namespace hdps::gui {

/**
 * Vertical group action class
 *
 * Group action which displays multiple child actions vertically with labels
 *
 * @author Thomas Kroes
 */
class VerticalGroupAction : public GroupAction
{
    Q_OBJECT

public:

    /** Label sizing types */
    enum class LabelSizingType {
        Auto,           /** The widest label in the group action determines the width for the other labels */
        Percentage,     /** The label width is a percentage of the available width */
        Fixed           /** The label width is fixed width in pixels */
    };

public:

    /** Widget class for vertical group action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param verticalGroupAction Pointer to vertical group action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, VerticalGroupAction* verticalGroupAction, const std::int32_t& widgetFlags);

    protected:
        VerticalGroupAction*    _verticalGroupAction;   /** Pointer to vertical group action which created this widget */
        QGridLayout             _layout;                /** Vertical layout */

        friend class VerticalGroupAction;
    };

protected:

    /**
     * Get widget representation of the vertical group action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param expanded Whether the group is initially expanded/collapsed
     */
    VerticalGroupAction(QObject* parent, const QString& title, const bool& expanded = false);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

public: // Label sizing

    /**
     * Get the label sizing type
     * @return Label sizing type enum
     */
    LabelSizingType getLabelSizingType() const;

    /**
     * Sets the label sizing type
     * @param labelSizingType Type of label sizing
     */
    void setLabelSizingType(const LabelSizingType& labelSizingType);

    /** Gets the user label width in percentages */
    std::uint32_t getLabelWidthPercentage() const;

    /**
     * Sets the user label width in percentages (sets the label sizing type to LabelSizingType::Percentage)
     * @param labelWidthPercentage User label width in percentages
     */
    void setLabelWidthPercentage(std::uint32_t labelWidthPercentage);

    /** Gets the user label width in pixels */
    std::uint32_t getLabelWidthFixed() const;

    /**
     * Sets the user label width in pixels (sets the label sizing type to LabelSizingType::Fixed)
     * @param labelWidthFixed User label width in pixels
     */
    void setLabelWidthFixed(std::uint32_t labelWidthFixed);

signals:

    /**
     * Signals that the actions changed to \p actions
     * @param actions Current actions in the group
     */
    void actionsChanged(const ConstWidgetActions& actions);

    /**
     * Signals that the group show labels option changed
     * @param showLabels Whether label are visible or not
     */
    void showLabelsChanged(const bool& showLabels);

    /**
     * Signals that the label sizing type changed
     * @param labelSizingType Label sizing type
     */
    void labelSizingTypeChanged(const LabelSizingType& labelSizingType);

    /**
     * Signals that the label width in percentages changed
     * @param labelWidthPercentage Label width in percentages
     */
    void labelWidthPercentageChanged(const std::uint32_t& labelWidthPercentage);

    /**
     * Signals that the label width in fixed pixels changed
     * @param labelWidthFixed Label width in fixed pixels
     */
    void labelWidthFixedChanged(const std::uint32_t& labelWidthFixed);

private:
    LabelSizingType                 _labelSizingType;               /** Type of label sizing */
    std::uint32_t                   _labelWidthPercentage;          /** User label width in percentages [0..100] */
    std::uint32_t                   _labelWidthFixed;               /** User label width in pixels */

    static const std::uint32_t      globalLabelWidthPercentage;     /** Global label width in percentages */
    static const std::uint32_t      globalLabelWidthFixed;          /** Global label width in pixels */
};

}
