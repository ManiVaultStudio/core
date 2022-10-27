#pragma once

#include "WidgetAction.h"

#include <QTreeWidget>
#include <QResizeEvent>
#include <QVBoxLayout>

class QWidget;
class QGridLayout;

namespace hdps {

namespace gui {

/**
 * Group action class
 *
 * Groups multiple actions
 * When added to a dataset, the widget is added to the data properties widget as a section
 * 
 * @author Thomas Kroes
 */
class GroupAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Label sizing types */
    enum class LabelSizingType {
        Auto,           /** The widest label in the group action determines the width for the other labels */
        Percentage,     /** The label width is a percentage of the available width */
        Fixed           /** The label width is fixed width in pixels */
    };

    /**
     * Group widget class for widget action group
     */
    class FormWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param groupAction Pointer to group action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        FormWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags);

        /** Get grid layout */
        QGridLayout* layout();

    protected:
        QGridLayout*    _layout;        /** Main grid layout */

        friend class GroupAction;
    };

    /**
     * Get widget representation of the group action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new FormWidget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param expanded Whether the group is initially expanded/collapsed
     */
    GroupAction(QObject* parent, const bool& expanded = false);

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param widgetActions Widget actions to initialize with
     * @param expanded Whether the group is initially expanded/collapsed
     */
    GroupAction(QObject* parent, WidgetActions widgetActions, const bool& expanded = false);

    /** Set expanded/collapsed */
    void setExpanded(const bool& expanded);

    /** Expand the group */
    void expand();

    /** Collapse the group */
    void collapse();

    /** Toggle the group */
    void toggle();

    /** Get whether the group is expanded */
    bool isExpanded() const;

    /** Get whether the group is collapsed */
    bool isCollapsed() const;

    /** Gets the group read-only */
    bool isReadOnly() const;

    /**
     * Sets the group read-only
     * @param readOnly Whether the group is read-only
     */
    void setReadOnly(const bool& readOnly);

    /** Gets whether the group should show labels or not */
    bool getShowLabels() const;

    /**
     * Sets whether the group should show labels or not
     * @param showLabels Whether the group should show labels or not
     */
    void setShowLabels(bool showLabels);

    /** Gets the label sizing type */
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

    /**
     * Add widget action using stream in operator
     * @param widgetAction Reference to widget action
     */
    void operator << (WidgetAction& widgetAction)
    {
        _widgetActions << &widgetAction;

        emit actionsChanged(_widgetActions);
    }

    /**
     * Set actions
     * @param widgetActions Widget actions
     */
    void setActions(const WidgetActions& widgetActions = WidgetActions());

    /**
     * Get sorted widget actions
     * @return Vector of sorted widget actions
     */
    WidgetActions getSortedWidgetActions() const;

signals:

    /** Signals that the actions changed */
    void actionsChanged(const WidgetActions& widgetActions);

    /** Signals that the group got expanded */
    void expanded();

    /** Signals that the group got collapsed */
    void collapsed();

    /**
     * Signals that the group read-only status changed
     * @param readOnly Read-only status
     */
    void readOnlyChanged(const bool& readOnly);

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
    bool                            _expanded;                      /** Whether or not the group is expanded */
    bool                            _readOnly;                      /** Whether or not the group is read-only */
    WidgetActions          _widgetActions;                 /** Widget actions */
    bool                            _showLabels;                    /** Whether to show labels or not */
    LabelSizingType                 _labelSizingType;               /** Type of label sizing */
    std::uint32_t                   _labelWidthPercentage;          /** User label width in percentages [0..100] */
    std::uint32_t                   _labelWidthFixed;               /** User label width in pixels */
    static const std::uint32_t      globalLabelWidthPercentage;     /** Global label width in percentages */
    static const std::uint32_t      globalLabelWidthFixed;          /** Global label width in pixels */
};

/**
 * Print group action to console
 * @param debug Debug
 * @param groupAction Reference to group action
 */
inline QDebug operator << (QDebug debug, const GroupAction& groupAction)
{
    debug << groupAction.getSettingsPath();

    return debug.space();
}

/**
 * Print group action to console
 * @param debug Debug
 * @param groupAction Pointer to group action
 */
inline QDebug operator << (QDebug debug, const GroupAction* groupAction)
{
    Q_ASSERT(groupAction != nullptr);

    debug << groupAction->getSettingsPath();

    return debug.space();
}

}
}
