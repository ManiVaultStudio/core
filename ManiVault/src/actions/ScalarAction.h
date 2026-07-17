#pragma once

#include "DatasetPickerAction.h"
#include "GroupAction.h"
#include "ScalarSourceAction.h"

namespace mv::gui {

/**
 * Scalar action class
 *
 * Action class picking scalar options (scalar by constant or dataset dimension)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ScalarAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title
     * @param minimum Scalar minimum value
     * @param maximum Scalar maximum value
     * @param value Scalar value
     */
    Q_INVOKABLE ScalarAction(QObject* parent, const QString& title, const float& minimum = 0.0f, const float& maximum = 100.0f, const float& value = 0.0f);

    /**
     * Add dataset to the model
     * @param dataset Dataset to add
     */
    void addDataset(const Dataset<DatasetImpl>& dataset);

    /** Removes all datasets */
    void removeAllDatasets();

    /** Get smart pointer to current scalar dataset (if any) */
    Dataset<DatasetImpl> getCurrentDataset();

    /**
     * Set the current dataset
     * @param dataset Smart pointer to dataset
     */
    void setCurrentDataset(const Dataset<DatasetImpl>& dataset);

    /**
     * Set the current source index
     * @param sourceIndex Source index
     */
    void setCurrentSourceIndex(std::int32_t sourceIndex);

    /** Determines whether the scalar source is a constant */
    bool isSourceConstant() const;

    /** Determines whether the scalar source is a selection */
    bool isSourceSelection() const;

    /** Determines whether the scalar source is a dataset */
    bool isSourceDataset() const;

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

    DecimalAction& getMagnitudeAction() { return _magnitudeAction; }
    ScalarSourceAction& getSourceAction() { return _sourceAction; }

	const DatasetPickerAction& getSourceDatasetPickerAction() { return _sourceDatasetPickerAction; }

signals:

    /**
     * Signals that the source selection changed
     * @param sourceSelectionIndex Index of the selected source (0 is constant, 1 is selection, above is a dataset)
     */
    void sourceSelectionChanged(const std::uint32_t& sourceSelectionIndex);

    /**
     * Signals that the source data changed (only emitted when a source dataset is selected)
     * @param dataset Source dataset that changed
     */
    void sourceDataChanged(const Dataset<DatasetImpl>& dataset);

    /**
     * Signals that the scalar range changed
     * @param minimum Scalar range minimum
     * @param maximum Scalar range maximum
     */
    void scalarRangeChanged(const float& minimum, const float& maximum);

    /**
     * Signals that the scalar magnitude changed
     * @param magnitude Scalar magnitude
     */
    void magnitudeChanged(const float& magnitude);

    /**
     * Signals that the scalar offset changed
     * @param offset Scalar offset
     */
    void offsetChanged(const float& offset);

private:
    DatasetPickerAction     _sourceDatasetPickerAction;     /** Dataset picker action for source dataset selection */
    DecimalAction           _magnitudeAction;               /** Scalar magnitude action */
    ScalarSourceAction      _sourceAction;                  /** Scalar source action */
    Dataset<>               _currentDataset;                /** Cached current dataset (if any) */

    friend class mv::AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::ScalarAction)

inline const auto scalarActionMetaTypeId = qRegisterMetaType<mv::gui::ScalarAction*>("mv::gui::ScalarAction");
