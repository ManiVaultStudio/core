#pragma once

#include <QWidget>
#include <QLabel>

class QMimeData;
class QGraphicsOpacityEffect;
class QPropertyAnimation;

namespace hdps
{

namespace gui
{

/**
 * Advanced drop widget class
 *
 * This class adds advanced drag and drop support to arbitrary (parent) widget without having to change it
 * Depending on the data being dragged, an overlay is presented with one or more drop regions
 * Each region has its own visual representation and drop behaviour (with custom lambda function)
 *
 * Note: Add the following line to the parent widget to enabled drag and drop: setAcceptDrops(true);
 *
 * @author Thomas Kroes
 */
class DropWidget : public QWidget
{
public:
    /**
     * Drop region class
     *
     * This class represents a drop region (a region where mime data can be dropped)
     * It has a visual representation \p widget and a callback function \p dropped when dropping takes place
     *
     * @author Thomas Kroes
     */
    class DropRegion : public QObject
    {
    public:
        /**
         * Standard widget class
         * 
         * Drop region widget class for standardized visual representation
         *
         * @author Thomas Kroes
         */
        class StandardWidget : public QWidget
        {
        public:
            /**
             * Constructor
             * @param parent Parent widget
             * @param title Title of the drop region
             * @param description Description of the drop region
             * @param dropAllowed Whether dropping is allowed in the region
             */
            StandardWidget(QWidget* parent, const QString& title, const QString& description, const bool& dropAllowed = true);
        };

    public:
        /** Callback function when mime data is dropped */
        using Dropped = std::function<void(void)>;

    public:
        /**
         * Constructs a drop region object with a custom widget
         * @param parent Parent widget to enable drag and drop behaviour for
         * @param widget Widget for the visual representation of the region
         * @param dropped Callback function when mime data is dropped
         */
        DropRegion(QObject* parent, QWidget* widget, const Dropped& dropped = Dropped());

        /**
         * Constructs a drop region object with a standard widget
         * @param parent Parent widget to enable drag and drop behaviour for
         * @param title Title of the drop region
         * @param description Description of the drop region
         * @param dropAllowed Whether dropping is allowed in the region
         * @param dropped Callback function when mime data is dropped
         */
        DropRegion(QObject* parent, const QString& title, const QString& description, const bool& dropAllowed = true, const Dropped& dropped = Dropped());

        /** Get visual representation of the region */
        QWidget* getWidget() const;

        /** Initiate the dropping process */
        void drop();

    protected:
        QWidget*        _widget;        /** visual representation of the region */
        const Dropped   _dropped;       /** Callback function when mime data is dropped */
    };

    /** Alias for dropping regions list */
    using DropRegions = QList<DropRegion*>;

    /** Function for obtaining a list of available drop regions */
    using GetDropRegionsFunction = std::function<DropRegions(const QMimeData*)>;

public:
    /**
     * Drop region container widget class
     *
     * This class is the widget representation that is added to the layout of the drop widget
     *
     * @author Thomas Kroes
     */
    class DropRegionContainerWidget : public QWidget
    {
    public:
        /**
         * Constructor
         * @param dropRegion Drop region
         * @param parent Parent widget
         */
        DropRegionContainerWidget(DropRegion* dropRegion, QWidget* parent);

        /** Get the drop region */
        DropRegion* getDropRegion();

    protected:
        /**
         * Sets the highlight state of the widget
         * @param highlight Whether to highlight the widget or not
         */
        void setHighLight(const bool& highlight = false);

    protected:
        DropRegion*                 _dropRegion;        /** Drop region */
        QGraphicsOpacityEffect*     _opacityEffect;     /** Effect for modulating opacity */
        QPropertyAnimation*         _opacityAnimation;  /** Animation of the opacity effect */

        friend class DropWidget;
    };

public:
    /**
     * Constructor
     * @param parent Parent widget
     */
    DropWidget(QWidget* parent);

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Initialize the drop widget
     * @param getDropRegions Function that (based on mime data) returns a list of drop regions
     */
    void initialize(const GetDropRegionsFunction& getDropRegions);

private:
    /** Remove all (temporary) drop regions from the layout */
    void removeAllDropRegionWidgets();

protected:
    GetDropRegionsFunction  _getDropRegionsFunction;        /** Function that (based on mime data) returns a list of drop regions */
};

}
}