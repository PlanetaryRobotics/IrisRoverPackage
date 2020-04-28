import * as d3 from "d3";

export function calculateCmCoordinatesForSegment(segment, id, index, roverXPosPx, roverYPosPx, gridUnitCm, gridUnitPx) {
  let point = d3.select("#"+id+"Point"+index);

  let centre = getAbsoluteCoordinates(point);

  let {xCm, yCm} = convertPxToCm(centre.x-roverXPosPx, centre.y-roverYPosPx, gridUnitCm, gridUnitPx);
  segment.xCmCoordinate = xCm.toFixed(1);
  segment.yCmCoordinate = yCm.toFixed(1);
}

export function generateFirstSegmentVars(route, segment, roverXPosPx, roverYPosPx, originXPosPx, originYPosPx, gridUnitCm, gridUnitPx) {

  let startX, startY, endX, endY, angle;

  if (!route.segmentList.length) {
    // Handle version where CM coords are known
    if (segment.xCmCoordinate && segment.yCmCoordinate) {

      // Convert cm to px
      let coords = convertCmToPx(segment.xCmCoordinate, segment.yCmCoordinate, gridUnitCm, gridUnitPx);

      // End coords is segment coords applied to origin position
      endX = originXPosPx + coords.xPx;
      endY = originYPosPx + coords.yPx;

      // Start coords is same as end for very first waypoint
      startX = endX;
      startY = endY;

      angle = 0;

      segment.setPxCoordinates(endX, endY);
    
    // Handle version where PX coords are known
    } else {

      // End coords is segment coords applied to origin position
      endX = segment.xPxCoordinate;
      endY = segment.yPxCoordinate;

      // Start coords is same as end for very first waypoint
      startX = endX;
      startY = endY;
      angle = 0;

      // Compute the coords into cm and save into segment
      let {xCm, yCm} = convertPxToCm(endX-originXPosPx, endY-originYPosPx, gridUnitCm, gridUnitPx);
      segment.setCmCoordinates(xCm, yCm);
    }

  } else {

    // Get last point in route
    let index = route.segmentList.length - 1;
    let lastCircle = d3.select("#"+route.routeName+"-Segment"+(index))
                    .select("circle")
                  
    // Need to get the transformed coordinates of the previous circle
    let centre = getAbsoluteCoordinates(lastCircle);

    // Set start coords
    startX = Number(centre.x);
    startY = Number(centre.y);

    // Handle version where CM coords are known
    if (segment.xCmCoordinate && segment.yCmCoordinate) {

      // Convert cm to px
      let coords = convertCmToPx(segment.xCmCoordinate, segment.yCmCoordinate, gridUnitCm, gridUnitPx);

      // End coords is segment coords applied to origin position
      endX = originXPosPx + coords.xPx;
      endY = originYPosPx + coords.yPx;
      angle = 0;

      segment.setPxCoordinates(endX, endY);
    
    // Handle version where PX coords are known
    } else {  

      // Set end coords
      endX = segment.xPxCoordinate;
      endY = segment.yPxCoordinate;
      angle = 0;

      // Compute the coords into cm and save into segment
      let {xCm, yCm} = convertPxToCm(endX-originXPosPx, endY-originYPosPx, gridUnitCm, gridUnitPx);
      segment.setCmCoordinates(xCm, yCm);
    }
  }

  return {angle: angle, startX: startX, startY: startY, endX: endX, endY: endY};
}

/**
 * Converts a coord in cm units to pixel units
 * 
 * @param {number} xCm      Route id
 * @param {number} yCm      Starting x coord of line
 * 
 * @return {object}         {xPx: x, yPx: y}
*/
function convertCmToPx(xCm, yCm, gridUnitCm, gridUnitPx) {

  let y = (yCm / gridUnitCm) * gridUnitPx;
  let x = (xCm / gridUnitCm) * gridUnitPx;

  // Invert sign since pos value means negative translation 
  // (moving up) from origin  
  y *= -1;

  return {xPx: x, yPx: y};
}

