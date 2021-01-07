#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QResizeEvent>

namespace hdps
{

namespace gui
{

/**
 * Responsive stacked widget class
 *
 * @author Thomas Kroes
 */
class ResponsiveWidget : public QWidget
{
public:
    
    /**
     * Callback function which is called when the size of the source widget changes
     * @param size New size of the source widget
     * @param widget Reference to the contained widget
     */
    typedef std::function<void(const QSize& size, WidgetType& widget)> Callback;

public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    ResponsiveWidget(QWidget* parent) :
        QWidget(parent),
        _sourceWidget(nullptr),
        _widget(new WidgetType(this)),
        _callBack()
    {
        auto layout = new QHBoxLayout();

        layout->addWidget(_widget);

        setLayout(layout);
    }

    /**
     * Respond to changes of the source widget
     * @param target Source widget
     * @param event Event which occurred
     */
    bool eventFilter(QObject* target, QEvent* event)
    {
        if (event->type() != QEvent::Resize)
            return QWidget::eventFilter(target, event);

        if (_sourceWidget != nullptr && _callBack)
            _callBack(_sourceWidget->size(), *_widget);

        return QWidget::eventFilter(target, event);
    }

public: // Getters/setters

    /** Get/set source widget */
    QWidget* getSourceWidget() {
        return _sourceWidget;
    }

    void setSourceWidget(QWidget* sourceWidget) {
        Q_ASSERT(sourceWidget != nullptr);

        if (sourceWidget == _sourceWidget)
            return;

        if (_sourceWidget != nullptr)
            _sourceWidget->removeEventFilter(this);

        _sourceWidget = sourceWidget;

        _sourceWidget->installEventFilter(this);

        if (_sourceWidget != nullptr && _callBack)
            _callBack(_sourceWidget->size(), *_widget);
    }

    /** Get widget */
    QWidget* getWidget() {
        return _widget;
    }

    /**
     * Set callback function which is called when the size of the source widget changes
     * @param callback Callback function
     */
    void setCallback(Callback callback) {
        _callBack = callback;
    }

private:
    QWidget*        _sourceWidget;      /** Widget that governs the current page */
    WidgetType*     _widget;            /** Main widget */
    Callback        _callBack;          /** Function which is called when the size of the source widget changes */
};

}
}