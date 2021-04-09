<template>
  <div
    id="big-container"
    style="margin-top: 10%; margin-left: 20%"
  >
    <div style="padding-top: 30px; padding-bottom: 100px">
      <ErrorLogFilter :error-tags="errorTags" />
    </div>
  </div>
</template>

<script>
import ErrorLogFilter from '@/components/ErrorLog/ErrorLogFilter';

export default {
    name: 'App',
    components: {
        ErrorLogFilter,
    },
    data: function() {
        return {
            errorTags: [
                {
                    id: 'A014',
                    priority: 'Low',
                    sensors: [
                        'MotorSensor1',
                        'MotorSensor2',
                        'MotorSensor3',
                        'MotorSensor4',
                        'MotorSensor5',
                    ],
                    summary: 'Motor Overheating(2)',
                    notes:
            'temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.',
                    tags: ['#TempError'],
                    created: 'AO 10-10-2020 20:05:00',
                    lastModified: 'MO 10-10-2020 20:05:00',
                    start: '03:24:12',
                    end: '03:26:45',
                    category: 'Motor',
                },
                {
                    id: 'M001',
                    priority: 'Low',
                    sensors: [
                        'MotorSensor1',
                        'MotorSensor2',
                        'MotorSensor3',
                        'MotorSensor4',
                        'MotorSensor5',
                    ],
                    summary: 'Motor Overheating(2)',
                    notes:
            'temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.',
                    tags: ['#TempError'],
                    created: 'AO 10-10-2020 20:05:00',
                    lastModified: 'MO 10-10-2020 20:05:00',
                    start: '03:24:12',
                    end: '03:26:45',
                    category: 'Motor',
                },
                {
                    id: 'M002',
                    priority: 'Low',
                    sensors: [
                        'MotorSensor1',
                        'MotorSensor2',
                        'MotorSensor3',
                        'MotorSensor4',
                        'MotorSensor5',
                    ],
                    summary: 'Motor Overheating(2)',
                    notes:
            'temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.',
                    tags: ['#TempError'],
                    created: 'AO 10-10-2020 20:05:00',
                    lastModified: 'MO 10-10-2020 20:05:00',
                    start: '03:24:12',
                    end: '03:26:45',
                    category: 'Acceleration',
                },
                {
                    id: 'A015',
                    priority: 'Medium',
                    sensors: [
                        'MotorSensor1',
                        'MotorSensor2',
                        'MotorSensor3',
                        'MotorSensor4',
                        'MotorSensor5',
                    ],
                    summary: 'Motor Overheating(2)',
                    notes:
            'temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.',
                    tags: ['#TempError'],
                    created: 'AO 10-10-2020 20:05:00',
                    lastModified: 'MO 10-10-2020 20:05:00',
                    start: '03:24:12',
                    end: '03:26:45',
                    category: 'Power',
                },
                {
                    id: 'M003',
                    priority: 'Medium',
                    sensors: [
                        'MotorSensor1',
                        'MotorSensor2',
                        'MotorSensor3',
                        'MotorSensor4',
                        'MotorSensor5',
                    ],
                    summary: 'Motor Overheating(2)',
                    notes:
            'temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.',
                    tags: ['#TempError'],
                    created: 'AO 10-10-2020 20:05:00',
                    lastModified: 'MO 10-10-2020 20:05:00',
                    start: '03:24:12',
                    end: '03:26:45',
                    category: 'Communication',
                },
                {
                    id: 'M009',
                    priority: 'High',
                    sensors: [
                        'MotorSensor1',
                        'MotorSensor2',
                        'MotorSensor3',
                        'MotorSensor4',
                        'MotorSensor5',
                    ],
                    summary: 'Motor Overheating(2)',
                    notes:
            'temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.',
                    tags: ['#TempError'],
                    created: 'AO 10-10-2020 20:05:00',
                    lastModified: 'MO 10-10-2020 20:05:00',
                    start: '03:24:12',
                    end: '03:26:45',
                    category: 'Temperature',
                },
            ],
        };
    },
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