function convertPxToCm(xPx, yPx, gridUnitCm, gridUnitPx) {

  let y = (yPx / gridUnitPx) * gridUnitCm;
  let x = (xPx / gridUnitPx) * gridUnitCm;

  y *= -1;

  return {xCm: x, yCm: y};
}

export function getAbsoluteCoordinates(point) {

  let svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
  let centre = svg.createSVGPoint();
  let bbox = point.node().getBBox();
  centre.x = bbox.x + bbox.width / 2;
  centre.y = bbox.y + bbox.height / 2;
  let matrix = point.node().transform.baseVal.consolidate().matrix;
  let transformedCentre = centre.matrixTransform(matrix);

  return transformedCentre;

}

export function plotNewSegment(container, id, index, angle, startX, startY, endX, endY, isDashed, roverAngle) {

  let line = container.append("line")
                      .attr("id", id+"Line"+index)
                      .attr("transform", `rotate(${angle} ${startX} ${startY})`)
                      .attr("x1", startX)
                      .attr("y1", startY)
                      .attr("x2", endX)
                      .attr("y2", endY)
                      .style("stroke", "yellow") 
                      .style("stroke-width", "3px");

  // function getWidth(a) {
  //   return Math.sqrt(4 * a / Math.sqrt(3));
  // }
  // function getHeight(l) {
  //   return Math.sqrt(3)*l/2;
  // }

  if (roverAngle && roverAngle !== "") {

    let deg = roverAngle;
    var chairOriginX = endX + ((10) * Math.sin(0));
    var chairOriginY = endY - ((10) * Math.cos(0));

    // Square
    container.append("rect")
              .attr("x", chairOriginX - 5)
              .attr("y", chairOriginY - 2.5)
              .attr('width', 8)
              .attr('height', 5)
              .style("stroke", "#35BAF3")
              .style("fill", "#35BAF3")
              .attr("transform", `rotate(${deg} ${endX} ${endY})`);

    // let tWidth = getWidth(100);
    // let tHeight = getHeight(100);
    // console.log(tWidth);
    // console.log(tHeight);
    // console.log(deg);
    // // Triangle
    // let arc = d3.symbol().type(d3.symbolTriangle).size(100);
    //  container.append("g")
    //           .attr("transform", `translate(${chairOriginX} ${chairOriginY}) rotate(${deg} ${endX} ${tHeight})`)
    //           .append('path')
    //           .attr('d', arc)
    //           .attr('fill', '#A56DFF')
    //           .attr("transform", `rotate(${deg} ${tWidth} ${tHeight})`);
  }

  let circle = container
                .append("circle")
                .attr("id", id+"Point"+index)
                .attr("transform", `rotate(${angle} ${startX} ${startY})`)
                .attr("cx", endX)
                .attr("cy", endY)
                .attr("r", "3px")
                .style("fill", "yellow");

  if (isDashed) {
    line.style("stroke-dasharray", "5 3");
  }

  return getAbsoluteCoordinates(circle);
}

// TODO: this needs to be updated to reflect waypoint add (AKA seg-0 does NOT come from rover pos)
export function updateExistingSegment(routeName, segmentIdx, xCm, yCm, roverAngle, originXPosPx, originYPosPx, gridUnitCm, gridUnitPx) {

  // Convert cm to px
  let coords = convertCmToPx(xCm, yCm, gridUnitCm, gridUnitPx);

  // End coords is segment coords applied to origin position
  let endX = originXPosPx + coords.xPx;
  let endY = originYPosPx + coords.yPx;

  // Update the start of next seg
  if (!d3.select("#"+routeName+"Line"+(segmentIdx+1)).empty()) {
    d3.select("#"+routeName+"Line"+(segmentIdx+1))
      .attr('x1', endX)
      .attr('y1', endY);
  }

  // Update the current segment
  d3.select("#"+routeName+"Line"+(segmentIdx))
    .attr('x2', endX)
    .attr('y2', endY);

  d3.select("#"+routeName+"Point"+(segmentIdx))
    .attr("cx", endX)
    .attr("cy", endY);
}
