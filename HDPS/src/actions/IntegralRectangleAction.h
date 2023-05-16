#pragma once

#include "RectangleAction.h"
#include "IntegralRangeAction.h"

namespace hdps::gui {

/**
 * Integral rectangle action class
 *
 * Stores an integral rectangle and creates interfaces to interact with it
 *
 * @author Thomas Kroes
 */
class IntegralRectangleAction : public RectangleAction<QRect, IntegralRangeAction>
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param rectangle Rectangle
     */
    Q_INVOKABLE IntegralRectangleAction(QObject* parent, const QString& title, const QRect& rectangle = QRect());

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the rectangle changed
     * @param rectangle Rectangle
     */
    void rectangleChanged(const QRectF& rectangle);
};

}

Q_DECLARE_METATYPE(hdps::gui::IntegralRectangleAction)

inline const auto integralRectangleActionMetaTypeId = qRegisterMetaType<hdps::gui::IntegralRectangleAction*>("hdps::gui::IntegralRectangleAction");
