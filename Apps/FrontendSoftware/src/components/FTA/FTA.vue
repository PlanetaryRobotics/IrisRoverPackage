
<template>
  <div id="ftaContainer" ref="container" class="fta-container"></div>
</template>

<script>
const d3 = require('d3');
import d3_sankey from './sankey.js'

import graph from './sankey.json'

export default {
    name: 'FTA',
    data(){
      return {
        resizeTransitionTime: 100, // [ms] Duration of resizing transitions
        nodeWidth: 36,
        nodePadding: 40,
        layoutSize: 32,
        colors: ["#00FF00", "#FFFF00", "#FF0000", "#00000000"],
        units: "Widgets",
        svg: {},
        path: {},
        sankey: {},
        link: {},
        node: {}, // all nodes (each is a collection of node bodies)
        nodeBodies: {}, // all node bodies (eg. rectangles)
        nodeLabels: {}, // all text labels
        width: 1000,
        height: 700,
        resizeObserver: {}
      }
    },
    mounted(){
      // append the svg object to the body of the page
      this.svg = d3.select("#ftaContainer")
                   .append("svg")
                   .append("g")
                   .attr("transform", "translate(0,0)");

      // Set the sankey diagram properties
      this.sankey = d3_sankey()
        .nodeWidth(this.nodeWidth)
        .nodePadding(this.nodePadding)
        .size([this.width, this.height]);

      this.path = this.sankey.link();

      // load the data
      this.jsonUpdateHandler(graph);

      // Watch for / react to size changes:
      this.resizeObserver = new ResizeObserver(() => this.updateSize());
      this.resizeObserver.observe(this.$refs.container);
    },
    beforeDestroy(){
      this.resizeObserver.disconnect();
    },
    methods:{
      // Forces the Sankey plot to resize to fit the container and re-layout accordingly.
      updateSize(){
        this.resize(this.$refs.container.clientWidth, this.$refs.container.clientHeight-this.nodePadding)
      },

      /**
       * Resizes the Sankey plot to the given size and smoothly transitions from current layout to newly 
       * required layout.
       * 
       * As a result, this is also where all layout positioning code is handled.
       */
      resize(width, height){
        this.width = width;
        this.height = height;
        this.sankey.size([width, height]).layout(this.layoutSize);
        this.link.transition().duration(this.resizeTransitionTime)
          .attr('d', this.path);
        this.node.transition().duration(this.resizeTransitionTime)
          .attr("transform", d => `translate(${d.x},${d.y})`);
        this.nodeBodies.transition().duration(this.resizeTransitionTime)
          .attr("height", d => Math.max(4, d.dy) )
          .attr("width", this.sankey.nodeWidth());
        this.nodeLabels.transition().duration(this.resizeTransitionTime)
          .attr("x", -6)
          .attr("y", d => d.dy / 2 )
          .attr("text-anchor", "end")
          .filter(d => d.x < this.width / 2)
            .attr("x", 6 + this.sankey.nodeWidth())
            .attr("text-anchor", "start");
      },

      format(d) {
        const formatNumber = d3.format(",.0f"); // zero decimal places
        return formatNumber(d) + " " + this.units;
      },

      dragmove(target, d) {
        d3.select(target)
          .attr("transform", 
                "translate(" 
                  + d.x + "," 
                  + (d.y = Math.max(
                      0, Math.min(this.height - d.dy, d3.event.y))
                    )
                + ")"
          );
        this.sankey.relayout();
        this.link.attr("d", this.path);
      },

      jsonUpdateHandler(graph){
        this.sankey
            .nodes(graph.nodes)
            .links(graph.links)
            .layout(this.layoutSize);

        // add in the links
        this.link = this.svg.append("g").selectAll(".link")
                            .data(graph.links)
                            .enter().append("path")
                            .attr("class", "link")
                            .style("stroke-width", d => Math.max(1, d.dy) )
                            .style("stroke", d => this.colors[d.colorV] )
                            .sort( (a,b) => b.dy - a.dy );

        // add the link titles
        this.link.append("title")
            .text(d => {
              return `${d.source.name} → ${d.target.name}`;
            });

        // add in the nodes
        const dragMoveRef = this.dragmove; // Have to use an external ref bc d3-sankey needs to rebind `this` in its drag call function
        this.node = this.svg.append("g").selectAll(".node")
            .data(graph.nodes)
            .enter().append("g")
            .attr("class", "node")
            .call(
              d3.drag()
              .subject(d => d)
              .on("start", function(){ this.parentNode.appendChild(this); })
              .on("drag", function(d){ dragMoveRef(this, d) })
            );

        // add the rectangles for the nodes
        this.nodeBodies = this.node.append("rect");
        this.nodeBodies
            .style("fill", d => (d.color = this.colors[d.colorV]) ) //color(d.name.replace(/ .*/, "")); })
            .style("stroke", d => d3.rgb(d.color).darker(2) )
            .append("title")
            .text( d => `${d.name}` );

        // add in the title for the nodes
        this.nodeLabels = this.node.append("text");
        this.nodeLabels
            .attr("dy", "0.35rem")
            .attr("transform", null)
            .text(d => d.name);

        // Force a size update and new layout generation after JSON data changes:
        this.updateSize();
      }
    }
}
</script>

<style lang="scss">
  @import '@/styles/_colors.scss';
  @import '@/styles/_dimensions.scss';
  @import '@/styles/_functional.scss';

  .fta-container {
    width: 100%;
    height: 100%;

    background: $color-background;

    padding: 1rem;
    user-select: none;

    & > svg {
      width: 100%;
      height: 100%;
    }

    & .node > rect {
      cursor: move;
      fill-opacity: .9;
      shape-rendering: crispEdges;
    }

    & .node > text {
      pointer-events: none;
      fill: #fff;
      text-shadow: 0 1px 0 #000;
    }

    & .link {
      fill: none;
      stroke: #000;
      stroke-opacity: .2;

      &:hover {
        stroke-opacity: .5;
      }
    }
  }


</style>