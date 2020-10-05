<!-- D3 scatter -->
<template id="d3__scatter">
    <g class="points" :class="name">
        <d3__point
                v-for="(pointData, id) in activePoints"
                :series-id="seriesData.id"
                :key="id"
                :point-data="pointData"
                :layout="layout"
                :width="width"
                :scale="scale"
                :name="name"
                :safezone="safezone"></d3__point>
    </g>
</template>

<script>
    import d3__point from "./Points.vue"
    import * as d3 from "d3";

    export default {
        name: "Scatter",
        components:{
            d3__point
        },
        props: ['layout', 'series-data', 'name', 'width', 'scale','safezone'],
      computed:{
          activePoints: function() {
            return this.seriesData.values.filter((d) => d.value !== typeof null)
          }
      },
        mounted() {
            // tooltip

            const groupName = '.'+this.name+'.tooltip';
            const div = d3.selectAll(groupName);

            const layout = this.layout;
            const circles = '.'+this.name+" circle";

            // attach tooltip
            d3.selectAll(circles)
                .join("circle")
                .on("mouseover", function(d){
                    div.transition()
                        .duration(100)
                        .style("opacity", .9);
                    // div	.html(d3.timeFormat("%m/%d/%Y %H:%M")(d.Date) + "<br/>"  + "Temp : "+ d.value)
                    div.html(d.value + "&deg;C")
                        .style("left", (d3.select(this).attr("cx")) + "px")
                        .style("top", (layout.height/3) + "px");

                })
                .on("mouseout", function(){
                    div.transition()
                        .duration(250)
                        .style("opacity", 0);
                });
        },
        watch: {
            scale: {
                deep: true,
                handler: function() {}  // Has to be included for nested components watch to fire properly
            },
        }
    }
</script>

<style scoped>

</style>
