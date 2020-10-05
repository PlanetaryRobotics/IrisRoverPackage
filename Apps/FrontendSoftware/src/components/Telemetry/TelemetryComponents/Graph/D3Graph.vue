<template>
    <OuterNoTimeline
            :cData="this.Data"
            :name="name"
            :layout="layout"
            :width="width"
            :safezone="safezone"></OuterNoTimeline>
</template>

<script>
    import * as d3 from 'd3';
    import OuterNoTimeline from "./OuterNoTimeline";
    import data from "./data";

    const chartData = data;
    const columns = ['Date', 'Air_Temp'];
    export default {
        name: "D3Graph",
        components:{
            OuterNoTimeline
        },
        props:['column', 'layout','width', 'name','safezone'],
        data() {
            return {
                columns: this.column,
                Data: columns.map(function (id) {
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
                }),
            }
        },
        mounted() {
            const mleft = (this.layout.marginTop || 20) - 5;
            const groupName = '.'+this.name;
            // vertical line
            d3.selectAll(groupName).on("mousemove", function () {
                const mouse_x = d3.mouse(this)[0];
                d3.selectAll(groupName).select(".hoverline line").attr("x1", mouse_x - mleft).attr("x2", mouse_x - mleft);
                d3.selectAll(groupName).select(".hoverline").style("opacity", 1);
            })
                .on("mouseout", function () {
                    d3.selectAll(groupName).select(".hoverline").style("opacity", 1e-6);
                });
        },


    }
</script>

<style scoped>

</style>
