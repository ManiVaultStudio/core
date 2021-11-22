#pragma once

#include <QObject>

namespace hdps
{

class DatasetImpl;

/**
 * Smart dataset pointer signals class
 *
 * Designed to emit signals related to changes in the dataset pointed to by the smart pointer
 *
 * @author T. Kroes
 */
class DatasetSignals : public QObject
{
    Q_OBJECT

protected:

    /** Default constructor */
    DatasetSignals() {}

protected: // Notifiers

    void notifyChanged(DatasetImpl* dataset);
    void notifyDataChanged();
    void notifyDataAboutToBeRemoved();
    void notifyDataRemoved(const QString& datasetId);
    void notifyDataGuiNameChanged(const QString& oldGuiName, const QString& newGuiName);

signals:

    /**
     * Signals that the pointer to the dataset changed
     * @param dataset Pointer to current dataset
     */
    void changed(DatasetImpl* dataset);

    /** Signals that the dataset contents changed */
    void dataChanged();

    /** Signals that the dataset is about to be removed */
    void dataAboutToBeRemoved();

    /**
     * Signals that the dataset has been removed
     * @param datasetId Globally unique identifier of the dataset that is removed
     */
    void dataRemoved(const QString& datasetId);

    /**
     * Signals that the dataset GUI name changed
     * @param oldGuiName Old GUI name
     * @param newGuiName New GUI name
     */
    void dataGuiNameChanged(const QString& oldGuiName, const QString& newGuiName);

protected:
    template<typename DatasetType>
    friend class Dataset;

    friend class DatasetPrivate;
};

}
