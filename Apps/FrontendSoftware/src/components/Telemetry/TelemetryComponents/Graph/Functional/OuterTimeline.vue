<template>
    <div v-on:keydown="updateZoom">
        <svg :view-box.camel="viewBox"
             preserveAspectRatio="xMidYMid meet"
             :width="layout.width"
             :height="layout.height"
             class="dynamic"
            :class="name">
            <Inner :layout="layout"
                   :chart-data="chartData"
                   :axes="axes"
                   :name="name"
                   :width="width"
                   :safezone="safezone"></Inner>
<!--            <rect class="zoom"></rect>-->
        </svg>
        <div class="tooltip" :class="name"></div>
    </div>

</template>

<script>
    import data from '../data.json'
    import Inner from "../GraphComponents/Inner";
    import * as d3 from 'd3';

    const chartData = data;
    // const columns = ['Date', 'Air_Temp'];

    export default {
        name: "Outer",
        props:['cData','name','safezone','width','layout','graphid'],
        components:{
            Inner
        },
        computed:{
            // SVG viewbox
            viewBox: function() {
                let outerWidth = this.layout.width + this.layout.marginLeft + this.layout.marginRight,
                    outerHeight = this.layout.height + this.layout.marginTop + this.layout.marginBottom;
                return '0 0 ' + outerWidth + ' ' + outerHeight;
            },
        },
        data() {
            return {
                chartData: this.cData,
                axes: ['left', 'bottom'],
                svg: null,
                focus:null,
                context:null,
                zoom:null,
                brush:null,
                x:null,
                y:null,
                x2:null,
                xAxis:null
            }
        },
        mounted(){
            this.drawChart();
        },
        methods:{
            drawChart(){
                this.svg = d3.selectAll(".dynamic"); // the svg
                this.focus = d3.selectAll(".focus");
                this.context = d3.select(".context");

                const width = +this.svg.attr("width"),
                    height = +this.svg.attr("height"),
                    height2 = +this.svg.attr("height") - this.layout.marginTop - this.layout.marginBottom,
                    mleft = this.layout.marginTop - 5;

                const x = d3.scaleTime()
                        .range([0, this.layout.width])
                        .domain(d3.extent(chartData, (d) => d3.timeParse("%m/%d/%Y %H:%M")(d.Date))),
                    x2 = d3.scaleTime()
                        .range([0, this.layout.width])
                        .domain(d3.extent(chartData, (d) => d3.timeParse("%m/%d/%Y %H:%M")(d.Date))),
                    y = d3.scaleLinear().range([height, 0])
                        .domain([0, d3.max(this.chartData, (d) => d3.max(d.values, (e) => e.value))]);

                const xAxis = d3.axisBottom(x);

                // select brush area
                const brush = d3.brushX()
                    .extent([[0, 0], [width, height2]])
                    .on("brush end", () => {
                        if (d3.event.sourceEvent && d3.event.sourceEvent.type === "zoom") {
                            return;
                        } // ignore brush-by-zoom
                        let s = d3.event.selection || x2.range();
                        x.domain(s.map(x2.invert, x2));

                        this.focus.selectAll("circle")
                            .attr("cx", (d) => x(d.Date))
                            .attr("cy", (d) => y(d.value));

                        this.focus.select(".axis.bottom.x").call(xAxis);
                        console.log(this.svg.select(".zoom"));
                        this.svg.select(".zoom").call(zoom.transform, d3.zoomIdentity
                            .scale(width / (s[1] - s[0]))
                            .translate(-s[0], 0));
                    });

                // select zoom area
                const zoom = d3.zoom()
                    .scaleExtent([1, Infinity])
                    .translateExtent([[0, 0], [width, height]])
                    .extent([[0, 0], [width, height]]);

                this.x = x;
                this.y = y;
                this.x2 = x2;
                this.xAxis = xAxis;
                this.brush = brush;
                this.zoom = zoom;

                this.svg.select(".zoom")
                    .attr('width', width)
                    .attr('height', height)
                    .attr("transform", "translate(" + this.layout.marginLeft + ',' + this.layout.marginTop + ')')
                    .call(zoom);

                this.svg.on("mousemove", function(){
                    const mouse_x = d3.mouse(this)[0];
                    d3.selectAll(".hoverline line").attr("x1", mouse_x - mleft).attr("x2", mouse_x - mleft);
                    d3.selectAll(".hoverline").style("opacity", 1);
                });

            },
            updateZoom(event) {
                if (event.keyCode === 16) {
                    console.log('triggered');
                    this.zoom.on("zoom", () => {
                        zoomed();
                    });

                    const zoomed = () => {
                        if (d3.event.sourceEvent && d3.event.sourceEvent.type === "brush") {
                            return;
                        } // ignore zoom-by-brush
                        let t = d3.event.transform;
                        this.x.domain(t.rescaleX(this.x2).domain());
                        this.focus.selectAll("circle")
                            .attr("cx", (d) => this.x(d.Date))
                            .attr("cy", (d) => this.y(d.value));
                        this.focus.select(".axis.bottom.x").call(this.xAxis);
                        this.context.select(".brush").call(this.brush.move, this.x.range().map(t.invertX, t));
                    };
                }
            },
            removeZoom() {
                this.zoom.on("zoom", () => {});
            },
        },
        created: function () {
            window.addEventListener('keydown', this.updateZoom);
            window.addEventListener('keyup', this.removeZoom);
        },
        destroyed: function () {
            window.removeEventListener('keydown', this.updateZoom);
            window.addEventListener('keyup', this.removeZoom);
        },
        watch: {
            layout: {
                deep: true,
                handler: function() {
                    this.scale.x = this.getScaleX();
                    this.scale.y = this.getScaleY();
                }
            },
        },
    }
</script>

<style>
    .zoom {
        cursor: move;
        fill: none;
        pointer-events: all;
    }
</style>
