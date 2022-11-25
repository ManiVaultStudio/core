#pragma once

#include "DockWidget.h"

#include <util/Serializable.h>

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
     * Constructor
     */
    DockArea(DockManager* dockManager, std::uint32_t depth = 0);

public:

    /** Copy constructor */
    DockArea(const DockArea& other);

    QString getId() const;

    bool hasParent() const;

    DockArea* getParent();

    void setParent(DockArea* parent);

    bool isRoot() const;

    std::uint32_t getDepth() const;

    void setDepth(std::uint32_t depth);

    SplitterRatios getSplitterRatios() const;

    void setSplitterRatios(const SplitterRatios& splitterRatios);

    Qt::Orientation getOrientation() const;

    void setOrientation(Qt::Orientation orientation);

    DockAreas getChildren() const;

    void setChildren(DockAreas children);

    std::uint32_t getChildIndex(const DockArea& child) const;

    bool hasDockWidgets() const;
    DockWidgets getDockWidgets() const;

    void setDockWidgets(DockWidgets dockWidgets);

    ads::CDockAreaWidget* getCurrentDockAreaWidget();
    void setCurrentDockAreaWidget(ads::CDockAreaWidget* lastDockAreaWidget);

    bool hasLastDockAreaWidget() const;

    void createDockWidgets(std::uint32_t depth);
    void removePlaceHolderDockWidgets();
    void applyDocking();
    void sanitizeHierarchy();

    void loadViewPluginDockWidgets();

    void buildTreeFromDocking(QWidget* widget);

private:
    std::uint32_t getMaxDepth() const;

    void setSplitterWidgetSizes();

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
    SplitterRatios          _splitterRatios;
    DockAreas               _children;
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

    const auto getIndentation = [](std::uint16_t depth) -> QString {
        QString indentation;

        for (int i = 0; i < depth * 3; i++)
            indentation += " ";

        return indentation;
    };

    QStringList propertiesString;

    const auto addProperty = [&](const QString& name, const QString& value) -> void {
        propertiesString << QString("%1=%2").arg(name, value);
    };

    addProperty("depth", QString::number(dockArea.getDepth()));

    if (dockArea.getOrientation() >= 1)
        addProperty("orientation", DockArea::orientationStrings.value(dockArea.getOrientation()));

    QStringList splitterSizesString;

    for (const auto& splitterRatio : dockArea.getSplitterRatios())
        splitterSizesString << QString::number(splitterRatio);

    if (!splitterSizesString.isEmpty())
        addProperty("splitter_sizes", QString("[%1]").arg(splitterSizesString.join(", ")));

    outputDebug << getIndentation(dockArea.getDepth()) << "Dock area " << QString("(%1)").arg(propertiesString.join(", ")) << "\n";

    if (!dockArea.getDockWidgets().isEmpty()) {
        auto dockWidgetString = QStringList();

        for (auto dockWidget : dockArea.getDockWidgets())
            dockWidgetString << dockWidget->windowTitle();

        outputDebug << getIndentation(dockArea.getDepth() + 1) << QString("Dock widgets: [%1]").arg(dockWidgetString.join(", ")) << "\n";
    }

    for (const auto& child : dockArea.getChildren())
        outputDebug << child;

    return outputDebug;
}
