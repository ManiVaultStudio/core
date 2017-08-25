var _markerRangeSlider = document.getElementById('markerRange');
noUiSlider.create(_markerRangeSlider, {
        start: [20, 80],
        behaviour: 'tap-drag',
        connect: true,
        range: {
            'min': 0,
            'max': 100
        },
        pips: {
            mode: 'positions',
            values: [0, 25, 50, 75, 100],
            density: 4
        }
});

function rebuildRangeSlider() {
    
    if(_markerSelection.reduce(function (a, b) { return a + b }, 0) < 1) return;

    _markerRangeSlider.noUiSlider.destroy();
        
    _markerRange[0] = d3.min(_data.nodes, function(node) {
        return d3.min(node.expression.filter(function(e, i){ return (_markerSelection[i] > 0); }));
    });
    _markerRange[1] = d3.max(_data.nodes, function(node) {
        return d3.max(node.expression.filter(function(e, i){ return (_markerSelection[i] > 0); }));
    });
    //log(_markerRange);

    _markerUserBounds[0] = Math.max(_markerRange[0], _markerUserBounds[0]);
    _markerUserBounds[1] = Math.min(_markerRange[1], _markerUserBounds[1]);
    
    refreshColormap();
    
    var format = wNumb({ decimals: 2 })
    d3.select("#legendLabelBottom").text(format.to(_markerUserBounds[0]));
    d3.select("#legendLabelTop").text(format.to(_markerUserBounds[1]));

    //log(_markerUserBounds);

    noUiSlider.create(_markerRangeSlider, {
        start:_markerUserBounds,
        behaviour: 'tap-drag',
        connect: true,
        range: {
            'min': 0,
            'max': Math.max(_markerRange[1], 5.0)
        },
        pips: {
            mode: 'positions',
            values: [0, 25, 50, 75, 100],
            density: 7,
            format: wNumb({
                decimals: 2
            })
        }
    });

    _markerRangeSlider.noUiSlider.on('slide', function (values, handle) {
        _markerUserBounds[handle] = parseFloat(values[handle]);
        refreshColormap();
        d3.select("#legendLabelBottom").text(format.to(_markerUserBounds[0]));
        d3.select("#legendLabelTop").text(format.to(_markerUserBounds[1]));
        drawColumns(0);
    });
}

function initContextMenu() {
		
    var cmenu = function () {

        var m = [];

        var numSelectedItems = _selection.reduce(function (a, b) { return a + b }, 0);
        if (numSelectedItems > 1) {

            if (isQtAvailable) {
                m.push({
                    title: "Merge Selected Clusters",
                    action: function () {
                        var firstItem = -1;
                        for (var i = 0; i < _selection.length; i++) { if (_selection[i]) { firstItem = i; break; } }
                        _selectedBeforeMerge = firstItem;
                        d3.select('.d3-context-menu').style('display', 'none');
                        Qt.js_mergeClusters(_selection);
                    }
                });

                // divider
                m.push({ divider: true });
            }
        }

        m.push({
            title: "Toggle Marker Selection",
            action: function () { toggleMarkerSelectionMode(); }
        });

        m.push({
            title: "Toggle Variation",
            action: function () { toggleVariation(); }
        });

        m.push({
            title: "Toggle Dendrogram",
            action: function () { toggleDendrogram(); }
        });

        m.push({
            title: "Toggle Dendrogram Labels",
            action: function () { toggleDendrogramLabels(); }
        });
        
        if (isQtAvailable) {

            // divider
            m.push({ divider: true });

            m.push({
                title: "Save Clusters as FCS...",
                action: function () {
                    d3.select('.d3-context-menu').style('display', 'none');
                    Qt.js_saveClusters();
                }
            });

            m.push({
                title: "Save Cell Origins as CSV ...",
                action: function () {
                    d3.select('.d3-context-menu').style('display', 'none');
                    Qt.js_saveStatistics();
                }
            });

            // divider
            m.push({ divider: true });

            m.push({
                title: "Save Heatmap as Image ...",
                action: printSVG
		      });

		    m.push({
		        title: "Save Heatmap as CSV ...",
		        action: function () {
		            d3.select('.d3-context-menu').style('display', 'none');
		            Qt.js_saveCSV();
		        }
		    });
		}
		return m;
	}
	return cmenu;
}

function toggleSubsetNameModal() {

    var isVisible = document.getElementById("d3-cyto-overlay").style.visibility == "visible";

    if (isVisible) {
        d3.select("#d3-cyto-overlay").transition()
            .delay(500)
            .style("visibility", "hidden");

        d3.select("#subset-modal").transition()
            .duration(500)
            .style("margin-top", "-300px");

        //document.getElementById("subset-input").value = "";
        d3.select("#subset-textfield")
            .classed("is-dirty", false)
            .classed("is-invalid", true);
    } else {
        document.getElementById("subset-input").value = d3.select("#subsetLabel" + _clickedColumeLabelId).text();
        activateItem(document.getElementById("subset-input"));
        d3.select("#d3-cyto-overlay")
            .style("visibility", isVisible ? "hidden" : "visible");
        d3.select("#subset-modal").transition()
            .duration(500)
            .style("margin-top", "0px");

        document.getElementById("subset-input").focus();
    }
}

function activateItem(sender) {

    if (sender.id == "subset-input") {

        d3.select("#subset-accept").attr("disabled", sender.value.length > 0 ? null : "disabled");

    } else {

    }
}

function setSubsetName() {

    var name = document.getElementById("subset-input").value;

    //log(name);

    if (isQtAvailable) {
        Qt.js_setSubsetName(_clickedColumeLabelId, name);
    } else {
        d3.select("#subsetLabel" + _clickedColumeLabelId).text(name);
    }

    toggleSubsetNameModal();
}