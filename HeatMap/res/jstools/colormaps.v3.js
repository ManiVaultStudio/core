var _colormaps = [colorbrewer.Spectral[10].slice().reverse(),
                  colorbrewer.RdYlBu[10].slice().reverse(),
                  colorbrewer.RdYlGn[10].slice().reverse(),
				  colorbrewer.PiYG[10].slice().reverse(),
				  colorbrewer.BrBG[10].slice(),
				  colorbrewer.RdPu[9].slice(),
				  colorbrewer.YlGn[9].slice(),
				  colorbrewer.YlGnBu[9].slice(),
				  colorbrewer.YlOrBr[9].slice(),
				  colorbrewer.YlOrRd[9].slice(),
				  colorbrewer.Reds[9].slice(),
				  colorbrewer.Set3[10].slice()];
_colormaps[5].shift();
_colormaps[6].shift();
_colormaps[7].shift();
_colormaps[8].shift();
_colormaps[9].shift();
_colormaps[10].shift();

const _colorMapNames = ["#Spectral", "#RdYlBu", "#RdYlGn", "#PiYG", "#BrBG", "#RdPu", "#YlGn", "#YlGnBu", "#YlOrBr", "#YlOrRd", "#Reds", "#Qual"];

var _activeColormap = 9;
var _isColormapDiscrete = false;
var _color = d3.scale.quantize()
					.range(_colormaps[_activeColormap])
					.domain([0.0,5.0]);

function setColormap(idx) {

	_activeColormap = idx;

	updateLegendUi();
	
	_color.range(_colormaps[_activeColormap]);
}

function toggleDiscreteColormap (idx) {
    
    _isColormapDiscrete = !_isColormapDiscrete;

	//updateLegendUi();
	
	//_color.range(_colormaps[_activeColormap]);
}