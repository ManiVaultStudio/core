#pragma once

#include "WidgetActionWidget.h"
#include "WidgetActionLabel.h"
#include "WidgetActionResetButton.h"

#include <QWidgetAction>
#include <QMenu>
#include <QDebug>
#include <QFlags>

class QLabel;

namespace hdps {

class DataHierarchyItem;

namespace gui {

/**
 * Widget action class
 *
 * Base class for custom widget actions
 * 
 * @author Thomas Kroes
 */
class WidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    WidgetAction(QObject* parent);

    /**
     * Create standard widget
     * @param parent Parent widget
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent) override;

    /**
     * Create collapsed widget
     * @param parent Parent widget
     * @return Pointer to collapsed widget
     */
    QWidget* createCollapsedWidget(QWidget* parent);

    /**
     * Create label widget
     * @param parent Parent widget
     * @return Pointer to label widget
     */
    WidgetActionLabel* createLabelWidget(QWidget* parent);

    /**
     * Create reset button
     * @param parent Parent widget
     * @return Pointer to reset button
     */
    WidgetActionResetButton* createResetButton(QWidget* parent);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    virtual QMenu* getContextMenu(QWidget* parent = nullptr) {
        return nullptr;
    };

    /**
     * Create widget for the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags
     */
    QWidget* createWidget(QWidget* parent, const std::int32_t& widgetFlags);

    /** Determines whether a user may reset the action to the default value */
    virtual bool getMayReset() const;

    /** Set whether a user may reset the action to the default value */
    virtual void setMayReset(const bool& mayReset);

    /** Determines whether the action can be reset to its default */
    virtual bool isResettable() const;

    /** Sets the action resettable */
    virtual void setResettable(const bool& resettable);

    /** Reset to default */
    virtual void reset();

    /** Set the widget flags */
    void setDefaultWidgetFlags(const std::int32_t& widgetFlag);

public: // Context

    /**
     * Set the context (used for action grouping)
     * @param context Context
     */
    void setContext(const QString& context);

    /**
     * Set the data hierarchy item context (used for action grouping)
     * @param dataHierarchyItem Data hierarchy item
     */
    void setContext(const DataHierarchyItem* dataHierarchyItem);

    /** Get the context */
    QString getContext() const;

protected:

    /**
     * Get widget representation of the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    virtual QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard);

signals:

    /**
     * Signals that the resettable-ness changed
     * @param isResettable Whether the widget action can be reset
     */
    void resettableChanged(const bool& isResettable);

protected:
    QString                     _createdBy;                     /** Establishes who created the widget action (view, analysis, data etc.) */
    QString                     _context;                       /** The widget action resides outside of the data hierarchy widget (e.g. plugin view) */
    const DataHierarchyItem*    _dataHierarchyItemContext;      /** The widget action resides somewhere in the data hierarchy item */
    std::int32_t                _defaultWidgetFlags;            /** Default widget flags */
    bool                        _resettable;                    /** Whether the action can be reset */
    bool                        _mayReset;                      /** Whether the action may be reset (from the user interface) */
};

/** List of widget actions */
using WidgetActions = QVector<WidgetAction*>;

/**
 * Print widget action to console
 * @param debug Debug
 * @param widgetAction Reference to widget action
 */
inline QDebug operator << (QDebug debug, const WidgetAction& widgetAction)
{
    debug.noquote().nospace() << widgetAction.getContext() << "::" << widgetAction.text();

    return debug.space();
}

/**
 * Print widget action to console
 * @param debug Debug
 * @param widgetAction Pointer to widget action
 */
inline QDebug operator << (QDebug debug, WidgetAction* widgetAction)
{
    debug.noquote().nospace() << widgetAction->getContext() << "::" << widgetAction->text();

    return debug.space();
}

}
}
