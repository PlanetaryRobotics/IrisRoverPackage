<template>
  <div class="routeEntry">
    <div class="routeTabContainer">
      <!-- ROUTETAB -->
      <div ref="routeTab" class="routeTab"> 
        <!-- DOWN ARROW -->
        <div class="routeTab__header">
          <svg 
            @click="toggleSegmentList" width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeTab__icon" :class="{ open : show.segmentList, 'hidden': !route.segmentList.length}">
            <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
          </svg>

          <!-- ROUTE NAME --> 
          <input @focus="enableHighlight()" @blur="saveName()" :class="{'buffer': !route.segmentList.length}" ref="routeNameInput" type="text" class="route__item--name text__main" v-model="routeName">
        </div>

        <!-- PLUS ICON -->   
        <div class="route__item--plus" @click="openAddModal()">
          <svg width="17" height="17" viewBox="0 0 17 17" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeIcon selected">
            <line x1="8.5" y1="4.5" x2="8.5" y2="12.5" stroke="#333333" stroke-linecap="round" stroke-linejoin="round"/>
            <line x1="4.5" y1="8.5" x2="12.5" y2="8.5" stroke="#333333" stroke-linecap="round" stroke-linejoin="round"/>
            <circle cx="8.5" cy="8.5" r="8" stroke="#333333"/>
          </svg>
        </div>

        <!-- VISIBILITY ICON -->
        <div class="route__item--visibility" @click="toggleVisibility(route)" >
          <svg width="19" height="14" viewBox="0 0 19 14" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeIcon" :class="{selected: route.isVisible}" >
            <circle cx="9.50019" cy="6.88459" r="2.76923"/>
            <path d="M9.5 1C4.92308 1 1 4.92308 1 6.88462C1 8.84615 4.92308 12.7692 9.5 12.7692C14.0769 12.7692 18 8.84615 18 6.88462C18 4.92308 14.0769 1 9.5 1Z"/>
          </svg>
        </div>

        <!-- TRASH ICON -->
        <div class="route__item--trash" @click="toggleDeleteModal()">
          <svg class="routeIcon" width="15" height="19" viewBox="0 0 15 19" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect x="0.5" y="2.69006" width="13.2813" height="15.6615" rx="1.5" stroke-linecap="round"/>
            <line x1="3.77295" y1="5.07031" x2="3.77295" y2="15.9714" stroke-linecap="round"/>
            <line x1="7.04541" y1="5.07031" x2="7.04541" y2="15.9714" stroke-linecap="round"/>
            <line x1="1.68994" y1="0.5" x2="12.591" y2="0.5" stroke-linecap="round"/>
            <line x1="10.3184" y1="5.07031" x2="10.3184" y2="15.9714" stroke-linecap="round"/>
          </svg>
        </div>

      </div> <!-- END ROUTETAB -->
      
      <!-- ERROR MSG FOR DUPE ROUTE NAME -->
      <div ref="errorDupeRouteName" class="error">
        This route name already exists.
      </div>
      <div ref="errorEmptyRouteName" class="error">
        Route name cannot be empty.
      </div>
    </div> 
  
    <!-- ROUTEBODY -->
    <div class="routeBody">

      <div class="line" />

      <!-- SEGMENT LIST -->
      <div class="segmentList" v-show = "canShowSegmentList()">
        <!-- FOR LOOP -->
        <div class="segmentList__segment" v-for="(segment, index) in route.segmentList" :key="index">
          <!-- IF WAYPOINT -->
          <div v-if="segment.constructor.name === 'WaypointSegment'">
            <SegmentInfo :segment = "segment" :route = "route" :index="index" :POIName="null"/>
          </div>
          <!-- IF CIRCUMNAV -->
          <div v-else>
            <div class="circumnav">
              <!-- CIRCUMNAV START ROW -->
              <svg width="15" height="15" viewBox="0 0 15 15" fill="none" xmlns="http://www.w3.org/2000/svg">
                <path fill-rule="evenodd" clip-rule="evenodd" d="M13.3704 8.74926C12.793 11.4015 10.415 13.4002 7.55214 13.4002C4.68929 13.4002 2.31127 11.4015 1.73387 8.74926H0.104278C0.703168 12.2969 3.80977 15.0002 7.55214 15.0002C11.2945 15.0002 14.4011 12.2969 15 8.74926H13.3704ZM1.98239 5.41589C2.83073 3.19014 4.99857 1.6 7.55214 1.6C10.1057 1.6 12.2735 3.19014 13.1219 5.41589H14.8089C13.8995 2.28772 10.9948 0 7.55214 0C4.10943 0 1.20475 2.28772 0.295419 5.41589H1.98239ZM9.7075e-05 7.46165C3.23839e-05 7.47445 0 7.48726 0 7.50008C0 7.5129 3.23839e-05 7.52571 9.7075e-05 7.53852V7.46165Z" fill="#C4C4C4"/>
              </svg>
              
              <!-- POI NAME -->
              <div class="circumnav__label">
                {{segment.POICard.getName()}}
              </div>

              <!-- EDIT ICON -->
              <svg class="icon pencil" width="12" height="12" viewBox="0 0 12 12" fill="none" xmlns="http://www.w3.org/2000/svg">
                <path fill-rule="evenodd" clip-rule="evenodd" d="M8.84607 2.54393C8.78749 2.48536 8.69251 2.48536 8.63393 2.54393L7.82295 3.35492C7.76437 3.41349 7.76437 3.50847 7.82295 3.56705L8.43727 4.18136C8.69111 4.4352 8.69111 4.84676 8.43727 5.1006L3.60678 9.93109C3.55989 9.97798 3.4963 10.0043 3.43 10.0043H2V8.57431C2 8.50801 2.02634 8.44442 2.07322 8.39754L6.39989 4.07087C6.49752 3.97324 6.65581 3.97324 6.75344 4.07087C6.85107 4.1685 6.85107 4.32679 6.75344 4.42442L2.5 8.67787V9.50431H3.32645L8.08371 4.74705C8.14229 4.68847 8.14229 4.59349 8.08371 4.53491L7.4694 3.9206C7.21556 3.66676 7.21556 3.2552 7.4694 3.00136L8.28038 2.19038C8.53422 1.93654 8.94578 1.93654 9.19962 2.19038L9.81393 2.8047C10.0678 3.05854 10.0678 3.47009 9.81393 3.72393L9.50678 4.03109C9.40915 4.12872 9.25085 4.12872 9.15322 4.03109C9.05559 3.93346 9.05559 3.77517 9.15322 3.67754L9.46038 3.37038C9.51896 3.3118 9.51896 3.21683 9.46038 3.15825L8.84607 2.54393Z" fill="#FCFCFC"/>
              </svg>

              <!-- TRASH ICON -->
              <svg class="icon trash" width="15" height="19" viewBox="0 0 15 19" fill="none" xmlns="http://www.w3.org/2000/svg">
                <rect x="0.5" y="2.69006" width="13.2813" height="15.6615" rx="1.5" stroke-linecap="round"/>
                <line x1="3.77295" y1="5.07031" x2="3.77295" y2="15.9714" stroke-linecap="round"/>
                <line x1="7.04541" y1="5.07031" x2="7.04541" y2="15.9714" stroke-linecap="round"/>
                <line x1="1.68994" y1="0.5" x2="12.591" y2="0.5" stroke-linecap="round"/>
                <line x1="10.3184" y1="5.07031" x2="10.3184" y2="15.9714" stroke-linecap="round"/>
              </svg>

            </div>
            <div class="circumnav__segments">
              <SegmentInfo v-for="(circumnavSeg, index) in segment.waypoints" :segment = "circumnavSeg" :route = "route" :index="index" :POIName="segment.POICard.getName()" :key="index"/> 
            </div>
            <!-- CIRCUMNAV END ROW -->
            <div class="circumnav">
              <svg width="15" height="15" viewBox="0 0 15 15" fill="none" xmlns="http://www.w3.org/2000/svg">
                <path fill-rule="evenodd" clip-rule="evenodd" d="M13.3704 8.74926C12.793 11.4015 10.415 13.4002 7.55214 13.4002C4.68929 13.4002 2.31127 11.4015 1.73387 8.74926H0.104278C0.703168 12.2969 3.80977 15.0002 7.55214 15.0002C11.2945 15.0002 14.4011 12.2969 15 8.74926H13.3704ZM1.98239 5.41589C2.83073 3.19014 4.99857 1.6 7.55214 1.6C10.1057 1.6 12.2735 3.19014 13.1219 5.41589H14.8089C13.8995 2.28772 10.9948 0 7.55214 0C4.10943 0 1.20475 2.28772 0.295419 5.41589H1.98239ZM9.7075e-05 7.46165C3.23839e-05 7.47445 0 7.48726 0 7.50008C0 7.5129 3.23839e-05 7.52571 9.7075e-05 7.53852V7.46165Z" fill="#C4C4C4"/>
              </svg>
              <div class="circumnav__label">
                {{"END OF " + segment.POICard.getName()}}
              </div>
            </div>
          </div> <!-- END IF CIRCUMNAV -->
        </div> <!-- END FOR LOOP -->
      </div> 

    </div> <!-- END ROUTEBODY -->

  </div>  <!-- END ROUTE ENTRY -->
  
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import SegmentInfo from "@/components/Map/MapComponents/RouteManager/SegmentInfo.vue";
import Route from "@/data_classes/Route.js";
import { mapGetters } from 'vuex';

