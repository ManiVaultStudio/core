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
    explicit WidgetAction(QObject* parent);

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
     * Set drop target
     * @param isDropTarget whether the widget action is a drop target
     */
    void setDropTarget(const bool& isDropTarget);

    /** Gets whether the widget action is a drop target */
    bool isDropTarget() const;

    /** Determines whether the current color can be reset to its default */
    virtual bool canReset() const;

    /** Reset the current color to the default color */
    virtual void reset();

    /** Gets the widget flags */
    bool hasWidgetFlag(const std::int32_t& widgetFlag) const;

    /** Set the widget flags */
    void setWidgetFlags(const std::int32_t& widgetFlag);

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
    virtual QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard);

signals:

    /**
     * Signals that the resettable-ness changed
     * @param canReset Whether the widget action can be reset
     */
    void canResetChanged(const bool& canReset);

    /**
     * Signals that the widget action drop target status changed
     * @param isDropTarget Whether the widget action is a drop target
     */
    void isDropTargetChanged(const bool& isDropTarget);

protected:
    QString                     _createdBy;                     /** Establishes who created the widget action (view, analysis, data etc.) */
    QString                     _context;                       /** The widget action resides outside of the data hierarchy widget (e.g. plugin view) */
    const DataHierarchyItem*    _dataHierarchyItemContext;      /** The widget action resides somewhere in the data hierarchy item */
    bool                        _isDropTarget;                  /** Whether the widget action is eligible for dropping by another widget action */
    std::int32_t                _widgetFlags;                   /** Widget flags */
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
