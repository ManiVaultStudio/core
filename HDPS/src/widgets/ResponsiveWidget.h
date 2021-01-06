#pragma once

#include <QObject>

class QWidget;

namespace hdps
{

namespace gui
{

/**
 * Responsive class
 *
 * Widget class that is configured as a popup widget. The popup widget 
 * has a border and a group box. The actual target widget is inside the group box.
 *
 * @author Thomas Kroes
 */
class ResponsiveWidget : public QObject
{
public:
    ResponsiveWidget(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget, const std::uint32_t& visibleFromWidth = 0);

public: // Getters/setters

    /** Get/set visible from width threshold */
    std::uint32_t getVisibleFromWidth() const;
    void setVisibleFromWidth(const std::uint32_t& visibleFromWidth);
    
    /** Gets/sets the source widget */
    QWidget* getSourceWidget();
    void setSourceWidget(QWidget* sourceWidget);

    /** Gets/sets the target widget */
    QWidget* getTargetWidget();
    void setTargetWidget(QWidget* targetWidget);

public: // Listen to events from the target widget

    /**
     * Listens to the events of \p target
     * @param target Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* target, QEvent* event) override;;

private:

    /**
     * Invoked when the source widget width changes
     * @param 
     */
    void sourceWidthChanged(const std::uint32_t& sourceWidth);

private:
    QWidget*            _sourceWidget;          /** The width of the source widget governs when the target widget is visible */
    QWidget*            _targetWidget;          /** The target widget for which to control the visibility */
    std::uint32_t       _visibleFromWidth;      /** The target widget will be visible when the width of the source widget exceeds this width */
};

}
}