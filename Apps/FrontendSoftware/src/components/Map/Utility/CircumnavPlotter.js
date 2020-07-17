import * as d3 from "d3";
import WaypointSegment from "@/data_classes/WaypointSegment.js";
import States from "@/data_classes/WaypointStates.js";
import {COLORS} from "./SegmentColorer.js";
import { convertPxToCm, 
         getAbsoluteCoordinates } from '@/components/Map/Utility/SegmentPlotter.js';

export function plotNewCircumNav(data, gridUnitCm, gridUnitPx) {
  // Destructure data + convert to number (since values coming from form)
  let {STARTANG, ENDANG, PHOTOS, RADIUS, ISCLOCKWISE, TARGETPOI} = data;
  STARTANG = Number(STARTANG);
  ENDANG = Number(ENDANG);
  RADIUS = Number(RADIUS);
  PHOTOS = Number(PHOTOS);

  let container;

  // If doesn't exist yet, create the container
  if (d3.select("#NewCircum").empty()) {
    container = 
      d3.select("#gridContents")
        .append('g')
        .lower()
        .attr("id", "NewCircum")
        .append('g')
        .attr("id", "NewCircum-Group");

  // Else delete container contents
  } else {
    container = d3.select("#NewCircum-Group");
    container.selectAll("*").remove();
  }

  // Compute the coords to rotate around
  let startX = TARGETPOI.positionPx.xPx; 
  let startY = TARGETPOI.positionPx.yPx;

  // Compute end coords if just straight line 
  let endX = startX;
  let endY = startY - (RADIUS / gridUnitCm) * gridUnitPx; // Basically is radius converted into px

  // Plot first point
  let absCoords = plotFirstPoint(container, "NewCircum", 0, STARTANG, startX, startY, endX, endY, States.Waypoint.UNVISITED);
  let {xCm, yCm} = convertPxToCm(absCoords.x, absCoords.y, gridUnitCm, gridUnitPx);

  // Save first waypoint coords
  let firstWaypoint = new WaypointSegment();
  firstWaypoint.setPxCoordinates(absCoords.x, absCoords.y);
  firstWaypoint.setCmCoordinates(xCm, yCm);

  // Get the degree difference
  let degIncrement = getAngleIncrement(STARTANG, ENDANG, PHOTOS, ISCLOCKWISE);

  let prevWaypoint = firstWaypoint;
  let currAng = STARTANG + degIncrement;

  for (let i = 1; i < PHOTOS; i++) {
    let nextWaypoint = plotSegment(container, "NewCircum", i, currAng, prevWaypoint, startX, startY, endX, endY, true, gridUnitCm, gridUnitPx, States.Waypoint.UNVISITED)
    currAng += degIncrement;
    prevWaypoint = nextWaypoint;
  }

  // Return new circumnav object
}

/*
 * Plots the first point in the circumnavigation. 
 */
function plotFirstPoint(container, id, index, angle, startX, startY, endX, endY, state) {
  
  let color = getColor(state);

  let circle = container
                .append("circle")
                .attr("id", id+"Point"+index)
                .attr("transform", `rotate(${angle} ${startX} ${startY})`)
                .attr("cx", endX)
                .attr("cy", endY)
                .attr("r", "3px")
                .style("fill", color);
  
  let absCoords = getAbsoluteCoordinates(circle);

  plotRoverAngle(container, angle, absCoords.x, absCoords.y);

  return absCoords;
}

/*
 * Plots the a segment in the circumnavigation (any waypoint after the first)
 */
function plotSegment(container, id, index, angle, prevWaypoint, startX, startY, endX, endY, isDashed, gridUnitCm, gridUnitPx, state) {

  let color = getColor(state);

  // Plot circle according to angle rotation, and radius (endX, endY)
  let circle = container
                .append("circle")
                .attr("id", id+"Point"+index)
                .attr("transform", `rotate(${angle} ${startX} ${startY})`)
                .attr("cx", endX)
                .attr("cy", endY)
                .attr("r", "3px")
                .style("fill", color);

  // Get absolute coords of the circle 
  let absCoords = getAbsoluteCoordinates(circle);

  // Plot rover angle
  plotRoverAngle(container, angle, absCoords.x, absCoords.y);

  // Plot the line using circle's absolute coordinates as end and prev waypoint's coordinates as start
  let line = container.append("line")
              .attr("id", id+"Line"+index)
              .attr("x1", prevWaypoint.xPxCoordinate)
              .attr("y1", prevWaypoint.yPxCoordinate)
              .attr("x2", absCoords.x)
              .attr("y2", absCoords.y)
              .style("stroke", color)
              .style("stroke-width", "3px");

  if (isDashed) {
    line.style("stroke-dasharray", "5 3");
  }

  // Compute cm values
  let {xCm, yCm} = convertPxToCm(absCoords.x, absCoords.y, gridUnitCm, gridUnitPx);

  // Save to the new waypoint
  let waypoint = new WaypointSegment();
  waypoint.setPxCoordinates(absCoords.x, absCoords.y);
  waypoint.setCmCoordinates(xCm, yCm);

  return waypoint;
}

/*
 * Plots the rover angle rect at the specified coordinate.
 */
function plotRoverAngle(container, roverAngle, endX, endY) {

  roverAngle -= 180;

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
            .attr("transform", `rotate(${roverAngle} ${endX} ${endY})`);
}

/*
 * Calculates the degree to increase each circumnav waypoint by
 */
function getAngleIncrement(startAng, endAng, numPhotos, isClockwise) {

  let diff;

  if ((isClockwise && startAng < endAng) || (!isClockwise && startAng > endAng)) {
    diff = endAng - startAng;
  } else {
    diff = endAng + startAng;
  }

  let deg = diff / (numPhotos - 1);

  if (!isClockwise) {
    deg *= -1;
  }

  return deg;
}

/*
 * Helper to get right color depending on waypoint state
 */
function getColor(state) {
  if (state === States.Waypoint.UNVISITED) {
    return COLORS.YELLOW;
  }
  else if (state === States.Waypoint.VISITED) {
    return COLORS.GREEN;
  }
}
