#pragma once

#include <util/Serializable.h>

#include <DockWidget.h>

/**
 * Dock widget class
 *
 * ADS dock widget class extended with serialization
 *
 * @author Thomas Kroes
 */
class DockWidget : public ads::CDockWidget, public hdps::util::Serializable
{
public:

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param parent Pointer to parent widget
     */
    DockWidget(const QString& title, QWidget* parent = nullptr);

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

using DockWidgets = QVector<DockWidget*>;