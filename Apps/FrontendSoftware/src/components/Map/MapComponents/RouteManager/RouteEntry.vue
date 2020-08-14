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
            <SegmentInfo :segment = "segment" :route = "route" :index = "index" :POIName="null" :circumnavigation="null" :circumnavigationIndex="null"/>
          </div>
          <!-- IF CIRCUMNAV -->
          <div v-else>
            <CircumnavInfo :circumnavigation = "segment" :route = "route" :index = "index" />
          </div> <!-- END IF CIRCUMNAV -->
        </div> <!-- END FOR LOOP -->
      </div> 

    </div> <!-- END ROUTEBODY -->

  </div>  <!-- END ROUTE ENTRY -->
  
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import SegmentInfo from "@/components/Map/MapComponents/RouteManager/SegmentInfo.vue";
import CircumnavInfo from "@/components/Map/MapComponents/RouteManager/CircumnavInfo.vue";
import Route from "@/data_classes/Route.js";
import { mapGetters } from 'vuex';

export default {
  name: "RouteEntry",
  components: {
    SegmentInfo,
    CircumnavInfo
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
      circumnavHighlight: null,
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
    },
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
