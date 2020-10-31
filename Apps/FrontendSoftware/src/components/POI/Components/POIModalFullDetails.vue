<template>
  <div class="modal-backdrop">
    <div class="POIModalDetailsContainer">
        <section class="modal-row">
            Priority
            <select>
              <option disabled value=""><img src="@/assets/imgviewer/two_priority.svg"> Select</option>
              <option>Low</option>
              <option>Medium</option>
              <option>High</option>
            </select>
            Type
        </section>
        <section class="modal-row">
            Name*
        </section>
        <section class="modal-row">
            Size
            Width x Depth x Height
        </section>
        <section class="modal-row">
            Details
        </section>
    </div>
  </div>
</template>

<script>
export default {
  name: "POIModalFullDetails",
};
</script>


<style>
.modal-backdrop {
  position: fixed;
  top: 0;
  bottom: 0;
  left: 0;
  right: 0;
  background-color: rgba(0, 0, 0, 0.8);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 2;
}

/* REM conversion: 1rem = 10px */
.POIModalDetailsContainer {
  background: #424242;
  overflow-x: auto;
  position: fixed;
  display: flex;
  width: 52.6rem; /* 120px/16px (per 1rem) = 7.5 rem; */
  flex-direction: column;
  border-radius: 0.4rem;

  font-size: 10px;
  font-style: normal;
  font-weight: 400;
  text-align: left;
  z-index: 2;
}

.modal-row {
  padding: 10px 12px;
}

.dot-icon {
  width: 0.8rem;
  height: 0.8rem;
  top: 0.2rem;
  display: inline-block;
  margin-right: 0.8rem;
  margin-top: 0.1rem;
  border-radius: 50%;
}

.dot-icon.red {
  background-color: #ff2f46;
}

.dot-icon.green {
  background-color: #21df9a;
}
</style>