export default {
  name: "RouteEntry",
  components: {
    SegmentInfo,
  },
  props: {
    route: Route
  },
  data() {
    return {
      show: {
        segmentList: false,
        route: false,
        deleteModal: false,
      },
      routeNameDisplay: "",
    }
  },
  computed: {
    ...mapGetters({
      routeList: 'routeList',
    }),
    routeName: {
      get: function() {
        return this.route.routeName; // Need to force it to get from model class so when deletes happen,
                                     // right name is displayed. This means have to do manual CSS manip.
                                     // for errors in validateName().
      },
      set: function(newValue) {
        this.routeNameDisplay = newValue;
        this.validateName();
      }
    },
  },
  methods: {
    canShowSegmentList() {
      return this.show.segmentList && this.route.segmentList.length;
    },
    toggleSegmentList() {
      this.show.segmentList = !this.show.segmentList;
    },
    toggleVisibility(route) {
      route.isVisible = !route.isVisible;
      route.setVisibility();
    },
    toggleDeleteModal() {
      let html = `Are you sure you want to delete route `;
      html += `<span class='text__main--bold' style='color:red'>${this.route.routeName}</span>? This action cannot be undone.`;
      
      let payload = {
        html: html,
        deleteCallback: this.deleteRoute,
      };

      GridEventBus.$emit("TOGGLE_DELETE_MODAL", payload);
    },
    openAddModal() {
      GridEventBus.$emit("OPEN_SEGMENT_MODAL", {route: this.route, action: "ADD"});
    },
    enableHighlight() {
      let container = this.$refs.routeNameInput;
      let parent = this.$refs.routeTab;

      parent.style.border = "1px solid #A56DFF";
      container.classList.add("highlight");
    },
    disableHighlight() {
      let container = this.$refs.routeNameInput;
      let parent = this.$refs.routeTab;

      parent.style.border = "none";
      container.classList.remove("highlight");
    },
    enableError(type) {
      if (type === "EMPTY_ROUTE_NAME") {
        this.$refs.errorEmptyRouteName.style.display = "block";
      } else {
        this.$refs.errorDupeRouteName.style.display = "block";
      }
    },
    disableError(type) {
      if (type === "EMPTY_ROUTE_NAME") {
        this.$refs.errorEmptyRouteName.style.display = "none";
      } else {
        this.$refs.errorDupeRouteName.style.display = "none";
      }
    },
    validateName() {
      // Need to do manual CSS manipulation to avoid Vue from "re-getting"
      // the computed property (which will give model routeName, we just
      // want the typed route at this point).
      if (this.routeNameDisplay.length < 1) {
        this.enableError("EMPTY_ROUTE_NAME");
        return;
      } else {
        this.disableError("EMPTY_ROUTE_NAME");
      }

      if (this.routeNameDisplay !== this.route.routeName && this.routeList.hasRouteName(this.routeNameDisplay)) {
        this.enableError("DUPE_ROUTE_NAME");
      } else {
        this.disableError("DUPE_ROUTE_NAME");
      }
    },
    saveName() {
      this.disableHighlight();
      // If there are still errors, disable them, and reset the visible route name to model level's
      if (this.$refs.errorEmptyRouteName.style.display === "block" || this.$refs.errorDupeRouteName.style.display === "block") {
        this.disableError("EMPTY_ROUTE_NAME");
        this.disableError("DUPE_ROUTE_NAME");
        this.$refs.routeNameInput.value = this.route.routeName;
      } else {
        // Model level now updates with new route name
        this.routeList.renameRouteName(this.route.routeName, this.routeNameDisplay);
      }
    },
    deleteRoute() {
      this.routeList.delete(this.route);
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_mapTab.scss';

.hidden {
  display: none;
}

.buffer {
  margin-left: 1.4rem;
}

.highlight::first-line {
  background-color: $color-primary;
}

.routeTab {
  display: flex;
  flex-direction: row;
  justify-content: flex-start;
  padding: 0 1rem 0 3rem;
  margin-top: 1rem;
  
  &__header {
    display: flex;
    justify-content: flex-end;
    align-items: center;
    cursor: pointer;
    user-select: none;
    flex-grow: 1;
  }

  &__icon {
    stroke: $color-grey;
    width: 14px;
    height: 14px;
    transform: rotate(-90deg);
    transition: .1s ease-in-out;

    &:hover {
      stroke: $color-primary;
    }
  }

  &__name {
    padding-left: 0.5rem;
  }
}

.routeBody {
  display: flex;
  flex-direction: row;
  padding-left: 3.5rem;
  margin-right: 1rem;
}

.line {
  border-left: 1px solid $color-grey-dark;
  margin-top: 0.5rem;
  margin-left: 0.5rem;
}

.segmentList {
  flex-grow: 1;
  &__segment {
    width: 100%;
  }
}

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

.route {
  &__item {
    display: flex;
    align-items: center;
    justify-content: space-between;

    &--name {
      padding-left: 1rem;
      flex-grow: 1;
    }

    &--visibility {
      display: flex;
      align-items: center;
      margin-right: 0.5rem;
    }

    &--plus {
      display: flex;
      align-items: center;
      margin-right: 0.5rem;
    }

    &--trash {
      display: flex;
      align-items: center;
    }
  }
}

.routeIcon {
  stroke: white;
  transform: scale(.8);
  cursor: pointer;

  &.selected {
    stroke: $color-near-white;
    * {
      stroke: $color-near-white;
    } 
  }

  &:hover {
    > * {
      stroke: $color-primary;
    }
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


input {
  /* Tell the input to use all the available space */
  flex-grow:2;
  /* And hide the input's outline, so the form looks like the outline */
  background-color: $color-near-black;
  border: none;
  border-radius: 5px;
  color: white;
  padding: 1rem;
  font-size: 1.2rem;

  &::-webkit-input-placeholder {
    color:$color-grey-light;
  }

  &::-moz-placeholder {
    color:$color-grey-light;
  }

  &::-ms-placeholder {
    color:$color-grey-light;
  }

  &::placeholder {
    color:$color-grey-light;
  }

  &:focus {
    outline: none !important;
    border: none;
    box-shadow: none;
  }
}

.error {
  padding-left: 5.5rem;
  color: red;
  font-size: 1rem;
  display: none;
}

.open {
  transform: rotate(0deg);
}

</style>
