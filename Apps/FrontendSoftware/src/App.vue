<template>
  <div style="margin-top: 10%; margin-left: 20%" id="big-container">
    <div style="padding-top: 30px; padding-bottom: 100px">
      <!-- <AccordianWithHeader :header="header" :contents="contents" /> -->

      <!--<TagSearch :validTerms = "validTerms" />  -->
      <ErrorLogFilter />
    </div>
  </div>
</template>

<script>
import ErrorLogFilter from "@/components/ErrorLog/ErrorLogFilter";
//import AccordianWithHeader from "@/components/atomic/ErrorTagAccordian";
//import TextInput from '@/components/atomic/TextInput'
//import TagSearch from '@/components/atomic/TagSearch'

export default {
  name: "App",
  components: {
    ErrorLogFilter,
    //AccordianWithHeader,
  },
  data: function () {
    return {
      validTerms: [
        "Acceleration",
        "Communication",
        "Control",
        "Motor",
        "Motor",
        "Power",
        "Temperature",
      ],
      header: "Acceleration",
      contents: [
        {
          max: 5,
          id: "A014",
          priority: 1,
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Motor",
        },
        {
          id: "M001",
          priority: 2,
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Motor",
        },
      ],
    };
  },
  methods: {},
};
</script>

<style lang="scss">
@import "/styles/_functional.scss";

body {
  background-color: $color-background;
}

p,
span {
  font-family: "Barlow";
  font-weight: 100;
  color: white;
}
</style>
