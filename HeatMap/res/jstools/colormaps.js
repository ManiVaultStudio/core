const _colormapsSequential = [d3.interpolateSpectral, d3.interpolateRdYlBu, d3.interpolatePiYG, d3.interpolatePuOr, d3.interpolateMagma, d3.interpolatePlasma, d3.interpolateViridis, d3.interpolateRdPu, d3.interpolateYlGnBu, d3.interpolateGnBu, d3.interpolateBuPu, d3.interpolateYlOrRd, d3.interpolateReds, d3.schemeSet3];

const _colormapNames = ['spectral', 'RdYlBu', 'PiYG', 'PuOr', 'Magma', 'Plasma', 'Viridis', 'RdPu', 'YlGnBu', 'GnBu', 'BuPu', 'YlOrRd', 'Reds', 'Qual'];

var _colormapsDiscrete =[];
_colormapsDiscrete.length = _colormapsSequential.length;

for(var i = 0; i < _colormapsDiscrete.length-1; i++)
{
    _colormapsDiscrete[i] = [];
    _colormapsDiscrete[i].length = 10;
    for(var j = 0; j < 10; j++)
    {
        _colormapsDiscrete[i][j] = _colormapsSequential[i](j/9);
    }
}
_colormapsDiscrete[_colormapsDiscrete.length-1] = d3.schemeSet3.slice(0,10);
_colormapsDiscrete[0].reverse();
_colormapsDiscrete[1].reverse();
_colormapsDiscrete[2].reverse();
_colormapsDiscrete[3].reverse();

var _activeColormap = 1;
var _isColormapDiscrete = false;
var _color = d3.scaleSequential(_colormapsSequential[_activeColormap])
					.domain([0.0,5.0]);

var _colors = [d3.scaleSequential(_colormapsSequential[_activeColormap])
					.domain([0.0,5.0]),
               d3.scaleSequential(_colormapsSequential[_activeColormap])
					.domain([0.0,5.0])];


var _numColormaps = _colormapsSequential.length;
var _colormapDefs = [];
initColormapDefinitions();

function setColormap(idx, hide) {

	_activeColormap = idx;

	updateLegendUi(hide);
     
    refreshColormap();
}

function refreshColormap()
{
    _color = activeColormapWithCustomDomain(_markerUserBounds.slice());
}

function refreshColormaps()
{
    _colors[0] = activeColormapWithCustomDomain(expressionUserBounds[activeMarker[0]].slice());
    _colors[1] = activeColormapWithCustomDomain(expressionUserBounds[activeMarker[1]].slice());
}

function activeColormapWithCustomDomain(domain) {
    
    var colormap;
    
    if(_isColormapDiscrete || _activeColormap === (_numColormaps-1))
    {
        colormap = d3.scaleQuantize()
					.range(_colormapsDiscrete[_activeColormap])
					.domain(domain);
    } else {
        
        // Reverse domain for flipped continous scales ....
        if(_activeColormap < 4) domain.reverse();
        
        colormap = d3.scaleSequential(_colormapsSequential[_activeColormap])
					.domain(domain);
    }
    return colormap;
}

// =====================================================================================
// Helpers
function initColormapDefinitions() {
    
    _colormapDefs.length = 2 * _numColormaps;
    for(var i = 0; i < _numColormaps - 1; i++) {

        // sequential
        var pct = linspace(0, 100, _colormapsDiscrete[i].length).map(function(d) { return d + '%'; });
        var colorPct = d3.zip(pct, _colormapsDiscrete[i]);

        _colormapDefs[i] = { "id": _colormapNames[i], "gradient": colorPct };

        // discrete
        pct = discspace(0, 100, _colormapsDiscrete[i].length).map(function(d) { return d + '%'; });
        colorPct = d3.zip(pct, _colormapsDiscrete[i].reduce(function(out, n){out.push(n); out.push(n); return out;},[]));

        _colormapDefs[_numColormaps + i] = { "id": _colormapNames[i] + "_discrete", "gradient": colorPct };
    }
    
    var i = _numColormaps - 1;
    
    var pct = discspace(0, 100, _colormapsDiscrete[i].length).map(function(d) { return d + '%'; });
    var colorPct = d3.zip(pct, _colormapsDiscrete[i].reduce(function(out, n){out.push(n); out.push(n); return out;},[]));

    _colormapDefs[i] = { "id": _colormapNames[i], "gradient": colorPct };
    _colormapDefs[_numColormaps + i] = { "id": _colormapNames[i] + "_discrete", "gradient": colorPct };
}

function linspace(start, end, n) {
    var out = [];
    var delta = (end - start) / (n - 1);

    var i = 0;
    while(i < (n - 1)) {
        out.push(start + (i * delta));
        i++;
    }

    out.push(end);
    return out;
}

function discspace(start, end, n) {
    var out = [];
    var delta = (end - start) / (n);

    var i = 0;
    while(i < n) {
        out.push(start + (i * delta));
        out.push(start + ((1+i) * delta));
        i++;
    }
    
    return out;
}