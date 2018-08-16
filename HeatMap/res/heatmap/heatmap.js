// =============================================================================
// find out if the QtBridge is available
// otherwise we are running in the browser
// =============================================================================
try {
    new QWebChannel(qt.webChannelTransport, function (channel) {

        QtBridge = channel.objects.QtBridge;

        QtBridge.qt_setData.connect(function () { setData(arguments[0]); });
        QtBridge.qt_setSelection.connect(function () { setSelection(arguments[0]); });
        QtBridge.qt_setHighlight.connect(function () { setHighlight(arguments[0]); });
        QtBridge.qt_addAvailableData.connect(function () { addAvailableData(arguments[0]); });
        QtBridge.qt_setMarkerSelection.connect(function () { initMarkerSelection(arguments[0]); });

        notifyBridgeAvailable();
    });
} catch (error) {
	isQtAvailable = false;
	log("could not connect qt");
}

// =============================================================================
// globals
// =============================================================================

// data ========================================================================
var _data = null;
var _availableDataSets = [];

var _markerRange = [0.0, 5.0];
var _markerUserBounds = [-1.0, 99999999999.0];

var _selection = [];
var _selectedBeforeMerge = -1;
var _highlight = -1;

var _markerSelection = [];
var _markerSelectionSAT = [];

var _sorting = [];
var _sortingMarker = [];
var _sortingDendrogram = [];
var _sortBy = -1;

var _dendrogramClusters = [];

// sizes =======================================================================
const _legendUIWidth = 90;       // legendWidth
const _subsetLabelUIHeight = 90; // colLabelHeight

var _svgWidth = width - _legendUIWidth;
var _svgHeight = height - 20 ;//- _subsetLabelUIHeight;

const _markerSelectionUIWidth = 30;

var _labelColumnWidth = 0;

const _dendrogramHeight = 120;    // dendroHeight
const _dendrogramTopMargin = 20;

var _heatmapHeight = 100;

const _cellBorder = 0.0;
const _selectionBorderWidth = 1.5;

const _magnifier = 1.2;

// flags =======================================================================
var _isMakerSelectionActive = false;
var _isDendrogramActive = false;
var _isDendrogramLabelsActive = true;

var _showVariation = false; // showStdDev

var _sortLowToHigh = false;

// scales ======================================================================
var _variationScale = d3.scaleLinear()
                        .domain([0.0, 3.0])
                        .range([1.0, 0.05])
                        .clamp(true);

var _dendrogramScale = d3.scaleLinear()
                        .range([_dendrogramHeight, _dendrogramTopMargin]);

// UI ==========================================================================
const _bgcolor = "#fcfcfc";
const _contextMenu = initContextMenu();

const _markerCircle = { "x": _labelColumnWidth + _markerSelectionUIWidth / 2.5, "y": -1, "r": 7.5 };
const _markerRect = { w: 7, h: 2.5 };

var _subsetLabels =  d3.select("body").append("div")
    .attr("id", "labelsDiv");

var _containerSvg = d3.select("body").select("#mainView").append("svg")
	.attr("id", "container")
	.attr("width", _svgWidth)
	.attr("height", height)
	.on("contextmenu", d3.contextMenu(_contextMenu));

var _dendrogram = _containerSvg.append("g")  // dendroSVG
	.attr("id", "dendrogramGroup") // dendroGroup
	.attr("height", _dendrogramHeight);

var _heatmap = _containerSvg.append("g") // heatmapSVG
	.attr("id", "heatmapGroup")
	.attr("transform", function (d, i) { return "translate(0, " + (_isDendrogramActive ? _dendrogramHeight : 0 )+ ")" });

var _heatmapColumns = null;     // columnsSvg
var _columns = null;
var _columnLabels = null;
var _columnLabelRects = null;
var _columnLabelsGroup = null;
var _clickedColumeLabelId = 0;
var _cells = null;
var _cellPaint = null;
var _columnSelect = {left: null, right: null, top: null, bottom: null};
var _columnSelectSorted = null;

var _markerLabels = null;       // markerLabels

var _dataQueue = new HeatmapDataQueue(1, queueData);

