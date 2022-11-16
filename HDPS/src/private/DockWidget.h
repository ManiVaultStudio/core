#pragma once

#include <util/Serializable.h>
#include <widgets/OverlayWidget.h>

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

    /**
     * Get overlay widget (for showing docking information)
     * @return Reference to overlay widget
     */
    hdps::gui::OverlayWidget& getOverlayWidget();

    /**
     * Override ADS dock widget
     * @param widget Pointer to widget
     * @param insertMode Insert modus
     */
    void setWidget(QWidget* widget, eInsertMode insertMode = AutoScrollArea);

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
    hdps::gui::OverlayWidget    _overlayWidget;     /** Overlay widget for showing loading information */
};

using DockWidgets = QVector<DockWidget*>;