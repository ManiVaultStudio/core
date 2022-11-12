#pragma once

#include <util/Serializable.h>

#include <DockManager.h>

#include <QString>

/**
 * Archiver class
 *
 * Class for archiving files and directories (wraps QuaZip)
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

    /**
     * Initializes the dock manager
     */
    void initialize();

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
     * Resets the docking layout to defaults
     */
    void reset();

    /**
     * Load dock areas from variant map
     * @param variantMap Variant map representation of the dock areas
     */
    void loadDockAreasFromVariantMap(const QVariantMap& variantMap) const;

    /**
     * Load dock area from variant map
     * @param variantMap Variant map representation of the dock area
     */
    void loadDockAreaFromVariantMap(const QVariantMap& variantMap) const;

    /**
     * Save to variant map
     * @param widget Pointer to widget
     * @return Variant map representation of the widget
     */
    QVariantMap toVariantMap(QWidget* widget) const;
};
