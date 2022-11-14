#pragma once

#include "DockWidget.h"

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
     * @param parent Pointer to parent widget
     */
    ViewPluginDockWidget(const QString& title, QWidget* parent = nullptr);

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
};

using ViewPluginDockWidgets = QVector<ViewPluginDockWidget*>;