// =============================================================================
// Init
// =============================================================================
function initHeatMapLayout() {

	if (!_data) return;

	_heatmap.selectAll('*').remove();
	_subsetLabels.selectAll('*').remove();

	_heatmapColumns = _heatmap.append("g").attr("id", "columns");

    
    // =========================================================================
    // marker labels
	var markerLabelColumn = _heatmapColumns.selectAll(".labelColumn")
					.data([1])
					.enter().append("g")
					.attr("class", "labelColumn")
					.attr("transform", function (d, i) { return "translate(0, 0)" });

	_markerLabels = markerLabelColumn.selectAll(".markerLabel")
            .data(_data.names)
			.enter().append("g")
			.attr("class", "markerLabel");
    
    _markerLabels.append("text")
			.attr("class", "markerLabelText")
            //.text(function (d, i ) { if(_isMakerSelectionActive || _markerSelection[i] > 0) return d })
            .text(function (d, i ) { return d })
            .on("mousedown", function (d, i) { if (!_isMakerSelectionActive && _markerSelection[i] > 0 && !_isDendrogramActive) leftClickMarkerLabel(i, true); });;
      
	_markerLabels.append("circle")
			.attr("class", "markerSelectCirc")
			.attr("id", function(d,i){return "markerSelectCirc" + i;})
			.attr("cx", _markerCircle.x )
			.attr("cy", _markerCircle.y )
			.attr("r", _markerCircle.r )
			.on("mousedown", function (d,i) {  toggleMarker(i); });
	_markerLabels.append("rect")
			.attr("class", "markerSelectRectHor")
			.attr("width", _markerRect.w )
			.attr("height", _markerRect.h )
			.attr("x", _markerCircle.x - _markerRect.w/2 )
			.attr("y", _markerCircle.y - _markerRect.h/2 )
			.attr("fill", _bgcolor )
			.on("mousedown", function (d,i) {  toggleMarker(i); });
	_markerLabels.append("rect")
			.attr("class", "markerSelectRectVert")
			.attr("id", function(d,i){return "markerSelectVarBar" + i;})
			.attr("fill", _bgcolor )
			.on("mousedown", function (d,i) {  toggleMarker(i); });

    // =========================================================================
    // heatmap
	_columns = _heatmapColumns.selectAll(".column")
					.data(_data.nodes)
					.enter().append("g")
					.attr("class", "column")
					.attr("id", function (d, i) { return "cluster" + i })
					.on("contextmenu", d3.contextMenu(_contextMenu));

	var dat = _columns.data();

	_cells = _columns.selectAll(".cell")
				    .data(function(d, i)
                    {
				        var e = dat[i].expression;
						var s = dat[i].stddev;
						if(!s) s = dat[i].expression;
                        
                        var arr = [];
						for(var l = 0; l < e.length; l++)
						{
						  arr[l] = { "expression": e[l], "stddev": s[l], "column": i };
						}
						return arr;
				    })
				    .enter().append("g")
				    .attr("class", "cell")
					.attr("id", function (d, i) { return i });
    
	_cells.append("rect")
			.attr("class", "cellBound")
			.attr("fill", _bgcolor)
			.attr("x", 0)
			.attr("y", 0)
            .on("mouseover", function (d, i) {hoverColumn(d.column); })
            .on("mouseout", function (d, i) {leaveColumn(d.column); })
            .on("click", function (d, i) {leftClickColumn(d.column); });
    
	_cellPaint = _cells.append("rect")
			.attr("class", "cellPaint")
            .on("mouseover", function (d, i) {hoverColumn(d.column); })
            .on("mouseout", function (d, i) {leaveColumn(d.column); })
            .on("click", function (d, i) { leftClickColumn(d.column); });

	_columnLabelsGroup = _columns.append("g")
	_columnLabelRects = _columnLabelsGroup.append("rect")
                        .attr("width", _subsetLabelUIHeight)
                        .attr("fill-opacity", 1.0)
                        .attr("fill", _bgcolor)
			            .attr("stroke", _bgcolor)
                        .on("click", function (d, i) { _clickedColumeLabelId = i; toggleSubsetNameModal(); });

	_columnLabels = _columnLabelsGroup.append("text")
			.attr("class", "subsetLabelText")
			.attr("id", function (d, i) { return "subsetLabel" + i; })
			.attr("x", _subsetLabelUIHeight-10)
            .text(function (d, i) { return dat[i].name; })
            .on("click", function (d, i) { _clickedColumeLabelId = i; toggleSubsetNameModal(); });

    
    // =========================================================================
    // column selection highlights
    
    // single columns
	var columnSelect = _heatmapColumns.selectAll(".columnSelect")
			.data(_data.nodes)
			.enter().append("g")
			.attr("class", "columnSelectSingle");

	_columnSelect.left = columnSelect.append("line")
			.attr("class", "columnBound")
			.attr("x1", 0)
			.attr("y1", 0)
			.attr("x2", 0);
	_columnSelect.right = columnSelect.append("line")
			.attr("class", "columnBound")
			.attr("y1", 0);
	_columnSelect.top = columnSelect.append("line")
			.attr("class", "columnBound");
	_columnSelect.bottom = columnSelect.append("line")
			.attr("class", "columnBound")
			.attr("y1", 0);
    
    // sorted columns
    _columnSelectSorted = _heatmapColumns.append("g")
      .attr("class", "columnSelect");
      
	_columnSelectSorted.append("line")
        .attr("class", "columnBound")
        .attr("id", "columnBoundLeft")
        .attr("x1", 0)
        .attr("y1", 0)
        .attr("x2", 0);
	_columnSelectSorted.append("line")
        .attr("class", "columnBound")
        .attr("id", "columnBoundRight")
        .attr("y1", 0);
	_columnSelectSorted.append("line")
        .attr("class", "columnBound")
        .attr("id", "columnBoundTop");
	_columnSelectSorted.append("line")
        .attr("class", "columnBound")
        .attr("id", "columnBoundBottom")
        .attr("y1", 0)
        .attr("y2", 0);


    // =========================================================================
    // add variable properties
    drawHeatMap(0);
}

