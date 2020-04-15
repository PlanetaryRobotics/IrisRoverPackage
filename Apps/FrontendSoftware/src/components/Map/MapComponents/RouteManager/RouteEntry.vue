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
          <input :class="{'buffer': !route.segmentList.length}" ref="routeNameInput" type="text" class="route__item--name text__main" v-model="routeName">
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
      </div> <!-- END ROUTETAB -->
      
      <!-- ERROR MSG FOR DUPE ROUTE NAME -->
      <div class="error" v-if="show.errorDupeRouteName">
        This route name already exists.
      </div>
      <div class="error" v-if="show.errorEmptyRouteName">
        Route name cannot be empty.
      </div>
    </div> 
  
    <!-- ROUTEBODY -->
    <div class="routeBody">

      <div class="line" />

      <!-- SEGMENT LIST -->
      <div class="segmentList" v-show = "show.segmentList">
        <div class="segmentList__segment" v-for="(segment, index) in route.segmentList" :key="index">
          <SegmentInfo :segment = "segment" :index="index"/>
        </div>
      </div> 

    </div> <!-- END ROUTEBODY -->
  </div>  <!-- END ROUTE ENTRY -->
  
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import SegmentInfo from "@/components/Map/MapComponents/RouteManager/SegmentInfo.vue";
import Route from "@/data_classes/Route.js";
import { mapMutations, mapGetters } from 'vuex';
import $ from 'jquery';

export default {
  name: "RouteEntry",
  components: {
    SegmentInfo
  },
  props: {
    route: Route
  },
  data() {
    return {
      show: {
        segmentList: false,
        route: false,
        errorDupeRouteName: false,
        errorEmptyRouteName: false
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
        return this.routeNameDisplay;
      },
      set: function(newValue) {
        this.routeNameDisplay = newValue;

        if (newValue === "") {
          this.show.errorEmptyRouteName = true;
          return;
        } else {
          this.show.errorEmptyRouteName = false;
        }

        // renameRouteName() will handle saving newValue to the actual route obj, if valid
        if (this.route.routeName !== newValue && !this.routeList.renameRouteName(this.route.routeName, newValue)) {
          this.show.errorDupeRouteName = true;
        } else {
          this.show.errorDupeRouteName = false;
          this.$refs.routeNameInput.classList.remove("highlight");
          this.triggerRouteListUpdate(); //TODO: may need a more efficient way to do this
        }
      }
    },
  },
  mounted() {
    this.handleInputFormatting();
    this.routeNameDisplay = this.route.routeName;
  },
  methods: {
    ...mapMutations({
      triggerRouteListUpdate: 'triggerRouteListUpdate'
    }),
    toggleSegmentList() {
      this.show.segmentList = !this.show.segmentList;
    },
    toggleVisibility(route) {
      route.isVisible = !route.isVisible;
      this.triggerRouteListUpdate();
    },
    openAddModal() {
      GridEventBus.$emit("OPEN_ADD_MODAL", this.route);
    },
    handleInputFormatting() {
      let that = this;

      $(document).mouseup(function(e) {
        let container = that.$refs.routeNameInput;
        let parent = that.$refs.routeTab;

        // Note that this is will fire for EVERY route entry, so need to check 
        // that refs are not null
        if (container && parent) {
          
          // if the target is outside input 
          if (e.target !== container && !container.contains(e.target))
          {
            // Handles case when user is trying to save a routename that is already in list,
            // will force display name to be obj's route name
            if (that.routeNameDisplay !== that.route.routeName) {
              that.routeNameDisplay = that.route.routeName;
            }

            // Remove input highlight styling + error
            parent.style.border = "none";
            that.show.errorDupeRouteName = false;
            that.$refs.routeNameInput.classList.remove("highlight");

          // if target is on input
          } else {
            parent.style.border = "1px solid #A56DFF";
            container.classList.add("highlight");
          }

        }
      });
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
  margin-top: 1.5rem;
  margin-left: 2rem;
  flex-grow: 1;
  &__segment {
    width: 100%;
    margin-bottom: 2rem;
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
    }

    &--plus {
      display: flex;
      align-items: center;
      margin-right: 0.5rem;
    }
  }
}

.routeIcon {
  stroke: $color-grey-dark;
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
}

.open {
  transform: rotate(0deg);
}

</style>
