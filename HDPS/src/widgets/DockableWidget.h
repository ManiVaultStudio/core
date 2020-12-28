#pragma once

#include <QWidget>
#include <QIcon>

namespace hdps
{

namespace gui
{

class DockableWidget : public QWidget
{
    Q_OBJECT

public: // Enumerations

    /** Preferred docking location */
    enum class DockingLocation {
        Left,       /** Position at the left */
        Right,      /** Position at the right */
        Top,        /** Position at the top */
        Bottom,     /** Position at the bottom */
        Center      /** Position at the center (as a tab) */
    };

public: // Construction/destruction

    /**
     * Constructor
     * @param location Preferred docking location
     * @param parent Parent widget
     */
    explicit DockableWidget(const DockingLocation& location = DockingLocation::Left, QWidget* parent = nullptr);

    /** Destructor */
    ~DockableWidget() override;

public: // Dynamic property wrapper functions

    /**
     * Set title
     * @param title Title
     */
    void setTitle(const QString& title);

    /** Returns the title */
    QString getTitle() const;

    /**
     * Set subtitle
     * @param subtitle Subtitle
     */
    void setSubtitle(const QString& subtitle);

    /** Returns the subtitle */
    QString getSubtitle() const;

    /**
     * Set icon
     * @param icon Icon
     */
    void setIcon(const QIcon& icon);

    /** Returns the icon */
    QIcon getIcon() const;

public: // Docking location
    
    /** Get preferred docking location */
    DockingLocation getDockingLocation() const;
    void setDockingLocation(const DockingLocation& location);

private:
    DockingLocation     _dockingLocation;     /** Preferred docking location */

public:
    static const QString TITLE_PROPERTY_NAME;       /** Title property name */
    static const QString SUBTITLE_PROPERTY_NAME;    /** Subtitle property name */
    static const QString ICON_PROPERTY_NAME;        /** Icon property name */
};

}
}