// =============================================================================
// drawing
// =============================================================================
function drawHeatMap(dur) {

	if (!_data) return;
    
    _heatmap.transition()
        .duration(dur)
        .attr("transform", function (d, i) { return "translate(0, " + (_isDendrogramActive ? _dendrogramHeight : 0 )+ ")" });
    
    drawMarkerLabelColumn(dur);
    
    drawColumns(dur);
    
    drawSelectionHighlights(dur);
    
    drawDendrogram(dur);
}

function drawMarkerLabelColumn(dur) {
    

	var numDimensions = _data.nodes[0].expression.length;
    var numActiveMarkers = _markerSelection.reduce(function (a, b) { return a + b }, 0);
    var rowHeight = _heatmapHeight / (_isMakerSelectionActive ? numDimensions : numActiveMarkers);
    
    _markerCircle.x = _labelColumnWidth + _markerSelectionUIWidth / 2.5;
    
    _markerLabels.transition()
        .duration(dur)
        .attr("transform", function (d, i) {
            if(_isMakerSelectionActive)
              return "translate(0, " + ((i+0.5) * rowHeight) + ")";
            else
              return "translate(" + ((_markerSelection[i] > 0) ? 0 : -500) + ", " + ((_markerSelectionSAT[i]+0.5) * rowHeight) + ")";
        });
    
    _markerLabels.selectAll("text").transition()
        .duration(dur)
        .attr("transform", function (d) { return "translate(" + (_labelColumnWidth - d.width() - 5) + ", 0)" });
    
    d3.selectAll(".markerSelectCirc").transition()
        .duration(dur)
        .attr("opacity", _isMakerSelectionActive ? 1.0 : 0.0 )
        .attr("cx", _markerCircle.x )
        .attr("fill", function(d,i){ return (_markerSelection[i] == 1 ? "red" : "green");} );
    
	d3.selectAll(".markerSelectRectHor").transition()
        .duration(dur)
        .attr("opacity", _isMakerSelectionActive ? 1.0 : 0.0)
        .attr("x", _markerCircle.x - _markerRect.w/2 );
    
	d3.selectAll(".markerSelectRectVert").transition()
        .duration(dur)
        .attr("width", function(d,i){return (_markerSelection[i] == 1 ? _markerRect.w : _markerRect.h);} )
		.attr("height", function(d,i){return (_markerSelection[i] == 1 ? _markerRect.h : _markerRect.w );} )
		.attr("x", function(d,i){return (_markerSelection[i] == 1 ? _markerCircle.x - _markerRect.w/2 : _markerCircle.x - _markerRect.h/2);} )
		.attr("y", function(d,i){return (_markerSelection[i] == 1 ? _markerCircle.y - _markerRect.h/2 : _markerCircle.y - _markerRect.w/2);} )
        .attr("opacity", _isMakerSelectionActive ? 1.0 : 0.0 );
}

