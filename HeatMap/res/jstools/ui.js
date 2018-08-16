var uiActive = true;

var uihov = d3.select("#uihover")
	.on("mouseover", showUiIndicator)
	.on("mouseout", hideUiIndicator);

var uiDrawer = d3.select("#ui").select("#drawer")
    .on("mousedown", toggleUi);

var spinner = document.createElement('div');
spinner.className = "spinner";

function showUiIndicator() {
	if (!uiActive) {
		d3.select("#ui").transition()
			 .duration(300)
			 .style("bottom", "-95px");
	}
}

function hideUiIndicator() {
	if (!uiActive) {
		d3.select("#ui").transition()
			 .duration(300)
			 .style("bottom", "-110px");
	}
}

function toggleUi() {

	if (uiActive) {
		hideUi();
	} else {
		showUi();
	}
}

function showUi() {
	d3.select("#ui").transition()
	  .duration(500)
	  .style("bottom", "0px");
	d3.select("#leftDrawerHandle")
	  .transition()
	  .duration(500)
	  .styleTween("-webkit-transform", function () { return d3.interpolateString("rotate(-15deg)", "rotate(0deg)"); });
	d3.select("#rightDrawerHandle").transition()
	  .duration(500)
	  .styleTween("-webkit-transform", function () { return d3.interpolateString("rotate(15deg)", "rotate(0deg)"); });

	uiActive = true;
}

function hideUi() {
	d3.select("#ui").transition()
		.duration(500)
		.style("bottom", "-110px");
	d3.select("#leftDrawerHandle")
		.transition()
	  .duration(500)
	  .styleTween("-webkit-transform", function () { return d3.interpolateString("rotate(0deg)", "rotate(-15deg)"); });
	d3.select("#rightDrawerHandle").transition()
	  .duration(500)
	  .styleTween("-webkit-transform", function () { return d3.interpolateString("rotate(0deg)", "rotate(15deg)"); });

	uiActive = false;
}

function toggleLegendSelector() {

	var pos = parseInt(d3.select("#legendWrapper").style("right"));

	if (pos > 0) {
		hideLegendSelector();
	} else {
		showLegendSelector();
	}
}

function showLegendSelector() {
	d3.select("#legendWrapper").transition()
		.duration(500)
		.style("right", "10px");

	d3.select("#legendSelector").transition()
		.duration(500)
		.style("background", "rgba(255,255,255,0.75)");

	d3.select("#legend").transition()
		.duration(500)
		.style("background", "rgba(255,255,255,0.75)");
}

function hideLegendSelector() {
	d3.select("#legendWrapper").transition()
		.duration(500)
		.style("right", "-490px");

	d3.select("#legendSelector").transition()
		.duration(500)
		.style("background", "rgba(255,255,255,0.25)");

	d3.select("#legend").transition()
		.duration(500)
		.style("background", "rgba(255,255,255,0.25)");
}

function initLegend() {

    const paletteWidth = 24;
    const paletteHeight = 168;
    const palettePadding = 10;
    
    // Active
	activePalette = d3.select("#legendActive")
        .attr("width", paletteWidth)
        .attr("height", paletteHeight)
        .append("g");
    
    activePalette.append("defs").selectAll("linearGradient")
        .data(_colormapDefs)
        .enter()
        .append("linearGradient")
        .attr("id", function(d){ return d.id })
        .attr("x1", "0%") // bottom
        .attr("y1", "100%")
        .attr("x2", "0%") // to top
        .attr("y2", "0%")
        .attr("spreadMethod", "pad")
        .selectAll("stop")
        .data(function(d){return d.gradient})
        .enter()
        .append("stop")
        .attr("offset", function(d){return d[0]})
        .attr("stop-color", function(d){return d[1]})
        .attr("stop-opacity", 1);
    
    activePalette.append("rect")
        .attr("class", "palette")
        .attr("width", paletteWidth)
        .attr("height", paletteHeight)
        .style("fill", "url(#" + _colormapNames[_activeColormap] + (_isColormapDiscrete ? "_discrete" : "") + ")");

	d3.select("#legend")
		.on("mousedown", toggleLegendSelector);
    
    // Selectors
	var allPalettes = d3.select("#legendAll")
        .attr("width", _numColormaps * (paletteWidth + palettePadding) )
        .attr("height", paletteHeight)
        .append("g");
    
    allPalettes.append("defs").selectAll("linearGradient")
        .data(_colormapDefs)
        .enter()
        .append("linearGradient")
        .attr("id", function(d){ return d.id })
        .attr("x1", "0%") // bottom
        .attr("y1", "100%")
        .attr("x2", "0%") // to top
        .attr("y2", "0%")
        .attr("spreadMethod", "pad")
        .selectAll("stop")
        .data(function(d){return d.gradient})
        .enter()
        .append("stop")
        .attr("offset", function(d){return d[0]})
        .attr("stop-color", function(d){return d[1]})
        .attr("stop-opacity", 1);
    
    allPalettes.selectAll("rect")
        .data(_colormapNames)
        .enter()
        .append("rect")
        .attr("class", "palette")
        .attr("width", paletteWidth)
        .attr("height", paletteHeight)
        .attr("transform", function(d, i){ return "translate("+ (palettePadding/2 + i * (paletteWidth + palettePadding)) +",0)" })
        .style("fill", function(d){ return "url(#" + d + (_isColormapDiscrete ? "_discrete" : "") + ")" })
        .on("mouseup", function (d, i) { leftClickColormap(i) });

	updateLegendUi(true);
}

function updateLegendUi(hide) {
    
    d3.select("#legendActive")
        .select("rect")
        .style("fill", "url(#" + _colormapNames[_activeColormap] + (_isColormapDiscrete ? "_discrete" : "") + ")");
    
    d3.select("#legendAll")
        .selectAll("rect")
        .style("fill", function(d){ return "url(#" + d + (_isColormapDiscrete ? "_discrete" : "") + ")" });

	if(hide) hideLegendSelector();
}

function printSVG() {

    if (isQtAvailable)
    {
        log(Qt);
        Qt.js_saveSVG(document.getElementById("mainView").innerHTML);
    }
}