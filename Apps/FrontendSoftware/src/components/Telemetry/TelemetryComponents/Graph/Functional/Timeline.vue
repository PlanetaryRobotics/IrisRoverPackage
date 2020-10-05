<!-- D3 chart -->
<template id="d3__timeline">
    <div class="d3__timeline">
        <div class="text__small title">
            <span>Master Timeline</span>
        </div>
        <svg :view-box.camel="viewBox"
             preserveAspectRatio="xMidYMid meet"
             :width="layout.width"
             :height="layout.height"
             id = "timeline">
            <g class="context" :style="stageStyle">
                <d3__axis
                        v-for="(axis, id) in axes"
                        :key="id"
                        :axis="axis"
                        :layout="layout"
                        :scale="scale">
                </d3__axis>
            </g>
        </svg>
    </div>
</template>

<script>
    import data from '../data.json'
    import * as d3 from 'd3';
    import d3__axis from '../GraphComponents/Axis.vue'

    let chartData = data;
    const columns = ['Date', 'Air_Temp'];
    const cData = columns.map(function (id) {
        return {
            id: id,
            values: chartData.map(function (d) {
                return {
                    // for each data point, parse d[0] timestamp and transform it
                    Date: d3.timeParse("%m/%d/%Y %H:%M")(d.Date),
                    value: d['Air_Temp']
                }
            })
        }
    });

    export default {
        name: "Timeline",
        components:{
            d3__axis,
        },
        props: ['axes', 'layout', 'selectedRange','zoomable'],
        computed: {
            // Stage
            stageStyle: function() {
                return {
                    'transform': 'translate(' + this.layout.marginLeft + 'px,' + this.layout.marginTop + 'px)'
                }
            },
            // SVG viewbox
            viewBox: function() {
                let outerWidth = this.layout.width + this.layout.marginLeft + this.layout.marginRight,
                    outerHeight = this.layout.height + this.layout.marginTop + this.layout.marginBottom;
                return '0 0 ' + outerWidth + ' ' + outerHeight;
            },
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
                svg: null,
                focus:null,
                context:null,
            }
        },
        mounted(){
            this.timelineInit();
            this.drawTimeline();
        },
        methods: {
            // Get x-axis scale
            getScaleX: function() {
                // set time stamp
                return d3.scaleTime()
                    .range([0, this.layout.width])
                    .domain(d3.extent(chartData, (d) => d3.timeParse("%m/%d/%Y %H:%M")(d.Date)));
            },

            // Get y-axis scale
            getScaleY: function() {
                return d3.scaleLinear()
                    .range([this.layout.height, 0])
                    .domain([0, d3.max(cData, (d) => d3.max(d.values, (e) => e.value))]);
            },

            timelineInit() {
                this.svg = d3.selectAll('.dynamic');    // graphs
                this.focus = d3.selectAll(".focus")
                    .attr("transform", "translate(" + this.layout.marginLeft + "," + this.layout.marginTop + ")");
                this.timeline = d3.select('#timeline'); // timeline
                this.context = d3.select(".context")
                    .attr("transform", "translate(" + this.layout.marginLeft + "," + this.layout.marginTop + ")");
            },

            drawTimeline(){
                const width = this.svg ? +this.svg.attr("width") : this.layout.width,
                    zoomheight = this.svg ? +this.svg.attr("height") : this.layout.height,
                    height2 = this.timeline ? +this.timeline.attr("height") : this.layout.height;

                const x = d3.scaleTime()
                        .range([0, this.layout.width])
                        .domain(d3.extent(chartData, (d) => d3.timeParse("%m/%d/%Y %H:%M")(d.Date))),
                    x2 = d3.scaleTime()
                        .range([0, this.layout.width])
                        .domain(d3.extent(chartData, (d) => d3.timeParse("%m/%d/%Y %H:%M")(d.Date))),
                    y = d3.scaleLinear().range([zoomheight, 0])
                        .domain([0, d3.max(cData, (d) => d3.max(d.values, (e) => e.value))]);


                const xAxis = d3.axisBottom(x);

                if (this.zoomable === "true") {
                    const brush = d3.brushX()
                        .extent([[0, 0], [width, height2]])
                        .on("brush end", () => {
                            if (d3.event.sourceEvent && d3.event.sourceEvent.type === "zoom") {
                                return;
                            } // ignore brush-by-zoom
                            let s = d3.event.selection || x2.range();
                            x.domain(s.map(x2.invert, x2));
                            this.focus.selectAll("circle")
                                .attr("cx", (d) => {
                                    // console.log(d);
                                    return x(d.Date)
                                })
                                .attr("cy", (d) => y(d.value));

                            this.focus.select(".axis.bottom.x").call(xAxis);
                        })
                        .on("end", () => {
                            this.$emit('timelineSelection', x.domain());
                        });


                    this.context.append("g")
                        .attr("class", "brush")
                        .call(brush)
                        .call(brush.move, x.range());

                }
                this.drawError();
            },
            drawError(){
                this.timeline = d3.select('#timeline'); // timeline
                const width = +this.timeline.attr("width");
                const x = d3.scaleTime()
                        .range([0, width])
                        .domain(d3.extent(chartData, d => d3.timeParse("%m/%d/%Y %H:%M")(d.Date)));

                const list = this.selectedRange;

                const height = +this.timeline.attr("height");
                if (list) {

                    for(let i = 0; i < list.length; i++){
                        this.context.append("svg:line")
                            .attr("stroke", "red")
                            .attr("class", "tag")
                            .attr("x1", x(list[i][0]))
                            .attr("y1", height)
                            .attr("x2", x(list[i][0]))
                            .attr("y2", 0);
                    }
                }
            }
        },
        watch: {
            // Watch for layout changes
            layout: {
                deep: true,
                handler: function() {
                    this.scale.x = this.getScaleX();
                    this.scale.y = this.getScaleY();
                }
            },
            selectedRange: {
                deep: true,
                handler: function(){
                    this.drawError();
                }
            }
        }
    }
</script>

<style scoped lang="scss">
    @import '@/styles/_functional.scss';

    .d3__timeline{
        margin-left: 0;
        margin-bottom: 4px;
        margin-right: 4px;

        position: relative;
        .title {
            margin-top: 4px;
        }
    }
</style>
