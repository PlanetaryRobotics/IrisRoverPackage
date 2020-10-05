<!-- D3 points -->
<template id="d3__point">
    <circle class="point" ref="point" :style="style"></circle>
</template>

<script>
    import * as d3 from 'd3';

    export default {
        name: "Points",
        props: ['layout', 'point-data', 'scale','name','width', 'series-id','safezone'],
        mounted: function() {
            this.drawPoint();
        },
        methods: {
            drawPoint: function() {
                const low = this.safezone[0], high = this.safezone[1];
                // Get props
                const scale = this.scale;

                // DOM node for points
                let $point = d3.select(this.$refs.point);
                $point
                    .datum(this.pointData)
                    .attr('cx', function(d) {
                        return scale.x(d.Date);
                    })
                    .attr('cy', function(d) {
                        return scale.y(d.value);
                    })
                    .attr('r', 4)
                    .style("fill", function(d){
                        return (d.value > high || d.value < low) ? '#f00' : '#fff';
                    });
            }
        },
        computed: {
            style: function() {
                return {
                    fill: '#fff',
                }
            }
        },
        watch: {
            scale: {
                deep: true,
                handler: function() {
                    this.drawPoint();
                }
            },
            safezone: {
                deep: true,
                handler: function() {
                    console.log('range changed');
                    this.drawPoint();
                }
            }
        }
    }
</script>

<style>

    .tooltip {
        position: absolute;
        text-align: center;
        width: 55px;
        opacity: 1e-9;
        height: 20px;
        padding: 3px;
        font: 12px sans-serif;
        background: seagreen;
        border: 0;
        border-radius: 2px;
        pointer-events: none;
    }

</style>