function drawColumns(dur) {
    
	var numDimensions = _data.nodes[0].expression.length;
    var numActiveMarkers = _markerSelection.reduce(function (a, b) { return a + b }, 0);
    
    var offset = _labelColumnWidth + (_isMakerSelectionActive ? _markerSelectionUIWidth : 0);
	var rectSize = {x: (_svgWidth - offset) / _data.nodes.length, y: _heatmapHeight / (_isMakerSelectionActive ? numDimensions : numActiveMarkers)};
	var cellSize = {x: Math.max(0, rectSize.x - _cellBorder), y: Math.max(0, rectSize.y - _cellBorder)};
        
    var rectSizeHighlight = rectSize.x * _magnifier;
    var cellSizeHighlight = cellSize.x * _magnifier;
    
    if(_highlight >= 0)
    {
        rectSize.x = rectSize.x - ((rectSizeHighlight - rectSize.x) / (_data.nodes.length - 1));
        cellSize.x = cellSize.x - ((cellSizeHighlight - cellSize.x) / (_data.nodes.length - 1));
    }
    
    _columns.transition()
        .duration(dur)
        .attr("transform", function (d, i) {
            var translateBy = offset + _sorting[i] * rectSize.x;
            if( _sorting[i] != 0 && (_sorting[i] > _sorting[_highlight]))
                translateBy = translateBy + (rectSizeHighlight - rectSize.x);
            return "translate(" + translateBy + ", 0)";
        });
    
    _cells.transition()
        .duration(dur)
        .attr("transform", function (d, i) {
            var scale = (_isMakerSelectionActive ? i : _markerSelectionSAT[i]);
            return "translate(0, " + (scale * rectSize.y) + ")";
        });
    
    _cells.selectAll(".cellBound").transition()
        .duration(dur)
        .attr("width", function (d, i) {
            return (_highlight == d.column) ? rectSizeHighlight : cellSize.x;
        })
		.attr("height", cellSize.y);
    
    _cellPaint.transition()
        .duration(dur)
        .attr("x", function (d) {
            var sizeX = (_highlight == d.column) ? rectSizeHighlight : cellSize.x;
            return _showVariation ? (sizeX * (1.0 - _variationScale(d.stddev)) / 2) : 0.0;
        })
		.attr("y", function (d) { return _showVariation ? (cellSize.y * (1.0 - _variationScale(d.stddev)) / 2) : 0.0; })
		.attr("width",  function (d) {
            var sizeX = (_highlight == d.column) ? rectSizeHighlight : cellSize.x;
            return (_showVariation ? (sizeX * _variationScale(d.stddev)) : sizeX);
        })
		.attr("height", function (d) { return _showVariation ? (cellSize.y * _variationScale(d.stddev)) : cellSize.y; })
		.attr("fill", function (d) { return _color(d.expression); });

    _columnLabelsGroup.transition()
        .duration(dur)
        .attr("height", _subsetLabelUIHeight)
        .attr("width", function (d, i) {
            return (_highlight == d.column) ? rectSizeHighlight : cellSize.x;
        })
        .attr("transform", function (d, i) {
            return "translate(" + 0 + ", " + (_heatmapHeight + _subsetLabelUIHeight) + ")" + "rotate(-90)";
        });

    _columnLabelRects.transition()
        .duration(dur)
        .attr("height", function (d, i) {
            return ((_highlight == d.column) ? rectSizeHighlight : cellSize.x);
        });

    _columnLabels.transition()
        .duration(dur)
        .attr("y", function (d, i) {
            return ((_highlight == d.column) ? rectSizeHighlight : cellSize.x) / 2;
        });

}

function drawSelectionHighlights(dur) {
    
    refreshSelectionHighlightClasses();
    
	var numDimensions = _data.nodes[0].expression.length;
    var numActiveMarkers = _markerSelection.reduce(function (a, b) { return a + b }, 0);
    
    var offset = _labelColumnWidth + (_isMakerSelectionActive ? _markerSelectionUIWidth : 0);
	var rectSize = {x: (_svgWidth - offset) / _data.nodes.length, y: _heatmapHeight / (_isMakerSelectionActive ? numDimensions : numActiveMarkers)};
    
    var rectSizeHighlight = rectSize.x * _magnifier;
    if(_highlight >= 0)
    {
        rectSize.x = rectSize.x - ((rectSizeHighlight - rectSize.x) / (_data.nodes.length - 1));
    }
    
    var numSelectedItems = _selection.reduce(function (a, b) { return a + b }, 0);
    var selectIdx = _selection.map(function (d, i) { if(d==1) return i; else return -1})
                           .filter(function(e){ return (e > -1); });
        
    
    // single columns
    d3.selectAll(".columnSelectSingle").transition()
            .duration(dur)
			.attr("transform", function (d, i) { return "translate(" + (offset + i * rectSize.x) + ", 0)" });

	_columnSelect.left.transition()
            .duration(dur)
			.attr("y2", _heatmapHeight);
	_columnSelect.right.transition()
            .duration(dur)
			.attr("x1", rectSize.x)
			.attr("x2", rectSize.x)
			.attr("y2", _heatmapHeight);
	_columnSelect.top.transition()
            .duration(dur)
			.attr("x1", -_selectionBorderWidth)
			.attr("y1", _heatmapHeight)
			.attr("x2", rectSize.x + _selectionBorderWidth)
			.attr("y2", _heatmapHeight);
	_columnSelect.bottom.transition()
            .duration(dur)
			.attr("x1", -_selectionBorderWidth)
			.attr("x2", rectSize.x + _selectionBorderWidth);
    
    // sorted columns
    if (_isDendrogramActive && numSelectedItems == 2) {
        _columnSelectSorted.style("opacity", 0.0);
        _columnSelectSorted.transition()
			.duration(0)
			.delay(dur)
			.style("opacity", 1.0);
    } else {
        _columnSelectSorted.style("opacity", 1.0);
    }
    
    var translateCol = ( numSelectedItems == 1 ) ? (offset + _sorting[selectIdx] * rectSize.x) : offset;
    if( _sorting[selectIdx] != 0 && (_sorting[selectIdx] > _sorting[_highlight]))
        translateCol = translateCol + (rectSizeHighlight - rectSize.x);
    
    var selectWidth = numSelectedItems * rectSize.x;
    if(_selection[_highlight] > 0) selectWidth = selectWidth - rectSize.x + rectSizeHighlight;
    
    _columnSelectSorted.transition()
            .duration(dur)
            .attr("transform", "translate(" + translateCol + ", 0)" );
      
	_columnSelectSorted.selectAll("#columnBoundLeft").transition()
            .duration(dur)
            .attr("y2", _heatmapHeight);
	_columnSelectSorted.selectAll("#columnBoundRight").transition()
            .duration(dur)
            .attr("x1", selectWidth)
            .attr("x2", selectWidth)
            .attr("y2", _heatmapHeight);
	_columnSelectSorted.selectAll("#columnBoundTop").transition()
            .duration(dur)
            .attr("x1", -_selectionBorderWidth)
            .attr("y1", _heatmapHeight)
            .attr("x2", selectWidth + _selectionBorderWidth)
            .attr("y2", _heatmapHeight);
	_columnSelectSorted.selectAll("#columnBoundBottom").transition()
            .duration(dur)
            .attr("x1", -_selectionBorderWidth)
            .attr("x2", selectWidth + _selectionBorderWidth);
}

