#pragma once

#include "WidgetAction.h"

namespace hdps {

namespace gui {

/**
 * Rectangle action class
 *
 * Base rectangle class for storing a rectangle and interfacing with it
 *
 * @author Thomas Kroes
 */
template<typename RectangleType>
class RectangleAction : public WidgetAction
{
    /** Templated classes with Q_OBJECT macro are not allowed, so use function pointers in stead */
    using RectangleChangedCB        = std::function<void()>;
    using DefaultRectangleChangedCB = std::function<void()>;

public:

    /** Describes the widget settings */
    enum WidgetFlag {
        LineEdit = 0x00001,      /** Widget includes a line edit */

        Default = LineEdit
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param rectangle Rectangle
     * @param defaultRectangle Default rectangle
     */
    RectangleAction(QObject * parent, const QString& title, const RectangleType& rectangle = RectangleType(), const RectangleType& defaultRectangle = RectangleType()) :
        WidgetAction(parent, title)
    {
        setText(title);
        setDefaultWidgetFlags(WidgetFlag::Default);
    }

    /**
     * Get rectangle
     * @return Rectangle
     */
    RectangleType getRectangle() const
    {
        return _rectangle;
    }

    /**
     * Set rectangle
     * @param rectangle Rectangle
     */
    void setRectangle(const RectangleType& rectangle)
    {
        if (rectangle == _rectangle)
            return;

        _rectangle = rectangle;

        _rectangleChanged();
    }

    /**
     * Get default rectangle
     * @return Default rectangle
     */
    RectangleType getDefaultRectangle() const
    {
        return _defaultRectangle;
    }

    /**
     * Set default rectangle
     * @param defaultRectangle Default rectangle
     */
    void setDefaultRectangle(const RectangleType& defaultRectangle)
    {
        if (defaultRectangle == _defaultRectangle)
            return;

        _defaultRectangle = defaultRectangle;

        _defaultRectangleChanged();
    }

protected:
    RectangleType   _rectangle;             /** Rectangle */
    RectangleType   _defaultRectangle;      /** Default rectangle */

protected: // Callbacks for implementations of the numerical action
    RectangleChangedCB          _rectangleChanged;          /** Callback which is called when the rectangle changed */
    DefaultRectangleChangedCB   _defaultRectangleChanged;   /** Callback which is called when the default rectangle changed */
};

}
}
