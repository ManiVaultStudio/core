// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

class QWidget;

namespace mv::gui {

/**
 * Variant widget action class
 *
 * Stores a Qt variant
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT VariantAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param variant Initial variant value
     */
    Q_INVOKABLE VariantAction(QObject* parent, const QString& title, const QVariant& variant = QVariant());

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
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const mv::VariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    mv::VariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the current variant changed
     * @param variant Updated variant value
     */
    void variantChanged(const QVariant& variant);

protected:
    QVariant    _variant;       /** Current variant value */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::VariantAction)

inline const auto variantActionMetaTypeId = qRegisterMetaType<mv::gui::VariantAction*>("mv::gui::VariantAction");
