#pragma once

#include "DockWidget.h"

#include <util/Serializable.h>
#include <util/Miscellaneous.h>

#include <QString>
#include <QVector>
#include <QUuid>

class DockArea;
class DockManager;

using DockAreas = QVector<DockArea>;

/**
 * Dock area class
 *
 * During de-serialization of the layout, a hierarchy of dock areas is created.
 * This hierarchy is the used to re-create the layout in the project. 
 *
 * @author Thomas Kroes
 */
class DockArea : public hdps::util::Serializable
{
public:
    using SplitterRatios = QVector<float>;

    static QMap<Qt::Orientation, QString> orientationStrings;

protected:

    /**
     * Constructs a dock area using a \p dockManager and \p depth
	 * @param dockManager Pointer to dock manager
	 * @param depth Initial depth of the dock area
     */
    DockArea(DockManager* dockManager, std::uint32_t depth = 0);

public:

    /** Copy constructor */
    DockArea(const DockArea& other);
	
	/**
	 * Get the globally unique identifier of the dock area
	 * @return Globally unique identifier of the dock area in string format
	 */
    QString getId() const;
	
	/**
	 * Determine whether the dock area has a parent or not
	 * @return Boolean determining whether the dock area has a parent or not
	 */
    bool hasParent() const;

	/**
	 * Get the dock area parent
	 * @return Pointer to parent dock area (may be nullptr)
	 */
    DockArea* getParent();
	
	/**
	 * Set the dock area parent
	 * @param parent Pointer to parent dock area
	 */
    void setParent(DockArea* parent);
	
	/**
	 * Determine whether the dock area is a root dock area or not
	 * @return Boolean determining whether the dock area is a root dock area or not
	 */
    bool isRoot() const;
	
	/**
	 * Get the depth of the dock area
	 * @return Depth of the dock area
	 */
    std::uint32_t getDepth() const;
	
	/**
	 * Set the depth of the dock area
	 * @param depth Depth of the dock area
	 */
    void setDepth(std::uint32_t depth);
	
	/**
	 * Get the splitter ratios
	 * @return Splitter ratios
	 */
    SplitterRatios getSplitterRatios() const;
	
	/**
	 * Set the splitter ratios to \p splitterRatios
	 * @param splitterRatios Splitter ratios
	 */
    void setSplitterRatios(const SplitterRatios& splitterRatios);
	
	/**
	 * Get the orientation of the dock area
	 * @return Orientation of the dock area
	 */
    Qt::Orientation getOrientation() const;
	
	/**
	 * Set the orientation of the dock area to \p orientation
	 * @param orientation Orientation of the dock area
	 */
    void setOrientation(Qt::Orientation orientation);
	
	/**
	 * Get the children of the dock area
	 * @return Children of the dock area
	 */
    DockAreas getChildren() const;
	
	/**
	 * Set the children of the dock area to \p children
	 * @param children Children of the dock area
	 */
    void setChildren(DockAreas children);

    /**
     * Get whether the dock area is a central docking area
     * @return Boolean determining whether the dock area is a central docking area
     */
    bool isCentral() const;

    /**
     * Set whether the dock area is a central docking area
     * @param central Boolean determining whether the dock area is a central docking area
     */
    void setCentral(bool central);
	
	/**
	 * Get the child index of \p child
	 * @param child Child dock area
	 * @return Index of the child
	 */
    std::uint32_t getChildIndex(const DockArea& child) const;
	
	/**
	 * Get whether the dock area has any dock widgets
	 * @return Boolean determining whether the dock area has any dock widgets
	 */
    bool hasDockWidgets() const;
	
    DockWidgets getDockWidgets() const;

    void setDockWidgets(DockWidgets dockWidgets);

    ads::CDockAreaWidget* getCurrentDockAreaWidget();
    void setCurrentDockAreaWidget(ads::CDockAreaWidget* lastDockAreaWidget);

    bool hasCurrentDockAreaWidget() const;

    void createDockWidgets(std::uint32_t depth);
    void removePlaceHolderDockWidgets();
    void applyDocking();
    void sanitizeHierarchy();

    void loadViewPluginDockWidgets();

    void buildTreeFromDocking(QWidget* widget);

private:
    std::uint32_t getMaxDepth() const;

