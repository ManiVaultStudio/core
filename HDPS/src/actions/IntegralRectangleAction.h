#pragma once

#include "RectangleAction.h"

#include <QLineEdit>

class QWidget;

namespace hdps::gui {

/**
 * Integral rectangle action class
 *
 * Stores an integral rectangle and creates interfaces to interact with it
 *
 * @author Thomas Kroes
 */
class IntegralRectangleAction : public RectangleAction<QRect>
{
    Q_OBJECT

public:

    /** Line edit widget class for integral rectangle action */
    class LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralRectangleAction Pointer to integral rectangle action
         */
        LineEditWidget(QWidget* parent, IntegralRectangleAction* integralRectangleAction);

        friend class IntegralRectangleAction;
    };

protected:

    /**
     * Get widget representation of the integral rectangle action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param rectangle Rectangle
     * @param defaultRectangle Default rectangle
     */
    Q_INVOKABLE IntegralRectangleAction(QObject * parent, const QString& title, const QRect& rectangle = QRect(), const QRect& defaultRectangle = QRect());

    /**
     * Initialize the action
     * @param rectangle Rectangle
     * @param defaultRectangle Default rectangle
     */
    void initialize(const QRect& rectangle = QRect(), const QRect& defaultRectangle = QRect());

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
    void rectangleChanged(const QRect& rectangle);

    /**
     * Signals that the default rectangle changed
     * @param defaultRectangle Default rectangle
     */
    void defaultRectangleChanged(const QRect& defaultRectangle);
};

}

Q_DECLARE_METATYPE(hdps::gui::IntegralRectangleAction)

inline const auto integralRectangleActionMetaTypeId = qRegisterMetaType<hdps::gui::IntegralRectangleAction*>("IntegralRectangleAction");
