import * as d3 from "d3";
import {COLORS} from "./SegmentColorer.js";

export function generateFirstSegmentVars(route, segment, originXPosPx, originYPosPx, gridUnitCm, gridUnitPx) {

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

export function calculateCmToPxCoords(xCm, yCm, originXPosPx, originYPosPx, gridUnitCm, gridUnitPx) {
  // Convert cm to px
  let coords = convertCmToPx(xCm, yCm, gridUnitCm, gridUnitPx);

  // End coords is segment coords applied to origin position
  let endX = originXPosPx + coords.xPx;
  let endY = originYPosPx + coords.yPx;

  return {xPx: endX, yPx: endY};
}


export function calculatePxToCmCoords(xPx, yPx, originXPosPx, originYPosPx, gridUnitCm, gridUnitPx) {
  let {xCm, yCm} = convertPxToCm(xPx-originXPosPx, yPx-originYPosPx, gridUnitCm, gridUnitPx);

  return {xCm: xCm, yCm: yCm};
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
                      .style("stroke", COLORS.YELLOW) 
                      .style("stroke-width", "3px");

  createRoverAngle(container, roverAngle, endX, endY);

  let circle = container
                .append("circle")
                .attr("id", id+"Point"+index)
                .attr("transform", `rotate(${angle} ${startX} ${startY})`)
                .attr("cx", endX)
                .attr("cy", endY)
                .attr("r", "3px")
                .style("fill", COLORS.YELLOW);

  if (isDashed) {
    line.style("stroke-dasharray", "5 3");
  }

  return getAbsoluteCoordinates(circle);
}

function createRoverAngle(container, roverAngle, endX, endY) {

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
  }
}

/**
 * Updates an existing segment based on what is supplied in the coords.
 * Will take in either xCm, yCm or xPx, yPx in coords object.
 * 
 * @param {string} routeName 
 * @param {int} segmentIdx 
 * @param {object} coords 
 * @param {double} roverAngle 
 * @param {double} originXPosPx 
 * @param {double} originYPosPx 
 * @param {double} gridUnitCm 
 * @param {double} gridUnitPx 
 */
export function updateExistingSegment(routeName, segmentIdx, coords, roverAngle, originXPosPx, originYPosPx, gridUnitCm, gridUnitPx) {
  
  let endX, endY;

  if (('xCm' in coords) && ('yCm' in coords)) {
    let {xCm, yCm} = coords;

    // Convert cm to px
    let newCoords = convertCmToPx(xCm, yCm, gridUnitCm, gridUnitPx);

    // End coords is segment coords applied to origin position
    endX = originXPosPx + newCoords.xPx;
    endY = originYPosPx + newCoords.yPx;
  }
  else if ('xPx' in coords && 'yPx' in coords) {
    let {xPx, yPx} = coords;

    endX = xPx;
    endY = yPx;
  }

  let container = d3.select("#"+routeName+"-Segment"+(segmentIdx));

  // Update the start of next seg
  if (!d3.select("#"+routeName+"Line"+(segmentIdx+1)).empty()) {
    d3.select("#"+routeName+"Line"+(segmentIdx+1))
      .attr('x1', endX)
      .attr('y1', endY);
  }

  // Update the current segment
  container.select("line")
            .attr('x2', endX)
            .attr('y2', endY);

  container.select("circle")
          .attr("cx", endX)
          .attr("cy", endY);

  // If is the first segment, update the start of line too
  if (segmentIdx === 0) {
    container.select("line")
             .attr("x1", endX)
             .attr("y1", endY);
  }

  // Update rover angle
  if (roverAngle && roverAngle !== "") {
 
    if (d3.select("#"+routeName+"-Segment"+(segmentIdx) + " rect").empty()) {
      createRoverAngle(container, roverAngle, endX, endY);
    } else {
      let deg = roverAngle;
      var chairOriginX = endX + ((10) * Math.sin(0));
      var chairOriginY = endY - ((10) * Math.cos(0));

      // Square
      container.select("rect")
              .attr("x", chairOriginX - 5)
              .attr("y", chairOriginY - 2.5)
              .attr("transform", `rotate(${deg} ${endX} ${endY})`);
    }
  }
}

