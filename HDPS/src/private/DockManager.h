#pragma once

#include "DockArea.h"

#include <util/Serializable.h>

#include <DockManager.h>

#include <QString>

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

    static QMap<ads::DockWidgetArea, QString> dockWidgetAreaStrings;

public:

    /**
     * Constructs a dock manager derived from the advanced docking system
     * @param parent Pointer to parent widget
     */
    DockManager(QWidget* parent = nullptr);

    /**
     * Find the dock area widget where \p widget resides
     * @param widget Pointer to widget to look for
     * @return Pointer to ADS dock widget area (if found, otherwise nullptr)
     */
    ads::CDockAreaWidget* findDockAreaWidget(QWidget* widget);

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map representation of the serializable object
     */
    void fromVariantMap(const QVariantMap& variantMap) override;
    
    /**
     * Save to variant map
     * @return Variant map representation of the serializable object
     */
    QVariantMap toVariantMap() const override;

private:

    /**
     * Load dock areas from variant map
     * @param dockAreasList Dock areas variant list
     * @param depth Tree depth
     */
    QVector<DockArea> dockAreasFromVariantMap(const QVariantList& dockAreasList, std::uint32_t depth = 0);

    /**
     * Load dock area from variant map
     * @param dockAreaMap Dock area variant map
     * @param depth Tree depth
     */
    DockArea dockAreaFromVariantMap(const QVariantMap& dockAreaMap, std::uint32_t depth = 0);

    /**
     * Load dock widgets from variant list
     * @param dockWidgetsList Dock widgets variant list
     * @return Dock widgets
     */
    DockWidgets dockWidgetsFromVariantList(const QVariantList& dockWidgetsList);

    /**
     * Save widget to variant map
     * @param widget Pointer to widget
     * @return Variant map representation of the widget
     */
    QVariantMap widgetToVariantMap(QWidget* widget) const;

    /** Resets the docking layout to defaults */
    void reset();
};
