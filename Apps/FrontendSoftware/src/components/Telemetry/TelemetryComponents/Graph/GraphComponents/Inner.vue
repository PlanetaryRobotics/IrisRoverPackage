<!-- D3 chart -->
<template id="d3__graph">
        <g class="focus" :style="stageStyle">
            <d3__axis
                    v-for="axis in axes"
                    :axis="axis"
                    :layout="layout"
                    :width="width"
                    :scale="scale">
            </d3__axis>
            <g class="inner">
                <d3__area
                        v-for="seriesData in chartData"
                        :scale="scale"
                        :series-data="seriesData"
                        :layout="layout"
                        :width="width"
                        :safezone="safezone">
                </d3__area>
                <rect class="zoom"></rect>
                <d3__scatter
                        v-for="seriesData in chartData"
                        :layout="layout"
                        :series-data="seriesData"
                        :scale="scale"
                        :name="name"
                        :width="width"
                        :safezone="safezone">
                </d3__scatter>
            </g>
            <g class="hoverline">
                <line stroke="darkgreen" x1="0" x2="0" y1="0" :y2="layout.height" style="opacity: 1;"></line>
            </g>
        </g>
</template>

<script>
    import data from '../data.json'
    import * as d3 from 'd3';
    import d3__axis from './Axis.vue'
    import d3__scatter from "./Scatter.vue"
    import d3__area from "./Area";

    let chartData = data;

    export default {
        name: "Inner",
        components:{
            d3__axis,
            d3__scatter,
            d3__area
        },
        props: ['axes', 'layout', 'width', 'name','chart-data','safezone'],
        computed: {
            // Stage
            stageStyle: function() {
                return {
                    'transform': 'translate(' + this.layout.marginLeft + 'px,' + this.layout.marginTop + 'px)'
                }
            }
        },
        data() {
            return {
                scale: {
                    x: this.getScaleX(),
                    y: this.getScaleY(),
                    color: d3.scaleOrdinal()
                        .range(['#159078'])
                        .domain(['Current'])
                },
            }
        },
        mounted(){
          this.clipGraph();
        },
        methods: {
            clipGraph: function(){
                /* Clip the graph */
                d3.selectAll(".inner").append("defs").append("svg:clipPath")
                    .attr("id", "clip")
                    .append("svg:rect")
                    .attr("width", this.width)
                    .attr("height", this.layout.height)
                    .attr("x", 0)
                    .attr("y", 0);
                d3.selectAll(".inner").attr("clip-path", "url(#clip)");
            },
            // Get x-axis scale
            getScaleX: function() {
                // set time stamp
                return d3.scaleTime()
                    .range([0, this.width])
                    .domain(d3.extent(chartData, (d) => d3.timeParse("%m/%d/%Y %H:%M")(d.Date)));
            },

            // Get y-axis scale
            getScaleY: function() {
                return d3.scaleLinear()
                    .range([this.layout.height, 0])
                    .domain([0, d3.max(this.chartData, (d) => d3.max(d.values, (e) =>e.value))]);
            },
        },
        watch: {
            // Watch for layout changes
            width: {
                deep: true,
                handler: function(val, oldVal) {
                    this.scale.x = this.getScaleX();
                    this.scale.y = this.getScaleY();
                }
            },
        }
    }
</script>

<style scoped>

</style>
