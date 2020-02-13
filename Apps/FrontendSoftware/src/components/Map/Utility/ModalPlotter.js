import * as d3 from "d3";

function removeModal(id) {
  d3.select("#routeModals")
    .select("#" + id + "-" + "Modal")
    .remove();
}

function resetSegmentColor(circle, line) {
  circle.style("fill", "yellow");
  line.style("stroke", "yellow");
}

export function toggleModal(selection, route, segmentNum, pointX, pointY) {

  let id = route.routeName + "-Segment" + segmentNum;

  let dragHandler = d3.drag()
    .on("drag", function () {
        d3.select(this)
          .attr("transform", "translate(" + (d3.event.x - 100) + "," + (d3.event.y - (130/2)) + ")")
  });

  let circle = d3.select(selection).select("circle");
  let line = d3.select(selection).select("line");

  if (d3.select("#" + id + "-" + "Modal").empty()) {
    circle.style("fill", "#A56DFF");
    line.style("stroke", "#A56DFF");

    // Container
    let group = d3.select("#routeModals")
                  .append("g")
                  .attr("transform", "translate(" + (pointX + 50) + "," + (pointY) + ")")
                  .lower()
                  .attr("id", id + "-" + "Modal")
                  .on("mouseover", function(d) {
                    d3.select(this).style("cursor", "-webkit-grab");
                  });    

    dragHandler(group);

    // Rect
    group.append("svg")
          .attr("width", 200)
          .attr("height", 130)
          .attr("viewbox", "0 0 200 130")
            .append("rect")
            .attr("x", 0.5)
            .attr("y", 0.5)
            .attr("width", 199)
            .attr("height", 129)
            .attr("rx", 3.5)
            .attr("fill", "#2E2E2E")
            .attr("stroke", "#7F31FF")
    
    // Close button
    let button = group.append("g")
                      .attr("class", "closeButton")
                      .attr("transform", "translate(180, 10)")
                      .append("svg")
                      .attr("width", 10)
                      .attr("height", 11)
                      .attr("viewBox", "0 0 10 11")
                      .on("mouseover", function(d) {
                        d3.select(this).selectAll("path")
                                       .attr("stroke", "purple")
                                       .style("cursor", "pointer");
                      })                  
                      .on("mouseout", function(d) {
                        d3.select(this).selectAll("path")
                                       .attr("stroke", "white")
                                       .style("cursor", "pointer");
                      })
                      .on("click", function(d) {
                        d3.event.stopPropagation();
                        resetSegmentColor(circle, line);
                        removeModal(id);
                      })
                              
    button.append("path")
          .attr("d", "M1 1L9 10")
          .attr("stroke", "white")

    button.append("path")
          .attr("d", "M8.75555 1L1.24445 10")
          .attr("stroke", "white")

    // Route text
    group.append("g")
          .attr("transform", "translate(10, 30)")
          .append("text")
          .text(route.routeName)
          .attr("class", "text")
          .attr("fill", "white")
    
    // Segment text
    group.append("g")
          .attr("transform", "translate(10, 50)")
          .append("text")
          .text("Segment-" + segmentNum)
          .attr("class", "text__smallCaps--bold")
          .attr("fill", "white")
  }
  else {
    resetSegmentColor(circle, line);
    removeModal(id);
  }
}
