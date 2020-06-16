<template>
  <g :id="gID"> 
  </g>
</template>

<script>
import POICard from "@/data_classes/POICard.js";
import * as d3 from "d3";

export default {
  name: "POIObject",
  props: {
    POI: POICard,
    positionPx: Object,
  },
  mounted() {
    this.plot();
  },
  computed: {
    gID() {
      // Need to prefix UUID with a string in case starts with number (not valid selector if so)
      return "gID" + this.POI.uuid;
    }
  },
  methods: {
    plot() {
      let width = this.POI.getData().width;
      let height = this.POI.getData().height;

      let svg = d3.select("#" + this.gID)
                  .append("svg");

      svg.append("rect")
        .attr("x", this.positionPx.xPx - 0.5*width)
        .attr("y", this.positionPx.yPx - 0.5*height)
        .attr("width", width)
        .attr("height", height)
        .style("fill", this.POI.getColor());

      svg.append("text")
        .attr("x", this.positionPx.xPx - 0.5 * width)
        .attr("y", this.positionPx.yPx - 0.5 * height - 5)
        .text(this.POI.getName())
        .attr("class", "text")
        .attr("fill", this.POI.getColor())
    },
  }
}

</script>

<style lang="scss" scoped>
</style>