function refreshSelectionHighlightClasses() {
    
    var numSelectedItems = _selection.reduce(function (a, b) { return a + b }, 0);
    
    
    if (_isDendrogramActive)
    {
    	if (numSelectedItems == 1)
        {
            _columnSelect.left.classed("selected", false );
            _columnSelect.right.classed("selected", false );
            _columnSelect.top.classed("selected", false );
            _columnSelect.bottom.classed("selected", false );

    		_columnSelectSorted.classed("selected", true );
    	}
        else
        {
    		var inverseSorting = [];
    		inverseSorting.length = _sorting.length;
    		for (var i = 0; i < _sorting.length; i++)
    		{
    			inverseSorting[_sorting[i]] = i;
    		}

    		_columnSelect.left.classed("selected", function (d, i) { return (_selection[inverseSorting[i]] && !_selection[inverseSorting[i-1]]) });
    		_columnSelect.right.classed("selected", function (d, i) { return (_selection[inverseSorting[i]] && !_selection[inverseSorting[i+1]]) });
    		_columnSelect.top.classed("selected", function (d, i) { return _selection[inverseSorting[i]] });
    		_columnSelect.bottom.classed("selected", function (d, i) { return _selection[inverseSorting[i]] });

    		_columnSelectSorted.classed("selected", false );
		}
    }
    else
    {
    	_columnSelect.left.classed("selected", false );
    	_columnSelect.right.classed("selected", false );
    	_columnSelect.top.classed("selected", false );
    	_columnSelect.bottom.classed("selected", false );

    	_columnSelectSorted.classed("selected", function () { return (numSelectedItems > 0); });
    }    
}

function drawDendrogram(dur) {

	if (!_isDendrogramActive)
    {        
	   _dendrogram.selectAll('*').remove();
        return;
    }
    
	if (_dendrogramClusters == undefined || _dendrogramClusters[0] == undefined) return;
    
	var dendrogramWidth = _svgWidth - _labelColumnWidth - (_isMakerSelectionActive ? _markerSelectionUIWidth : 0);

	_dendrogramScale.domain([0, Math.round(_dendrogramClusters[0].dist + 0.49)]);
    
    var dendroRoot = d3.hierarchy(
        _dendrogramClusters[0],
        function (d) { return (d.left && d.right) ? [d.right, d.left] : undefined; }
    );

	var dendroTree = d3.cluster()
        .separation(function (a, b) { return 1; })
        .size([dendrogramWidth, _dendrogramHeight - _dendrogramTopMargin]);
        
    dendroTree(dendroRoot);
    
	_dendrogram.transition()
        .duration(dur)
        .attr("width", dendrogramWidth)
        .attr("transform", function (d, i) { return "translate(" + (_labelColumnWidth + (_isMakerSelectionActive ? _markerSelectionUIWidth : 0)) + ",0)" });
    
	_dendrogram.selectAll('*').remove();

	//var nodes = dendroTree.nodes(_dendrogramClusters[0]);
    var nodes = dendroRoot.descendants();
	var link = _dendrogram.selectAll(".dendrogramLink")
                //.data(dendroTree.links(nodes))
                .data(nodes.slice(1))
                .enter().append("path")
                .attr("class", "dendrogramLink")
                .attr("d", dendrogramElbow);

	if (_isDendrogramLabelsActive) {

		var node = _dendrogram.selectAll(".dendrogramNode")
                    .data(nodes)
                    .enter().append("g")
                    .attr("class", "dendrogramNode")
                    .attr("transform", function (d) { return "translate(" + Math.round(d.x) + "," + Math.round(_dendrogramScale(d.data.dist < 99999999.9 ? d.data.dist : 0)) + ")"; })

		node.append("text")
			 .attr("class", "dendrogramLabel")
			 .attr("text-anchor", "middle")
			 .attr("y", -2)
			 .attr("x", function (d) {
                    if (d.parent == undefined) { return 0 };
                    return (d.parent.children[1] == d ? 20 : -20);
                })
			 .text(function (d) { return (d.data.dist < 99999999.9 ? d.data.dist.toFixed(2) : ""); });
	}
}

