<template>
  <g
    :id="gID"
    @click="handleClick"
  > 
    <svg>
      <rect />
      <text />
    </svg>
  </g>
</template>

<script>
import POICard from '@/data_classes/POICard.js';
import { mapGetters } from 'vuex';
import * as d3 from 'd3';

export default {
    name: 'POIObject',
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
            return 'gID' + this.POI.uuid;
        },
        ...mapGetters(['isListeningForPOIClick']),
    },
    watch: {
    // Enable/disable mouseover effects depending on whether listening for POI click or not
        isListeningForPOIClick(isListening) {
            let svg = d3.select('#' + this.gID)
                .select('svg rect');

            if (isListening) {
                svg.on('mouseover', function () {
                    d3.select(this)
                        .style('cursor', 'pointer')
                        .style('stroke', 'white')
                        .style('stroke-width', 2);
                })
                    .on('mouseout', function () {
                        d3.select(this)
                            .style('cursor', 'default')
                            .style('stroke', 'none')
                            .style('stroke-width', 0);
                    });
            } else {
                svg.on('mouseover', null)
                    .on('mouseout', null);
            }
        }
    },
    methods: {
        handleClick() {
            // When listening for POI click (when add circumnav form is open)
            if (this.isListeningForPOIClick) {
                this.$store.commit('setTargetPOI', {POICard: this.POI, positionPx: this.positionPx});
            }
        },
        plot() {
            let width = this.POI.getData().width;
            let height = this.POI.getData().height;

            let svg = d3.select('#' + this.gID)
                .select('svg');

            svg.select('rect')
                .attr('x', this.positionPx.xPx - 0.5*width)
                .attr('y', this.positionPx.yPx - 0.5*height)
                .attr('width', width)
                .attr('height', height)
                .style('fill', this.POI.getColor());

            svg.select('text')
                .attr('x', this.positionPx.xPx - 0.5 * width)
                .attr('y', this.positionPx.yPx - 0.5 * height - 5)
                .text(this.POI.getName())
                .attr('class', 'text')
                .attr('fill', this.POI.getColor());
        },
    }
};

</script>

<style lang="scss" scoped>
</style>
