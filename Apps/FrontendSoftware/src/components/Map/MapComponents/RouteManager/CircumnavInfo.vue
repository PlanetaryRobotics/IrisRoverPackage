<template>
  <div>
    <!-- CIRCUMNAV START ROW -->
    <div class="circumnav">
      <svg
        width="15"
        height="15"
        viewBox="0 0 15 15"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
      >
        <path
          fill-rule="evenodd"
          clip-rule="evenodd"
          d="M13.3704 8.74926C12.793 11.4015 10.415 13.4002 7.55214 13.4002C4.68929 13.4002 2.31127 11.4015 1.73387 8.74926H0.104278C0.703168 12.2969 3.80977 15.0002 7.55214 15.0002C11.2945 15.0002 14.4011 12.2969 15 8.74926H13.3704ZM1.98239 5.41589C2.83073 3.19014 4.99857 1.6 7.55214 1.6C10.1057 1.6 12.2735 3.19014 13.1219 5.41589H14.8089C13.8995 2.28772 10.9948 0 7.55214 0C4.10943 0 1.20475 2.28772 0.295419 5.41589H1.98239ZM9.7075e-05 7.46165C3.23839e-05 7.47445 0 7.48726 0 7.50008C0 7.5129 3.23839e-05 7.52571 9.7075e-05 7.53852V7.46165Z"
          fill="#C4C4C4"
        />
      </svg>

      <!-- POI NAME -->
      <div class="circumnav__label">
        {{ circumnavigation.POICard.getName() }}
      </div>

      <!-- EDIT ICON -->
      <svg
        class="icon pencil"
        width="12"
        height="12"
        viewBox="0 0 12 12"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
        @click.stop="openEditModal()"
      >
        <path
          fill-rule="evenodd"
          clip-rule="evenodd"
          d="M8.84607 2.54393C8.78749 2.48536 8.69251 2.48536 8.63393 2.54393L7.82295 3.35492C7.76437 3.41349 7.76437 3.50847 7.82295 3.56705L8.43727 4.18136C8.69111 4.4352 8.69111 4.84676 8.43727 5.1006L3.60678 9.93109C3.55989 9.97798 3.4963 10.0043 3.43 10.0043H2V8.57431C2 8.50801 2.02634 8.44442 2.07322 8.39754L6.39989 4.07087C6.49752 3.97324 6.65581 3.97324 6.75344 4.07087C6.85107 4.1685 6.85107 4.32679 6.75344 4.42442L2.5 8.67787V9.50431H3.32645L8.08371 4.74705C8.14229 4.68847 8.14229 4.59349 8.08371 4.53491L7.4694 3.9206C7.21556 3.66676 7.21556 3.2552 7.4694 3.00136L8.28038 2.19038C8.53422 1.93654 8.94578 1.93654 9.19962 2.19038L9.81393 2.8047C10.0678 3.05854 10.0678 3.47009 9.81393 3.72393L9.50678 4.03109C9.40915 4.12872 9.25085 4.12872 9.15322 4.03109C9.05559 3.93346 9.05559 3.77517 9.15322 3.67754L9.46038 3.37038C9.51896 3.3118 9.51896 3.21683 9.46038 3.15825L8.84607 2.54393Z"
          fill="#FCFCFC"
        />
      </svg>

      <!-- TRASH ICON -->
      <svg
        class="icon trash"
        width="15"
        height="19"
        viewBox="0 0 15 19"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
        @click.stop="openDeleteModal()"
      >
        <rect
          x="0.5"
          y="2.69006"
          width="13.2813"
          height="15.6615"
          rx="1.5"
          stroke-linecap="round"
        />
        <line
          x1="3.77295"
          y1="5.07031"
          x2="3.77295"
          y2="15.9714"
          stroke-linecap="round"
        />
        <line
          x1="7.04541"
          y1="5.07031"
          x2="7.04541"
          y2="15.9714"
          stroke-linecap="round"
        />
        <line
          x1="1.68994"
          y1="0.5"
          x2="12.591"
          y2="0.5"
          stroke-linecap="round"
        />
        <line
          x1="10.3184"
          y1="5.07031"
          x2="10.3184"
          y2="15.9714"
          stroke-linecap="round"
        />
      </svg>
    </div>
    <!-- CIRCUMNAV SEGMENTS -->
    <div class="circumnav__segments">
      <SegmentInfo
        v-for="(circumnavSeg, wpIndex) in circumnavigation.waypoints"
        :key="wpIndex"
        :segment="circumnavSeg"
        :route="route"
        :index="wpIndex"
        :p-o-i-name="circumnavigation.POICard.getName()"
        :circumnavigation="circumnavigation"
        :circumnavigation-index="index"
      />
    </div>
    <!-- CIRCUMNAV END ROW -->
    <div class="circumnav">
      <svg
        width="15"
        height="15"
        viewBox="0 0 15 15"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
      >
        <path
          fill-rule="evenodd"
          clip-rule="evenodd"
          d="M13.3704 8.74926C12.793 11.4015 10.415 13.4002 7.55214 13.4002C4.68929 13.4002 2.31127 11.4015 1.73387 8.74926H0.104278C0.703168 12.2969 3.80977 15.0002 7.55214 15.0002C11.2945 15.0002 14.4011 12.2969 15 8.74926H13.3704ZM1.98239 5.41589C2.83073 3.19014 4.99857 1.6 7.55214 1.6C10.1057 1.6 12.2735 3.19014 13.1219 5.41589H14.8089C13.8995 2.28772 10.9948 0 7.55214 0C4.10943 0 1.20475 2.28772 0.295419 5.41589H1.98239ZM9.7075e-05 7.46165C3.23839e-05 7.47445 0 7.48726 0 7.50008C0 7.5129 3.23839e-05 7.52571 9.7075e-05 7.53852V7.46165Z"
          fill="#C4C4C4"
        />
      </svg>
      <div class="circumnav__label">
        {{ "END OF " + circumnavigation.POICard.getName() }}
      </div>
    </div>
  </div>
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import Route from '@/data_classes/Route.js';
import Circumnavigation from '@/data_classes/Circumnavigation.js';
import SegmentInfo from '@/components/Map/MapComponents/RouteManager/SegmentInfo.vue';
import { highlightSegment } from '@/components/Map/Utility/SegmentColorer.js';
import { mapMutations } from 'vuex';

