#pragma once

#include "WidgetActionWidget.h"
#include "WidgetActionLabel.h"

#include <QWidgetAction>
#include <QMenu>

class QLabel;

namespace hdps {

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

    QWidget* createWidget(QWidget* parent) override;

    QWidget* createCollapsedWidget(QWidget* parent);

    WidgetActionLabel* createLabelWidget(QWidget* parent);

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

public:

    /**
     * Set the context (used for linking actions)
     * @param context Context
     */
    void setContext(const QString& context);

    /** Get the context */
    QString getContext() const;

protected:
    virtual QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard);

signals:

    /**
     * Signals that the widget action drop target status changed
     * @param isDropTarget Whether the widget action is a drop target
     */
    void isDropTargetChanged(const bool& isDropTarget);

protected:
    QString     _createdBy;         /** Establishes who created the widget action (view, analysis, data etc.) */
    QString     _context;           /** Context in which the widget action resides (for instance in a dataset) */
    bool        _isDropTarget;      /** Whether the widget action is eligible for dropping by another widget action */
};

/** List of widget actions */
using WidgetActions = QVector<WidgetAction*>;

}
}