// =============================================================================
// interaction
// =============================================================================
function leftClickColumn(idx) {
    
    var add = d3.event.shiftKey;
    
    var sum = _selection.reduce(function (a, b) { return a + b }, 0);
    
    var newState = 1 - _selection[idx];
    if( !add )
    {
        for (var i = 0; i < _selection.length; i++) _selection[i] = 0;
    }
    _selection[idx] = newState;
    
    if (isQtAvailable) { QtBridge.js_selectionUpdated(_selection); }
    
	updateSorting(_sortBy, false);
    
	drawColumns(500);
    drawSelectionHighlights(500);
}

function hoverColumn(idx) {
    
    if (isQtAvailable) { QtBridge.js_highlightUpdated(idx); }

    return;

    _highlight = idx;
    
	drawColumns(250);
    drawSelectionHighlights(250);
}

function leaveColumn(idx) {
    
    if (isQtAvailable) { QtBridge.js_highlightUpdated(idx); }

    return;

    _highlight = -1;
    
	drawColumns(250);
    drawSelectionHighlights(250);
}

function leftClickMarkerLabel(index, switchOrder) {

	updateSorting(index, switchOrder);

	drawColumns(500);
	drawSelectionHighlights(500);
}

function leftClickColormap(idx) {
    
    setColormap(idx);
    
    drawColumns(500);
}

function toggleMarkerSelectionMode() {
    
    if (!_data) return;

	_isMakerSelectionActive = !_isMakerSelectionActive;

    drawHeatMap(500);
}

function toggleVariation() {
	
	_showVariation = !_showVariation;

	var e = document.getElementById("uncertaintySwitch").querySelector(".mdl-js-switch");
	if (_showVariation) { e.MaterialSwitch.on(); }
	else { e.MaterialSwitch.off(); }
    
    drawColumns(500);
}

function toggleMarker( marker ) {
  
    _markerSelection[marker] = !_markerSelection[marker];
    
    refreshMarkerSelectionSAT();
    
    rebuildRangeSlider();
    drawColumns(500);
    drawMarkerLabelColumn(500);

    if(_isDendrogramActive)
    {
        updateSorting();
        drawHeatMap(500);
    }
}

function toggleDiscreteColormap () {
    
    _isColormapDiscrete = !_isColormapDiscrete;
    
    setColormap(_activeColormap, false);
    
    drawHeatMap(500);
}

function toggleDendrogram() {

	_isDendrogramActive = !_isDendrogramActive;

	var e = document.getElementById("hierarchicalSwitch").querySelector(".mdl-js-switch");
	if (_isDendrogramActive) { e.MaterialSwitch.on(); }
	else { e.MaterialSwitch.off(); }
    
	_heatmapHeight = _svgHeight - (_isDendrogramActive ? _dendrogramHeight : 0) - _subsetLabelUIHeight;
    
    updateSorting(_sortBy, false);
    
    drawHeatMap(500);
}

function toggleDendrogramLabels() {

	_isDendrogramLabelsActive = !_isDendrogramLabelsActive;

	var e = document.getElementById("hierarchicalLabelSwitch").querySelector(".mdl-js-switch");
	if (_isDendrogramLabelsActive) { e.MaterialSwitch.on(); }
	else { e.MaterialSwitch.off(); }

	drawDendrogram(0);
}

function moveSelection( moveTo )
{  
    var sum = _selection.reduce(function (a, b) { return a + b }, 0);

    if( sum > 1 ) return;
    if( sum < 1 && (moveTo == "left" || moveTo == "right") ) return;
    
    var selectedIdx = 0;
    if( sum > 0 )
    {
        for (var i = 0; i < _selection.length; i++){ if(_selection[i] > 0){ selectedIdx = i; break;} }
        _selection[selectedIdx] = 0;
    }

    var targetIdx = 0;
    var d = 200;
    if( moveTo == "left" )
    {
        targetIdx = Math.max(0, _sorting[selectedIdx] - 1);
    }
    else if( moveTo == "right" )
    {
        targetIdx = Math.min(_selection.length-1, _sorting[selectedIdx] + 1); 
    }
    else if( moveTo == "first" )
    {    
        targetIdx = 0;
        d = 750;
    }
    else if( moveTo == "last" )
    {    
        targetIdx = _sorting.length - 1;
        d = 750;
    }
    
    for (var i = 0; i < _sorting.length; i++)
    {
        if( _sorting[i] == targetIdx ){ _selection[i] = 1; break; }
    }

    if (isQtAvailable) { QtBridge.js_selectionUpdated(_selection); }

    drawSelectionHighlights(d);
}

