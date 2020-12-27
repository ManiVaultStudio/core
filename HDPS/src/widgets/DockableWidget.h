#pragma once

#include <QDockWidget>

class QWidget;

namespace hdps
{

namespace gui
{

class DockableWidget : public QWidget
{
    Q_OBJECT

public: // Enumerations

    /** This enumeration determines where the dock widget will be placed when added to the GUI */
    enum class DockingLocation {
        Left,       /** Position at the left */
        Right,      /** Position at the right */
        Top,        /** Position at the top */
        Bottom,     /** Position at the bottom */
        Center      /** Position at the center (as a tab) */
    };

public:

    /**
     * Constructor
     * @param location Preferred docking location
     * @param parent Parent widget
     */
    explicit DockableWidget(const DockingLocation& location = DockingLocation::Left, QWidget* parent = nullptr);

    /** Destructor */
    ~DockableWidget() override;

public: // Docking location
    
    /** Get preferred docking location */
    DockingLocation getDockingLocation() const;
    void setDockingLocation(const DockingLocation& location);

private:
    DockingLocation     _dockingLocation;     /** Preferred docking location */
};

}
}