    /**
     * Set splitter sizes recursively
     * Visits the widget hierarchy from \p widget downwards and sets the splitter sizes for each encountered splitter.
     * Splitter sizes are established by multiplying splitter ratios (attached to the widget prior as a property) by 
     * either the width or the height of the splitter respectively.
     * @param widget Pointer to root widget to descendant downward from
     */
    static void setSplitterWidgetSizes(QWidget* widget);

public: // Serialization

    /**
     * Load dock area from variant map
     * @param variantMap Variant map representation of the dock area 
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save dock area to variant map
     * @return Variant map representation of the dock area 
     */
    QVariantMap toVariantMap() const override;

public:

    /**
     * Assignment operator
     * @param other Reference to assign from
     */
    DockArea& operator=(const DockArea& other) {
        _id                     = other._id;
        _dockManager            = other._dockManager;
        _parent                 = other._parent;
        _depth                  = other._depth;
        _splitterRatios         = other._splitterRatios;
        _orientation            = other._orientation;
        _children               = other._children;
        _isCentral              = other._isCentral;
        _placeholderDockWidget  = other._placeholderDockWidget;
        _dockWidgets            = other._dockWidgets;
        _currentDockAreaWidget  = other._currentDockAreaWidget;

        return *this;
    }

    /**
     * Equality operator (which compares the identifier of two dock areas)
     * @param other Other dock area to compare with
     */
    bool operator == (const DockArea& other) {
        return other.getId() == _id;
    }

private:
    QString                 _id;                        /** Unique identifier for the dock area (for comparison operatorF) */
    DockManager*            _dockManager;
    DockArea*               _parent;
    std::uint32_t           _depth;
    Qt::Orientation         _orientation;
    SplitterRatios          _splitterRatios;			/** Splitter ratios determine the percentage of space that each child occupies (in percentage in either horizontal or vertical direction */
    DockAreas               _children;					/** Child dock areas */
    bool                    _isCentral;                 /** Whether the docking area is a central docking area or not */
    DockWidget*             _placeholderDockWidget;
    DockWidgets             _dockWidgets;
    ads::CDockAreaWidget*   _currentDockAreaWidget;     /** Pointer to last dock area widget */

    friend class DockManager;
};

/**
 * Compares two dock areas for equality
 * @param lhs Left-hand-side dock area
 * @param rhs Right-hand-side dock area
 * @return Whether lhs and rhs are equal
 */
inline bool operator == (const DockArea& lhs, const DockArea& rhs)
{
    return lhs.getId() == rhs.getId();
}

/**
 * Print dock area (and its children) to the console (for debugging purposes)
 * @param debug Debug to print to
 * @param dockArea Reference to dock area to print
 */
inline QDebug operator << (QDebug debug, const DockArea& dockArea)
{
    auto outputDebug = debug.noquote().nospace();

    QStringList propertiesString;

    const auto addProperty = [&](const QString& name, const QString& value) -> void {
        propertiesString << QString("%1=%2").arg(name, value);
    };

    addProperty("depth", QString::number(dockArea.getDepth()));
    addProperty("central", dockArea.isCentral() ? "true" : "false");

    if (dockArea.getOrientation() >= 1)
        addProperty("orientation", DockArea::orientationStrings.value(dockArea.getOrientation()));

    QStringList splitterSizesString;

    for (const auto& splitterRatio : dockArea.getSplitterRatios())
        splitterSizesString << QString::number(splitterRatio);

    if (!splitterSizesString.isEmpty())
        addProperty("splitter_sizes", QString("[%1]").arg(splitterSizesString.join(", ")));

    outputDebug << hdps::util::getTabIndentedMessage(QString("Dock area (%1)\n").arg(propertiesString.join(", ")), dockArea.getDepth());

    if (!dockArea.getDockWidgets().isEmpty()) {
        auto dockWidgetString = QStringList();

        for (auto dockWidget : dockArea.getDockWidgets())
            dockWidgetString << dockWidget->windowTitle();

        outputDebug << hdps::util::getTabIndentedMessage(QString("Dock widgets: [%1]").arg(dockWidgetString.join(", ")), dockArea.getDepth());
    }

    for (const auto& child : dockArea.getChildren())
        outputDebug << child;

    return outputDebug;
}