// =============================================================================
// dendrogram
// =============================================================================
function computeDendrogram() {

	if (!_isDendrogramActive) return;

	var input = [];
	input.length = _data.nodes.length;

	var numActiveMarkers = _markerSelection.reduce(function (a, b) { return a + b }, 0);

	for (var i = 0; i < input.length; i++)
	{
		var shortExpression = [];
		shortExpression.length = numActiveMarkers;

		var expression = _data.nodes[i].expression;

		var m = 0;
		for(var j = 0; j < expression.length; j++)
		{
			if(_markerSelection[j] > 0)
			{
				shortExpression[m] = expression[j];
				m += 1;
			}
		}

		input[i] = shortExpression;
	}

	_dendrogramClusters = clusterfck.hcluster(input, clusterfck.EUCLIDEAN_DISTANCE);
    
    if (_dendrogramClusters != undefined) {
		sortByDendrogram();
	}
}

function dendrogramElbow(d, i)
{
    //log(d);
/*	return "M" + Math.round(d.source.x) + "," + Math.round(_dendrogramScale(d.source.dist < 99999999.9 ? d.source.dist : 0))
         + "H" + Math.round(d.target.x) + "V" + Math.round(_dendrogramScale(d.target.dist < 99999999.9 ? d.target.dist : 0));*/
	return "M" + Math.round(d.parent.x) + "," + Math.round(_dendrogramScale(d.parent.data.dist < 99999999.9 ? d.parent.data.dist : 0))
         + "H" + Math.round(d.x) + "V" + Math.round(_dendrogramScale(d.data.dist < 99999999.9 ? d.data.dist : 0));
}

// =============================================================================
// sorting
// =============================================================================
function updateSorting(index, switchOrder) {
    
    if(_isDendrogramActive)
    {
        computeDendrogram();
    }
    else
    {
        sortByMarker(index, switchOrder);
    }
}

function sortByMarker(index, switchOrder) {
    
	if (!_data || !_data.nodes) return;

	if (index >= _data.nodes[0].expression.length) {
		index = -1;
	}

	_sortingMarker.length = _sorting.length;

	// Sort selected first, then unselected
	var numSelectedItems = _selection.reduce(function (a, b) { return a + b }, 0);
	var sortSelectionFirst = (numSelectedItems > 1);

	if (index < 0) {
		if (sortSelectionFirst) {
			var tmp = []; tmp.length = _sorting.length;
			var sidx = 0;
			// manual sorting to reduce unnecessary animation
			for (var i = 0; i < _selection.length; i++) {
				if (_selection[i])
					tmp[sidx++] = i;
			}
			for (var i = 0; i < _selection.length; i++) {
				if (!_selection[i])
					tmp[sidx++] = i;
			}
			for (var i = 0; i < tmp.length; i++) {
				_sortingMarker[tmp[i]] = i;
			}
		} else {
			for (var i = 0; i < _sortingMarker.length; i++) {
				_sortingMarker[i] = i;
			}
		}
		_sorting = _sortingMarker;
		return;
	}

	var values = _data.nodes.map(function (d, i) { return [d.expression[index], i, _selection[i]] });
	if (sortSelectionFirst) {
		values.sort(function (a, b) {
			var sel = a[2] - b[2];
			if (sel == 0)
				return a[0] - b[0];
			else
				return sel;
		});
	} else {
		values.sort(function (a, b) { return a[0] - b[0]; });
	}

	var tmp = values.map(function (d, i) { return d[1] });

	if (switchOrder && _sortBy == index) // flip high -> low // low -> high
	{
		_sortLowToHigh = !_sortLowToHigh;
	}
	_sortBy = index;


	if (_sortLowToHigh) {
		if (sortSelectionFirst) {
			var numUnselectedItems = tmp.length - numSelectedItems;
			for (var i = 0; i < numUnselectedItems; i++) {
				_sortingMarker[tmp[i]] = numSelectedItems + i;
			}
			for (var i = numUnselectedItems; i < tmp.length; i++) {
				_sortingMarker[tmp[i]] = i - numUnselectedItems;
			}
		} else {
			for (var i = 0; i < tmp.length; i++) {
				_sortingMarker[tmp[i]] = i;
			}
		}
	} else {

		for (var i = 0; i < tmp.length; i++) {
			_sortingMarker[tmp[i]] = tmp.length - 1 - i;
		}
	}

	_sorting = _sortingMarker;
}

function sortByDendrogram() {
    
	var dendrogramSorting = [];

	sortByDendrogrammRecursive(_dendrogramClusters[0], dendrogramSorting);

	for (var i = 0; i < dendrogramSorting.length; i++) {

		_sortingDendrogram[dendrogramSorting[i]] = i;
	}
	_sorting = _sortingDendrogram;
}

function sortByDendrogrammRecursive(root, sort) {
    
	if (root.right != undefined) {
		sortByDendrogrammRecursive(root.right, sort)
	}
	if (root.left != undefined)
	{
		sortByDendrogrammRecursive(root.left, sort)
	}

	if (root.left == undefined && root.right == undefined)
	{
		sort.push(root.key);
	}
}

function refreshMarkerSelectionSAT() {

    _markerSelectionSAT.length = _markerSelection.length;
    _markerSelectionSAT[0] = 0;

    for(var i = 1; i < _markerSelection.length; i++)
    {
        _markerSelectionSAT[i] = _markerSelectionSAT[i-1] + _markerSelection[i-1];
    }
}

