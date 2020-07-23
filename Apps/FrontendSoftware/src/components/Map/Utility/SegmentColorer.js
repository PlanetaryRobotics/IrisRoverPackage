import * as d3 from "d3";

export const COLORS = {
  RED: "#F35B6C",
  PURPLE: "#A56DFF",
  YELLOW: "#FFC700",
  GREEN: "#21DF9A",
}

export function highlightSegment() {

  // Store state of prev route + index using closure
  var prevRoute;
  var prevIdx;
  var prevColor; 

  return {
    set: function(route, segmentIdx) {
      if (prevRoute !== undefined) {
        this.changeColor(prevRoute, prevIdx, prevColor);
      }
      this.changeColor(route, segmentIdx, COLORS.RED);
      prevRoute = route;
      prevIdx = segmentIdx;
    },
    changeColor: function(route, index, color) {  
      let container = d3.select("#"+route.routeName + "-Segment"+(index));

      // If waypoint
      if (route.segmentList[index].constructor.name === "WaypointSegment") {
        prevColor = container.select("circle").node().style.fill;

        container.select("circle").style("fill", color);
        container.select("line").style("stroke", color);

      // If circumnavigation
      } else {
        let circumnavigation = route.segmentList[index];
        prevColor = container.select("#"+route.routeName+"-CircumSegment0")
                             .select("circle").node().style.fill;
        
        for (let i = 0; i < circumnavigation.waypoints.length; i++) {
          let seg = container.select("#"+route.routeName+"-CircumSegment" + i);

          seg.select("circle").style("fill", color);
          seg.select("line").style("stroke", color);
        }
      }
    },
    removeColor: function() {
      if (prevRoute !== undefined) {
        this.changeColor(prevRoute, prevIdx, prevColor);
      }
    }
  }
}
