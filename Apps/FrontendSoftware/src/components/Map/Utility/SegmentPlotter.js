import * as d3 from "d3";

export function calculateRelativeSegmentCoordinates(segment, id, index, roverXPosPx, roverYPosPx, gridUnitCm, gridUnitPx) {
  let point = d3.select("#"+id+"Point"+index);

  let centre = getAbsoluteCoordinates(point);

  let {xCm, yCm} = convertPxToCm(centre.x-roverXPosPx, centre.y-roverYPosPx, gridUnitCm, gridUnitPx);
  segment.xCoordinate = xCm.toFixed(1);
  segment.yCoordinate = yCm.toFixed(1);
}

export function generateFirstSegmentVars(segment, roverXPosPx, roverYPosPx, originXPosPx, originYPosPx, gridUnitCm, gridUnitPx, xCmFromLander, yCmFromLander) {

  let startX, startY, endX, endY, angle;

  // Handle relative segment
  if (segment.constructor.name === "RelativeSegment") {

    // Convert cm to px 
    let coords = convertCmToPx(0, segment.distance, gridUnitCm, gridUnitPx);

    // If angle is not defined, set to 0
    if ((segment.angle && segment.angle === "-") || !segment.angle) {
      angle = 0
    } else {
      angle = segment.angle;
    }
      
    // Start coords is rover position
    startX = roverXPosPx;
    startY = roverYPosPx;

    // Compute end coords
    endX = startX;
    endY = startY + coords.yPx;

    //let coords2 = convertCmToPx(xCmFromLander, yCmFromLander, gridUnitCm, gridUnitPx);
    // Save the coords in Cm for printing in route manager
    // TODO: account for displacement of rover --> need to get the endX and endY coords AFTER FREAKING ROTATION
    // saveCoordsInCm(segment, endX-roverXPosPx, endY-roverYPosPx, gridUnitCm, gridUnitPx);

  // Handle absolute segment
  } else {

    // Convert cm to px
    let coords = convertCmToPx(segment.xCoordinate, segment.yCoordinate, gridUnitCm, gridUnitPx);

    // Start coords is rover position
    startX = roverXPosPx;
    startY = roverYPosPx;

    // End coords is segment coords applied to origin position
    endX = originXPosPx + coords.xPx;
    endY = originYPosPx + coords.yPx;
    angle = 0;

    // let opp = coords.xPx;
    // let adj = Math.sqrt(Math.pow(endX-startX, 2) + Math.pow(endY-startY, 2));
    // let relAngle = Math.atan(Math.abs(opp)/Math.abs(adj)) * (180/Math.PI);
    // segment.angle = relAngle;
    // console.log(relAngle);
  }

  return {angle: angle, startX: startX, startY: startY, endX: endX, endY: endY};
}

export function generateAppendedSegmentVars(route, segment, index, gridUnitCm, gridUnitPx, roverXPosPx, roverYPosPx, originXPosPx, originYPosPx) {

  let startX, startY, endX, endY, angle, computedAngle, lastCircle;

  // Handling for relative
  if (segment.constructor.name === "RelativeSegment") {

    let coords = convertCmToPx(0, segment.distance, gridUnitCm, gridUnitPx);

    // Get last point in route
    lastCircle = d3.select("#"+route.routeName+"-Segment"+(index))
                    .select("circle")
                  
    // Need to get the transformed coordinates of the previous circle
    let centre = getAbsoluteCoordinates(lastCircle);

    // Set start coords
    startX = Number(centre.x);
    startY = Number(centre.y);

    // Obtain sum of all prev angles in route
    let sumPrevAngles = 0;
    let list = route.segmentList.slice(0, index+1);
    list.forEach(seg => sumPrevAngles += seg.angle);
    
    // If angle is not defined, set to 0
    if ((segment.angle && segment.angle === "-") || !segment.angle) {
      angle = 0
    } else {
      angle = segment.angle;
    }

    computedAngle = sumPrevAngles + angle;

    // Set end coords
    endX = startX;
    endY = startY + coords.yPx;
  }

  // Handling for absolute
  else {
    let coords = convertCmToPx(segment.xCoordinate, segment.yCoordinate, gridUnitCm, gridUnitPx);

    // Get last point in route
    lastCircle = d3.select("#"+route.routeName+"-Segment"+(index))
                    .select("circle")
                  
    // Need to get the transformed coordinates of the previous circle
    let centre = getAbsoluteCoordinates(lastCircle);

    // Set start coords
    startX = Number(centre.x);
    startY = Number(centre.y);

    // Set end coords
    endX = originXPosPx + coords.xPx;
    endY = originYPosPx + coords.yPx;

    computedAngle = 0;

    // Compute the angle of the segment relative to the previous segment
    // tan(angle) = |m2 - m1 \ 1 + m2*m1|
    // let mPrev = (startY - roverYPosPx) / (startX - roverXPosPx);
    // let mCurr = (endY - roverYPosPx) / (endX - roverXPosPx);
    // segment.angle = Math.atan(Math.abs((mCurr - mPrev)/(1 + mCurr*mPrev))) * (180/Math.PI);
  }

  return {computedAngle: computedAngle, startX: startX, startY: startY, endX: endX, endY: endY};
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