// =============================================================================
// external
// =============================================================================
function addAvailableData(name) {
	
	for(var i = 0; i < _availableDataSets.length; i++)
	{
		if( name == _availableDataSets[i] )
		{
			return;
		}
	}

	_availableDataSets.push(name);

	updateAvailableDataSelectionBox();
}

function queueData(d) {

    //log(d);

    _data = JSON.parse(d);

    //log(_data);
    //log("setting data");

    _labelColumnWidth = 0;
    for (var i = 0; i < _data.names.length; i++)
    {
        var l = _data.names[i].width() + 10;
        _labelColumnWidth = Math.max(l, _labelColumnWidth);
    }
    
    _selection.length = _data.nodes.length;
    for (var i = 0; i < _selection.length; i++) _selection[i] = 0;
    
    if( _selectedBeforeMerge >= 0 )
    {
        _selection[_selectedBeforeMerge] = 1;
        _selectedBeforeMerge = -1;
    }
    
    if( _markerSelection.length < _data.names.length )
    {
        var start = _markerSelection.length;
        _markerSelection.length = _data.names.length;
      
        for (var i = start; i < _markerSelection.length; i++)
        {
            _markerSelection[i] = (start == 0) ? 1 : 0;
        }

        refreshMarkerSelectionSAT();
    }

    _sorting.length = _data.nodes.length;
    updateSorting(_sortBy, false);
    rebuildRangeSlider();
	initHeatMapLayout();

	//log("Data set.");
}

function setData(d) {    

    log("setting data");

    _dataQueue.addData(d);

    log("Data set.");
}

function setSelection(slct) {

    var sum = _selection.reduce(function (a, b) { return a + b }, 0);
    
    if( slct ){ _selection = slct; }
    
    var newSum = _selection.reduce(function (a, b) { return a + b }, 0);
    if( sum > 1 ||  newSum > 1 )
    {
	   updateSorting(_sortBy, false);
    }

	drawColumns(500);
    drawSelectionHighlights(500);
}

function setHighlight(highlight) {

    return;

    _highlight = highlight;

	drawColumns(500);
    drawSelectionHighlights(500);
}

function setActiveCluster(name) {
    
	if(isQtAvailable)
	{
	    QtBridge.js_selectData(name);
	}
}

function initMarkerSelection(markers) {

    if (_data) return;

    if (markers.length > _markerSelection.length) {
        _markerSelection.length = markers.length;
    }

    var numActiveMarkers = 0;
    for (var i = 0; i < _markerSelection.length; i++) {
        _markerSelection[i] = markers[i];
    }

    refreshMarkerSelectionSAT();
}



// =============================================================================
// Windowing ===================================================================
// =============================================================================
function resize() {

	width = window.innerWidth;
	height = window.innerHeight;
    
	_svgWidth = width - _legendUIWidth;
    _svgHeight = height - 20;
    
    _heatmapHeight = _svgHeight - (_isDendrogramActive ? _dendrogramHeight : 0) - _subsetLabelUIHeight;

	if (_containerSvg) _containerSvg.attr("width", _svgWidth).attr("height", _svgHeight);

	drawHeatMap(0);
	//drawDendrogram(dendroClusters);
}


// =============================================================================
// UI ==========================================================================
// =============================================================================
function updateAvailableDataSelectionBox() {
	
	var selector = document.getElementById("clusterSelector");
	var selected;
	if (selector.length > 0) {
		selected = selector.selectedIndex;
	}
	selector.length = 1;

	for (var i = 0; i < _availableDataSets.length; i++) {

		var opt = document.createElement("option");
		opt.text = _availableDataSets[i];
		opt.value = _availableDataSets[i];

		selector.add(opt);
	}

	selector.selectedIndex = selected;
}

// =============================================================================
// run =========================================================================
// =============================================================================

resize();
d3.select(window).on("resize", resize);
initLegend();

// =============================================================================
// browser debug ===============================================================
// =============================================================================
function setTestData(which) {

    setData(_debug_data_[which]);
}

if (!isQtAvailable) {

	var testButtons = [document.createElement("input"), document.createElement("input"), document.createElement("input")];
	testButtons[0].type = "button";
	testButtons[0].value = "Small Testcase";
	testButtons[0].className = "testbt";
	testButtons[0].onclick = function () { setTestData(0); };
	testButtons[1].type = "button";
	testButtons[1].value = "Large Testcase";
	testButtons[1].className = "testbt";
	testButtons[1].onclick = function () { setTestData(1); };
	testButtons[2].type = "button";
	testButtons[2].value = "__Debug__";
	testButtons[2].className = "testbt";
	testButtons[2].onclick = function () { _debug_(); };

	var pos = document.getElementById("uiContentLayout");
	pos.appendChild(testButtons[0]);
	pos.appendChild(testButtons[1]);
	pos.appendChild(testButtons[2]);

	setTestData(0);
}

function _debug_() {
}