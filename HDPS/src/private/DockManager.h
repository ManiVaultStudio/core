#pragma once

#include "DockArea.h"
#include "CentralDockWidget.h"
#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

#include <util/Serializable.h>

#include <DockManager.h>
#include <DockAreaWidget.h>

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

    /**
     * Constructs a dock manager derived from the advanced docking system
     * @param parent Pointer to parent widget
     */
    DockManager(QWidget* parent = nullptr);

    /** Destructor */
    ~DockManager();

    /**
     * Get view plugin dock widgets
     * @return Vector of pointers to plugin dock widgets
     */
    ViewPluginDockWidgets getViewPluginDockWidgets();

    /**
     * Get view plugin dock widgets
     * @return Vector of pointers to plugin dock widgets
     */
    const ViewPluginDockWidgets getViewPluginDockWidgets() const;

    /**
     * Find the dock area widget where \p widget resides
     * @param widget Pointer to widget to look for
     * @return Pointer to ADS dock widget area (if found, otherwise nullptr)
     */
    ads::CDockAreaWidget* findDockAreaWidget(QWidget* widget);

    /**
     * Remove \p viewPlugin from the dock manager
     * @param viewPlugin Pointer to the view plugin to remove
     */
    void removeViewPluginDockWidget(hdps::plugin::ViewPlugin * viewPlugin);

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

    /** Resets the docking layout to defaults */
    void reset();

    friend class ViewPluginsDockWidget;
};

/**
 * Print dock manager to the console (for debugging purposes)
 * @param debug Debug to print to
 * @param dockArea Reference to dock manager to print
 */
inline QDebug operator << (QDebug debug, const DockManager& dockManager)
{
    auto outputDebug = debug.noquote().nospace();

    const auto getIndentation = [](std::uint16_t depth) -> QString {
        QString indentation;

        for (int i = 0; i < depth * 3; i++)
            indentation += " ";

        return indentation;
    };

    const auto addProperty = [&](QStringList& propertiesString, const QString& name, const QString& value) -> void {
        propertiesString << QString("%1=%2").arg(name, value);
    };

    QStringList dockManagerPropertiesString;

    addProperty(dockManagerPropertiesString, "name", dockManager.objectName());

    outputDebug << getIndentation(0) << "Dock manager" << QString("(%1)").arg(dockManagerPropertiesString.join(", ")) << "\n";

    for (auto dockWidget : dockManager.dockWidgetsMap()) {
        QStringList dockWidgetsPropertiesString;

        addProperty(dockWidgetsPropertiesString, "title", dockWidget->windowTitle());
        addProperty(dockWidgetsPropertiesString, "central_widget", dockWidget->dockAreaWidget()->isCentralWidgetArea() ? "true" : "false");

        outputDebug << getIndentation(1) << "Dock widget " << QString("(%1)").arg(dockWidgetsPropertiesString.join(", ")) << "\n";
    }

    return outputDebug;
}
