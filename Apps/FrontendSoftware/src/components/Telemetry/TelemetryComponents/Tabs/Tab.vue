<template>
    <div class="content animated fadeIn" v-show="isActive">
        <article :class="'message ' + color">
            <div class="message-header">
<!--                <p>{{ title }}</p>-->
            </div>
            <div class="message-body row">
                <slot></slot>
            </div>
        </article>
    </div>
</template>

<script>
    export default {
        name: "Tab",
        props:['color', 'title', 'active'],
        data() {
            return {
                isActive: false,
                iconClass: ''
            };
        },
        computed: {
            href() {
                return '#' + this.title.toLowerCase().replace(/ /g, '-');
            }
        },
        mounted() {
            this.isActive = this.selected || this.active;
            this.iconClass = this.icon
        },
        watch: {
            active: {
                deep: true,
                handler: function (val, oldVal) {
                    this.isActive = val;
                }
            },
        }
    }
</script>

<style scoped>

</style>
