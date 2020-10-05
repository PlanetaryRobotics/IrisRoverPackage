<template>
    <div class="demo">
        <OuterTimeline
                :graphid="graphid"
                :cData="this.Data"
                :layout="layout"
                :width="layout.width"
                :safezone="safezone"
                :name="name"
                style="position:relative;"></OuterTimeline>

    </div>
</template>

<script>
    import * as d3 from 'd3';
    import OuterTimeline from "./OuterTimeline";
    import data from "../data";

    const chartData = data;
    const columns = ['Date', 'Air_Temp'];
    export default {
        name: "D3Functional",
        components:{
            OuterTimeline
        },
        props:['column', 'layout','safezone','graphid', 'name'],
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
        }

    }
</script>

<style scoped>
    .demo{
        margin-left: 0;
        position: relative;
    }

</style>
