import * as d3 from "d3";

export const COLORS = {
  RED: "#F35B6C",
  PURPLE: "#A56DFF",
  YELLOW: "#FFC700",
}

export function highlightSegment() {

  // Store state of prev route + index using closure
  var prevRoute;
  var prevIdx;
  var prevColor; 

  return {
    set: function(route, segmentIdx) {
      if (prevRoute !== undefined) {
        this.changeColor(prevRoute.routeName, prevIdx, prevColor);
      }
      this.changeColor(route.routeName, segmentIdx, COLORS.RED);
      prevRoute = route;
      prevIdx = segmentIdx;
    },
    changeColor: function(name, index, color) {  
      let container = d3.select("#"+name + "-Segment"+(index));
      prevColor = container.select("circle").node().style.fill;

      container.select("circle").style("fill", color);
      container.select("line").style("stroke", color);
    },
    removeColor: function() {
      if (prevRoute !== undefined) {
        this.changeColor(prevRoute.routeName, prevIdx, prevColor);
      }
    }
  }
}
