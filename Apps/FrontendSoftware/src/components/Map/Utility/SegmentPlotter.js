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

      // Start coords is rover position
      startX = roverXPosPx;
      startY = roverYPosPx;

      // End coords is segment coords applied to origin position
      endX = originXPosPx + coords.xPx;
      endY = originYPosPx + coords.yPx;
      angle = 0;

      segment.setPxCoordinates(endX, endY);
    
    // Handle version where PX coords are known
    } else {

      // Start coords is rover position
      startX = roverXPosPx;
      startY = roverYPosPx;

      // End coords is segment coords applied to origin position
      endX = segment.xPxCoordinate;
      endY = segment.yPxCoordinate;
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

export function plotNewSegment(container, id, index, angle, startX, startY, endX, endY, isDashed) {

  let circle = container
                .append("circle")
                .attr("id", id+"Point"+index)
                .attr("transform", `rotate(${angle} ${startX} ${startY})`)
                .attr("cx", endX)
                .attr("cy", endY)
                .attr("r", "3px")
                .style("fill", "yellow");

  let line = container.append("line")
            .attr("id", id+"Line"+index)
            .attr("transform", `rotate(${angle} ${startX} ${startY})`)
            .attr("x1", startX)
            .attr("y1", startY)
            .attr("x2", endX)
            .attr("y2", endY)
            .style("stroke", "yellow") 
            .style("stroke-width", "3px");
  
  if (isDashed) {
    line.style("stroke-dasharray", "5 3");
  }

  return getAbsoluteCoordinates(circle);
}
