#pragma once

#include "DockWidget.h"

#include <util/Serializable.h>

#include <DockManager.h>

#include <QString>
#include <QVector>
#include <QUuid>

/**
 * Dock manager class
 *
 * ADS inherited dock manager class, primary purpose it to support layout serialization
 *
 * @author Thomas Kroes
 */
class DockManager : public ads::CDockManager, public hdps::util::Serializable
{
    Q_OBJECT

public:

    class DockArea;

    using DockAreas = QVector<DockArea>;

    /**
     * Dock area class
     *
     * During de-serialization of the layout, a hierarchy of dock areas is created.
     * This hierarchy is the used to re-create the layout in the project. 
     *
     * @author Thomas Kroes
     */
    class DockArea
    {
    public:

        /**
         * Constructor
         */
        DockArea(DockManager* dockManager, std::uint32_t depth = 0);

        /** Copy constructor */
        DockArea(const DockArea& other);

        QString getId() const;

        DockArea* getParent();

        void setParent(DockArea* parent);

        std::uint32_t getDepth() const;

        void setDepth(std::uint32_t depth);

        Qt::Orientation getOrientation() const;

        void setOrientation(Qt::Orientation orientation);

        DockAreas getChildren() const;

        void setChildren(DockAreas children);

        std::uint32_t getChildIndex(const DockArea& child) const;

        DockWidgets getDockWidgets() const;

        void setDockWidgets(DockWidgets dockWidgets);

        ads::CDockAreaWidget* getCurrentDockAreaWidget();
        void setCurrentDockAreaWidget(ads::CDockAreaWidget* lastDockAreaWidget);

        bool hasLastDockAreaWidget() const;

        void createPlaceHolders(std::uint32_t depth);
        void removePlaceHolders();
        void applyDocking();
        void sanitizeHierarchy();
        DockWidget* getFirstDockWidget();

    private:
        std::uint32_t getMaxDepth() const;

    public:

        /**
         * Assignment operator
         * @param other Reference to assign from
         */
        DockArea& operator=(const DockArea& other) {
            _dockManager            = other._dockManager;
            _parent                 = other._parent;
            _depth                  = other._depth;
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
        DockAreas               _children;
        DockWidget*             _placeholderDockWidget;
        DockWidgets             _dockWidgets;
        ads::CDockAreaWidget*   _currentDockAreaWidget;     /** Pointer to last dock area widget */
    };

public:

    static QMap<ads::DockWidgetArea, QString> dockWidgetAreaStrings;
    static QMap<Qt::Orientation, QString> orientationStrings;

public:

    /**
     * Constructs a dock manager derived from the advanced docking system
     * @param parent Pointer to parent widget
     */
    DockManager(QWidget* parent = nullptr);

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map representation of the serializable object
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    DockWidgets dockWidgetsFromVariantList(const QVariantList& dockWidgetsList);
    QVector<DockArea> areasFromVariantMap(const QVariantList& areasList, std::uint32_t depth = 0);
    /**
     * Save to variant map
     * @return Variant map representation of the serializable object
     */
    QVariantMap toVariantMap() const override;

private:

    /**
     * Resets the docking layout to defaults
     */
    void reset();

    /**
     * Load widget from variant map
     * @param widgetMap Widget variant map
     * @param parentHasSplitter Whether the 
     */
    DockArea areaFromVariantMap(const QVariantMap& areaMap, std::uint32_t depth = 0);

    /**
     * Save widget to variant map
     * @param widget Pointer to widget
     * @return Variant map representation of the widget
     */
    QVariantMap widgetToVariantMap(QWidget* widget) const;

private:
};

/**
 * Compares two dock areas for equality
 * @param lhs Left-hand-side dock area
 * @param rhs Right-hand-side dock area
 * @return Whether lhs and rhs are equal
 */
inline bool operator == (const DockManager::DockArea& lhs, const DockManager::DockArea& rhs)
{
    return lhs.getId() == rhs.getId();
}

/**
 * Print dock area (and its children) to the console (for debugging purposes)
 * @param debug Debug to print to
 * @param dockArea Reference to dock area to print
 */
inline QDebug operator << (QDebug debug, const DockManager::DockArea& dockArea)
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
        addProperty("orientation", DockManager::orientationStrings.value(dockArea.getOrientation()));

    outputDebug << getIndentation(dockArea.getDepth()) << "Dock area " << QString("(%1)").arg(propertiesString.join(", ")) << "\n";

    if (!dockArea.getDockWidgets().isEmpty()) {
        auto dockWidgetString = QStringList();

        for (auto dockWidget : dockArea.getDockWidgets())
            dockWidgetString << dockWidget->windowTitle();

        outputDebug << getIndentation(dockArea.getDepth() + 1) << QString("Dock widgets: [%1]").arg(dockWidgetString.join(", ")) << "\n";
    }

    for (auto child : dockArea.getChildren())
        outputDebug << child;

    return outputDebug;
}