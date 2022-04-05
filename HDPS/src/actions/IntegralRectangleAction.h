#pragma once

#include "WidgetAction.h"

#include <QLineEdit>

class QWidget;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Integral rectangle action class
 *
 * Stores an integral rectangle and creates interface for it
 *
 * @author Thomas Kroes
 */
class IntegralRectangleAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget settings */
    enum WidgetFlag {
        LineEdit = 0x00001,      /** Widget includes a line edit */

        Default = LineEdit
    };

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
    IntegralRectangleAction(QObject * parent, const QString& title, const QRect& rectangle = QRect(), const QRect& defaultRectangle = QRect());

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Initialize the decimal action
     * @param rectangle Rectangle
     * @param defaultRectangle Default rectangle
     */
    void initialize(const QRect& rectangle = QRect(), const QRect& defaultRectangle = QRect());

    /**
     * Get rectangle
     * @return Rectangle
     */
    QRect getRectangle() const;

    /**
     * Set rectangle
     * @param rectangle Rectangle
     */
    void setRectangle(const QRect& rectangle);

    /**
     * Get default rectangle
     * @return Default rectangle
     */
    QRect getDefaultRectangle() const;

    /**
     * Set default rectangle
     * @param defaultRectangle Default rectangle
     */
    void setDefaultRectangle(const QRect& defaultRectangle);

public: // Linking

    /**
     * Get whether the action may be published or not
     * @return Boolean indicating whether the action may be published or not
     */
    bool mayPublish() const override;

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
    virtual WidgetAction* getPublicCopy() const;

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

protected:
    QRect       _rectangle;             /** Rectangle */
    QRect       _defaultRectangle;      /** Default rectangle */
};

}
}
