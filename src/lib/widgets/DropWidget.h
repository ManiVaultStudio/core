// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/WidgetFader.h"

#include <QWidget>

class QMimeData;
class QGraphicsOpacityEffect;
class QPropertyAnimation;

namespace mv::gui
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
class CORE_EXPORT DropWidget : public QWidget
{
public:

    /**
     * Drop indicator widget class
     *
     * Convenience (standard) class for a drop indicator (some visual cues that something can be dropped)
     *
     * @author Thomas Kroes
     */
    class CORE_EXPORT DropIndicatorWidget : public QWidget
    {
    public:
        /**
         * Constructor
         * @param parent Parent widget
         * @param title Title
         * @param description Description
         */
        DropIndicatorWidget(QWidget* parent, const QString& title, const QString& description);

    protected:
        QGraphicsOpacityEffect*     _opacityEffect;     /** Effect for modulating opacity */
    };

    /**
     * Drop region class
     *
     * This class represents a drop region (a region where mime data can be dropped)
     * It has a visual representation \p widget and a callback function \p dropped when dropping takes place
     *
     * @author Thomas Kroes
     */
    class CORE_EXPORT DropRegion : public QObject
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
             * @param iconName Name of the icon
             * @param dropAllowed Whether dropping is allowed in the region
             */
            StandardWidget(QWidget* parent, const QString& title, const QString& description, const QString& iconName = "file-import", const bool& dropAllowed = true);
        };

    public:
        /** Callback function when mime data is dropped */
        using Dropped = std::function<void(void)>;

    public:
        /**
         * Constructs a drop region object with a custom widget
         * @param parent Parent widget to enable drag and drop behavior for
         * @param widget Widget for the visual representation of the region
         * @param iconName Name of the icon
         * @param dropped Callback function when mime data is dropped
         */
        DropRegion(QObject* parent, QWidget* widget, const Dropped& dropped = Dropped());

        /**
         * Constructs a drop region object with a standard widget
         * @param parent Parent widget to enable drag and drop behavior for
         * @param title Title of the drop region
         * @param description Description of the drop region
         * @param iconName Name of the icon
         * @param dropAllowed Whether dropping is allowed in the region
         * @param dropped Callback function when mime data is dropped
         */
        DropRegion(QObject* parent, const QString& title, const QString& description, const QString& iconName = "file-import", const bool& dropAllowed = true, const Dropped& dropped = Dropped());

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
    class CORE_EXPORT DropRegionContainerWidget : public QWidget
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
        DropRegion*         _dropRegion;        /** Drop region */
        util::WidgetFader   _widgetFader;       /** Fade widget */

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

    /** Get/set whether to show the drop indicator widget */
    bool getShowDropIndicator() const;
    void setShowDropIndicator(const bool& showDropIndicator);

    /**
     * Sets the drop indicator widget
     * @param dropIndicatorWidget Drop indicator widget
     */
    void setDropIndicatorWidget(QWidget* dropIndicatorWidget);

private:

    /** Removes all (temporary) drop regions from the layout */
    void resetLayout();

protected:
    GetDropRegionsFunction  _getDropRegionsFunction;        /** Function that (based on mime data) returns a list of drop regions */
    bool                    _showDropIndicator;             /** Whether to show a drop indicator widget overlay */
    QWidget*                _dropIndicatorWidget;           /** Widget that makes users aware that items can be dropped */
};

}
