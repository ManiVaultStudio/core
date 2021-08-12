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

protected:
    QString     _createdBy;         /** Establishes who created the widget action (view, analysis, data etc.) */
    QString     _context;           /** Context in which the widget action resides (for instance in a dataset) */
};

/** List of widget actions */
using WidgetActions = QVector<WidgetAction*>;

}
}
