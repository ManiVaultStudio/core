#pragma once

#include "DockWidget.h"

#include <ViewPlugin.h>

/**
 * Visualization widget class
 *
 * Widget class which contains a docking manager for docking view plugins (non-standard)
 *
 * @author Thomas Kroes
 */
class ViewPluginDockWidget : public DockWidget
{
public:

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param viewPlugin Pointer to view plugin
     * @param parent Pointer to parent widget
     */
    ViewPluginDockWidget(const QString& title, hdps::plugin::ViewPlugin* viewPlugin, QWidget* parent = nullptr);

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /**
     * Get the view plugin
     * @return Pointer to view plugin (might be nullptr)
     */
    hdps::plugin::ViewPlugin* getViewPlugin();

private:
    hdps::plugin::ViewPlugin*     _viewPlugin;    /** Pointer to view plugin */
};

using ViewPluginDockWidgets = QVector<ViewPluginDockWidget*>;
