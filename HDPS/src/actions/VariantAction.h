#pragma once

#include "WidgetAction.h"

class QWidget;

namespace hdps::gui {

/**
 * Variant widget action class
 *
 * Stores a Qt variant
 *
 * @author Thomas Kroes
 */
class VariantAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param variant Initial variant value
     */
    VariantAction(QObject* parent, const QString& title = "", const QVariant& variant = QVariant());

    /**
     * Initialize the variant action
     * @param variant Initial variant value
     */
    void initialize(const QVariant& variant = QVariant());

    /**
     * Get variant value
     * @return Variant value
     */
    QVariant getVariant() const;

    /**
     * Set the variant value
     * @param variant Variant value
     */
    void setVariant(const QVariant& variant);

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

protected:  // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    WidgetAction* getPublicCopy() const override;

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

signals:

    /**
     * Signals that the current variant changed
     * @param variant Updated variant value
     */
    void variantChanged(const QVariant& variant);

protected:
    QVariant    _variant;       /** Current variant value */
};

}
