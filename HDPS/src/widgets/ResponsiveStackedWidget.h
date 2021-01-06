#pragma once

#include "PopupPushButton.h"
#include "PopupWidget.h"

#include <QStackedWidget>
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
template<typename WidgetType>
class ResponsiveStackedWidget : public QStackedWidget
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    ResponsiveStackedWidget(QWidget* parent) :
        QStackedWidget(parent),
        _sourceWidget(nullptr),
        _popupPushButton(new PopupPushButton(this)),
        _widget(new WidgetType(this)),
        _compactThreshold(500)
    {
        addWidget(_popupPushButton);
        addWidget(_widget);

        _popupPushButton->setPopupWidget(new PopupWidget<WidgetType>(this));
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

        sourceWidgetWidthChanged();

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

        sourceWidgetWidthChanged();
    }

    /** Get/set main widget */
    QWidget* getMainWidget() {
        return _widget;
    }

    /** Set main widget
    void setMainWidget(QWidget* mainWidget) {
        Q_ASSERT(mainWidget != nullptr);

        if (mainWidget == _mainWidget)
            return;

        _mainWidget = mainWidget;

        _popupPushButton->setPopupWidget(_mainWidget);
    }
    */

    /** Get/set compact threshold */
    std::uint32_t getCompactThreshold() const {
        return _compactThreshold;
    }

    void setCompactThreshold(const std::uint32_t& compactThreshold) {
        if (compactThreshold == _compactThreshold)
            return;

        _compactThreshold = compactThreshold;

        sourceWidgetWidthChanged();
    }

private:

    /** Invoked when the width of the source widget has changed */
    void sourceWidgetWidthChanged() {
        const auto sourceWidgetWidth    = static_cast<std::uint32_t>(_sourceWidget->size().width());
        const auto compact              = sourceWidgetWidth <= _compactThreshold;
        const auto currentPage          = compact ? 0 : 1;

        setCurrentIndex(currentPage);
    }

private:
    QWidget*                    _sourceWidget;          /** Widget that governs the current page */
    PopupPushButton*            _popupPushButton;       /** Popup push button that shows the main window when clicked (in compact mode) */
    WidgetType*                 _widget;            /** Main widget */
    std::uint32_t               _compactThreshold;      /** Below this threshold, the main widget is replaced by a popup push button */
};

}
}