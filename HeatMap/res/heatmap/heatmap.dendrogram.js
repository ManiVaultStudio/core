var dendroHeight = 120;
var topMargin = 20;
var dendroWidth = 100;

var dendroScale = d3.scaleLinear();


function drawDendrogram(dendroData)
{
	if (!isDendrogramActive) return;

	if (dendroData == undefined || dendroData[0] == undefined) return;
	//log(dendroData);

	dendroScale.domain([0, Math.round(dendroData[0].dist + 0.49)])
					.range([dendroHeight, topMargin]);

	var dendroTree = d3.layout.cluster()
    .separation(function (a, b) { return 1; })
    .children(function (d) { return (d.left && d.right) ? [d.right, d.left] : undefined; })
    .size([dendroWidth, dendroHeight - topMargin]);

	dendroSvg.selectAll('*').remove();

	var nodes = dendroTree.nodes(dendroData[0]);

	var link = dendroSvg.selectAll(".dendroLink")
	 .data(dendroTree.links(nodes))
  .enter().append("path")
	 .attr("class", "dendroLink")
	 .attr("d", elbow);

	if (isShowLabelsActive) {

		var node = dendroSvg.selectAll(".dendroNode")
			 .data(nodes)
		  .enter().append("g")
			 .attr("class", "dendroNode")
			 .attr("transform", function (d) { return "translate(" + Math.round(d.x) + "," + Math.round(dendroScale(d.dist < 99999999.9 ? d.dist : 0)) + ")"; })

		node.append("text")
			 .attr("class", "name")
			 .attr("text-anchor", "middle")
			 .attr("y", -2)
			 .attr("x", function (d) { if (d.parent == undefined) { return 0 }; return (d.parent.left == d ? 20 : -20); })
			 .text(function (d) { return (d.dist < 99999999.9 ? d.dist.toFixed(2) : ""); });
	}
}

function elbow(d, i)
{
	return "M" + Math.round(d.source.x) + "," + Math.round(dendroScale(d.source.dist < 99999999.9 ? d.source.dist : 0))
		  + "H" + Math.round(d.target.x) + "V" + Math.round(dendroScale(d.target.dist < 99999999.9 ? d.target.dist : 0));
}