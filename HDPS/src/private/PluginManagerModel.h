#pragma once

#include <QStandardItemModel>

/**
 * Plugin manager model class
 *
 * Model implementation for the plugin manager
 *
 * @author Thomas Kroes
 */
class PluginManagerModel : public QStandardItemModel
{
public:

    /**
     * Construct plugin manager model with \p parent
     * @param parent Pointer to parent object
     */
    PluginManagerModel(QObject* parent = nullptr);

private:

    /** Initializes the model from the contents of the plugin manager */
    void initializeFromPluginManager();
};
