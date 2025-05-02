// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "StretchAction.h"

#include <QGridLayout>

namespace mv::gui {

/**
 * Group action class
 *
 * Contains zero or more child actions and either displays them horizontally or vertically.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT GroupAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        Horizontal  = 0x00001,      /** Actions are arranged horizontally in the group */
        Vertical    = 0x00002,      /** Actions are arranged vertically in the group */
        NoMargins   = 0x00010,      /** Do not apply margins to widget layout */
        WithMargins = 0x00020,      /** Apply standard margins to widget layout */

        Default = Horizontal
    };

    /** Label sizing types (in case of vertical group) */
    enum class LabelSizingType {
        Auto,           /** The widest label in the group action determines the width for the other labels */
        Percentage,     /** The label width is a percentage of the available width */
        Fixed           /** The label width is fixed width in pixels */
    };

public:

    using WidgetFlagsMap = QMap<const WidgetAction*, std::int32_t>;
    using WidgetConfigurationFunctionsMap = QMap<const WidgetAction*, WidgetConfigurationFunction>;
    using WidgetFlagsList = QList<std::int32_t>;

public: // Widgets

    /** Widget class for arranging the groups actions vertically */
    class CORE_EXPORT VerticalWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param groupAction Pointer to group action which creates the widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        VerticalWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags);

        /** Destructor */
        ~VerticalWidget() override;

    protected:
        GroupAction*    _groupAction;   /** Pointer to group action which created the widget */
        QGridLayout     _layout;        /** Vertical layout */

        friend class GroupAction;
    };

    /** Widget class for arranging the groups actions horizontally */
    class CORE_EXPORT HorizontalWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param groupAction Pointer to group action which creates the widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        HorizontalWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags);

        /** Destructor */
        ~HorizontalWidget() override;

    protected:
        GroupAction* _groupAction;   /** Pointer to group action which created the widget */

        friend class GroupAction;
    };

protected: // Widgets

    /**
     * Get widget representation of the group action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Group title
     * @param expanded Whether the group is initially expanded/collapsed
     * @param alignment Item alignment
     */
    Q_INVOKABLE GroupAction(QObject* parent, const QString& title, const bool& expanded = false, const Qt::AlignmentFlag& alignment = Qt::AlignmentFlag::AlignLeft);

    /**
     * Get item alignment
     * @return Item alignment
     */
    Qt::AlignmentFlag getAlignment() const;

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

public: // Stretch

    /**
     * Add stretch action to the group (this will add a stretch to the group layout in the widget)
     * @param stretch Layout stretch
     * @return Pointer to created stretch action (group action is owner of the action)
     */
    StretchAction* addStretch(std::int32_t stretch = 1);

public: // Actions management

    /**
     * Add \p action to the group
     * @param action Pointer to action to add
     * @param widgetFlags Action widget flags (default flags if -1)
     * @param widgetConfigurationFunction When set, overrides the standard widget configuration function in the widget action
     * @param load Currently not used
     */
    virtual void addAction(WidgetAction* action, std::int32_t widgetFlags = -1, WidgetConfigurationFunction widgetConfigurationFunction = WidgetConfigurationFunction(), bool load = true);

    /**
     * Remove \p action from the group
     * @param action Pointer to action to add
     */
    virtual void removeAction(WidgetAction* action) final;

    /** Remove all actions */
    virtual void removeAllActions() final;

    /** Remove all actions (alias for GroupAction::removeAllActions()) */
    virtual void clear() final;

    /**
     * Get actions
     * @return Vector of pointers to actions
     */
    virtual WidgetActions getActions() final;

    /**
     * Get const actions
     * @return Vector of const pointers to actions
     */
    virtual ConstWidgetActions getConstActions() final;

    /**
     * Get widget flags map (maps widget action pointer to widget creation flags)
     * @return Widget flags map
     */
    WidgetFlagsMap getWidgetFlagsMap();

    /**
     * Get widget configuration functions map (maps widget action pointer to widget configuration function)
     * @return Widget configuration functions map
     */
    WidgetConfigurationFunctionsMap getWidgetConfigurationFunctionsMap();

public: // Label sizing for vertical layout

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

private:

    /** Sort added actions based on their sort index */
    virtual void sortActions() final;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the actions changed to \p actions
     * @param actions Update vector of pointers to actions in the group
     */
    void actionsChanged(const WidgetActions& actions);

    /** Signals that the group got expanded */
    void expanded();

    /** Signals that the group got collapsed */
    void collapsed();

    /**
     * Signals that the group read-only status changed to \p readOnly
     * @param readOnly Read-only status
     */
    void readOnlyChanged(const bool& readOnly);

    /**
     * Signals that the group show labels option changed to \p showLabels
     * @param showLabels Whether label are visible or not
     */
    void showLabelsChanged(const bool& showLabels);

    /**
     * Signals that the label sizing type changed to \p labelSizingType
     * @param labelSizingType Label sizing type
     */
    void labelSizingTypeChanged(const LabelSizingType& labelSizingType);

    /**
     * Signals that the label width in percentages changed to \p labelWidthPercentage
     * @param labelWidthPercentage Label width in percentages
     */
    void labelWidthPercentageChanged(const std::uint32_t& labelWidthPercentage);

    /**
     * Signals that the label width in fixed pixels changed to \p labelWidthFixed
     * @param labelWidthFixed Label width in fixed pixels
     */
    void labelWidthFixedChanged(const std::uint32_t& labelWidthFixed);

private:
    Qt::AlignmentFlag                   _alignment;                         /** Item alignment */
    bool                                _expanded;                          /** Whether or not the group is expanded */
    bool                                _readOnly;                          /** Whether or not the group is read-only */
    WidgetActions                       _actions;                           /** Vector of pointers to widget actions in the group */
    bool                                _showLabels;                        /** Whether to show labels or not */
    WidgetFlagsMap                      _widgetFlagsMap;                    /** Maps widget action pointer to widget creation flags */
    WidgetConfigurationFunctionsMap     _widgetConfigurationFunctionsMap;   /** Maps widget action pointer to widget configuration function */

private: // Specific settings for vertical layout
    LabelSizingType     _labelSizingType;           /** Type of label sizing */
    std::uint32_t       _labelWidthPercentage;      /** User label width in percentages [0..100] */
    std::uint32_t       _labelWidthFixed;           /** User label width in pixels */

    static const std::uint32_t          globalLabelWidthPercentage;     /** Global label width in percentages */
    static const std::uint32_t          globalLabelWidthFixed;          /** Global label width in pixels */
};

}

Q_DECLARE_METATYPE(mv::gui::GroupAction)

inline const auto groupActionMetaTypeId = qRegisterMetaType<mv::gui::GroupAction*>("mv::gui::GroupAction");