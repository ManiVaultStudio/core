#pragma once

#include "Plugin.h"

#include "DataHierarchyItem.h"
#include "Set.h"

namespace hdps
{

namespace plugin
{

/**
 * Transformation plugin base class
 *
 * @author T. Kroes
 */
class TransformationPlugin : public Plugin
{
public:

    /**
     * Constructor
     * @param factory Pointer to transformation plugin factory
     */
    TransformationPlugin(const PluginFactory* factory) :
        Plugin(factory)
    {
    }

    /**
     * The method which handles the data transformation
     * @param datasets Input datasets
     */
    virtual void transform(const Datasets& datasets) = 0;
};

/**
 * Transformation plugin factory class
 *
 * @author T. Kroes
 */
class TransformationPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:

    /** Default constructor */
    TransformationPluginFactory() :
        PluginFactory(Type::TRANSFORMATION)
    {
    }

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name)
    {
        QObject::setObjectName("Plugins/Transformation/" + name);
    }

    /** Destructor */
    ~TransformationPluginFactory() = default;
    
    /** Returns the plugin icon */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("random");
    }

    /** Produces an instance of a transformation plugin */
    TransformationPlugin* produce() override = 0;
};

}

}

Q_DECLARE_INTERFACE(hdps::plugin::TransformationPluginFactory, "hdps.TransformationPluginFactory")
