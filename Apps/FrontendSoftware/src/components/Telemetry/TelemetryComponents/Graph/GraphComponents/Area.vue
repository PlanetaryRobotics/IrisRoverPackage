<!-- D3 area -->
<template id="d3__area">
    <g class="safe">
        <path class="area" ref="area" :width="width" :style="style"></path>
    </g>
</template>

<script>
    import * as d3 from 'd3';
    export default {
        name: "Area",
        props: ['layout', 'series-data', 'width','scale','safezone'],
        mounted: function() {
            this.drawArea();
        },
        methods: {
            drawArea: function() {
                // Get scale
                let scale = this.scale;

                // Area object
                let area = d3.area()
                    .x(function(d) {
                        return scale.x(d.Date);
                    })
                    .y0(scale.y(this.safezone[0]))
                    .y1(scale.y(this.safezone[1]));


                // DOM node for area
                let $area = d3.select(this.$refs.area);

                $area
                    .datum(this.seriesData.values.filter(function(d) {
                        return typeof d.value !== typeof null;
                    }))
                    .attr('d', area);
            }
        },
        computed: {
            style: function() {
                return {
                    fill: this.scale.color(0),
                    fillOpacity: 0.15
                }
            }
        },
        watch: {
            // Changes to scale means we have to redraw the line!
            scale: {
                deep: true,
                handler: function(val, oldVal) {
                    this.drawArea();
                }
            },
            safezone: {
                deep: true,
                handler: function() {
                    console.log('range changed');
                    this.drawArea();
                }
            }
        }
    }
</script>

<style scoped>

</style>
