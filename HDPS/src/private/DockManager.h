#pragma once

#include "DockWidget.h"

#include <util/Serializable.h>

#include <DockManager.h>

#include <QString>
#include <QVector>

/**
 * Dock manager class
 *
 * Class for managing docking parameters
 *
 * @author Thomas Kroes
 */
class DockManager : public ads::CDockManager, public hdps::util::Serializable
{
    Q_OBJECT

public:

    class DockArea;

    using DockAreas = QVector<DockArea>;

    class DockArea
    {
    public:
        DockArea(std::uint32_t depth = 0) :
            _parent(nullptr),
            _depth(depth),
            _orientation(),
            _children(),
            _dockWidgets()
        {
        }

        DockArea* getParent() {
            return _parent;
        }

        void setParent(DockArea* parent) {
            _parent = parent;
        }

        std::uint32_t getDepth() const {
            return _depth;
        }

        void setDepth(std::uint32_t depth) {
            _depth = depth;
        }

        Qt::Orientation getOrientation() const {
            return _orientation;
        }

        void setOrientation(Qt::Orientation orientation) {
            _orientation = orientation;
        }

        DockAreas getChildren() const {
            return _children;
        }

        void setChildren(DockAreas children) {
            _children = children;

            for (auto& child : _children) {
                child.setParent(this);
                child.setDepth(_depth + 1);
            }
        }

        DockWidgets getDockWidgets() const {
            return _dockWidgets;
        }

        void setDockWidgets(DockWidgets dockWidgets) {
            _dockWidgets = dockWidgets;
        }

    private:
        DockArea*           _parent;
        std::uint32_t       _depth;
        Qt::Orientation     _orientation;
        DockAreas           _children;
        DockWidgets         _dockWidgets;
    };

public:

    static QMap<ads::DockWidgetArea, QString> dockWidgetAreaStrings;

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
    ads::CDockAreaWidget*      _lastDockAreaWidget;     /** Pointer to last dock area widget */
    ads::DockWidgetArea         _dockWidgetArea;
    ads::DockWidgetArea         _laggingDockWidgetArea;
    DockArea                    _rootDockArea;
};

/**
 * Print dock area (and its children) to the console (indented)
 * @param debug Debug
 * @param dockArea Reference to dock area to print
 */
inline QDebug operator << (QDebug debug, const DockManager::DockArea& dockArea)
{
    QString indent;

    for (int i = 0; i < dockArea.getDepth() * 4; i++)
        indent += " ";

    qDebug().noquote() << indent << "Dock area" << QString("[depth=%1, orientation=%2]").arg(QString::number(dockArea.getDepth()), QString::number(dockArea.getOrientation()));

    for (auto dockWidget : dockArea.getDockWidgets())
        qDebug().noquote() << indent << dockWidget->windowTitle();;

    for (auto child : dockArea.getChildren())
        qDebug().noquote() << child;

    return debug.space();
}