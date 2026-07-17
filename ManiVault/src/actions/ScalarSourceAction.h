#pragma once

#include "DecimalRangeAction.h"
#include "GroupAction.h"
#include "ScalarSourceModel.h"

#include <PointData/DimensionPickerAction.h>

namespace mv::gui {

/**
 * Scalar source action class
 *
 * Action class picking a scalar source (scalar by constant or dataset dimension)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ScalarSourceAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE ScalarSourceAction(QObject* parent, const QString& title);

    /** Get the scalar source model */
    ScalarSourceModel& getModel();

    /** Update scalar range */
    void updateScalarRange();

protected: // Linking

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
     * @param variantMap Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    OptionAction& getPickerAction() { return _pickerAction; }
    DimensionPickerAction& getDimensionPickerAction() { return _dimensionPickerAction; }
    DecimalAction& getOffsetAction() { return _offsetAction; }
    DecimalRangeAction& getRangeAction() { return _rangeAction; }

    const OptionAction& getPickerAction() const  { return _pickerAction; }

signals:

    /**
     * Signals that the scalar range changed
     * @param minimum Scalar range minimum
     * @param maximum Scalar range maximum
     */
    void scalarRangeChanged(const float& minimum, const float& maximum);

private:
    ScalarSourceModel       _model;                     /** Scalar model */
    OptionAction            _pickerAction;              /** Source picker action */
    DimensionPickerAction   _dimensionPickerAction;     /** Dimension picker action */
    DecimalAction           _offsetAction;              /** Scalar source offset action */
    DecimalRangeAction      _rangeAction;               /** Range action */

    friend class mv::AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::ScalarSourceAction)

inline const auto scalarSourceActionMetaTypeId = qRegisterMetaType<mv::gui::ScalarSourceAction*>("mv::gui::ScalarSourceAction");
