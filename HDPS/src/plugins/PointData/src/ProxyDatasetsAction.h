#pragma once

#include "PointData.h"
#include "EditProxyDatasetsAction.h"

#include <actions/StringAction.h>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

/**
 * Proxy datasets action class
 *
 * Action class for displaying the number of proxy datasets and providing an edit function
 *
 * @author Thomas Kroes
 */
class ProxyDatasetsAction : public WidgetAction
{
    Q_OBJECT

protected:

    /** Widget class for proxy datasets action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param proxyDatasetsAction Pointer to proxy datasets action
         */
        Widget(QWidget* parent, ProxyDatasetsAction* proxyDatasetsAction);
    };

    /**
     * Get widget representation of the proxy datasets action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param points Smart pointer to points dataset
     */
    ProxyDatasetsAction(QObject* parent, const Dataset<Points>& points);

public: // Action getters

    StringAction& getCountAction() { return _countAction; }
    EditProxyDatasetsAction& getEditProxyDatasetsAction() { return _editProxyDatasetsAction; }

signals:

    /** Signals that the dimension names changed
     * @param dimensionNames Dimension names
     */
    void dimensionNamesChanged(const QStringList& dimensionNames);

protected:
    Dataset<Points>             _points;                        /** Points dataset reference */
    StringAction                _countAction;                   /** Number of proxy datasets action */
    EditProxyDatasetsAction     _editProxyDatasetsAction;       /** Manual update action */
};