export default {
    name: 'CircumnavInfo',
    components: {
        SegmentInfo,
    },
    props: {
        route: Route,
        circumnavigation: Circumnavigation,
        index: Number,
    },
    data() {
        return {
            highlight: null,
        };
    },
    methods: {
        ...mapMutations(['deleteWaypoint']),
        openEditModal() {
            GridEventBus.$emit('OPEN_SEGMENT_MODAL', {
                route: this.route,
                segment: this.circumnavigation,
                segmentIndex: this.index,
                action: 'EDIT',
            });
        },
        openDeleteModal() {
            let html = '<div style=\'color:white\'>Are you sure you want to delete circumnavigation ';
            html += `<span class='text__main--bold' style='color:red'>${this.circumnavigation.POICard.getName()}</span>? This action cannot be undone.</div>`;

            this.highlight = highlightSegment();
            this.highlight.set(this.route, this.index);

            let payload = {
                html: html,
                deleteCallback: this.deleteCallback,
                cancelCallback: this.cancelCallback,
            };

            GridEventBus.$emit('TOGGLE_DELETE_MODAL', payload);
        },
        deleteCallback() {
            this.deleteWaypoint({
                route: this.route,
                segment: this.circumnavigation,
            });
        },
        cancelCallback() {
            this.highlight.removeColor();
        },
    },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";
@import "@/styles/_functional.scss";

.circumnav {
  padding: 1rem 0;
  display: flex;
  flex-direction: row;
  align-items: center;
  margin-left: 2rem;

  &__label {
    margin-left: 1rem;
    flex-grow: 1;
  }
  &__segments {
    margin-left: 2rem;
  }
}

.icon {
  width: 1.5rem;
  height: 1.5rem;
}

.pencil {
  margin-right: 0.8rem;
  > * {
    fill: white;
  }

  &:hover {
    cursor: pointer;
    > * {
      fill: $color-primary;
    }
  }
}

.trash {
  > * {
    stroke: white;
  }

  &:hover {
    cursor: pointer;
    > * {
      stroke: $color-primary;
    }
  }
}
</style>
