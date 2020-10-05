<template>
    <div class="safezone">
        <label for="min">Minimum</label>
        <div class="field">
            <span class="field-value" v-show="!showField('min')" @click="focusField('min')">{{range.min}}</span>
            <input v-model="range.min" v-show="showField('min')" id="min" type="text" class="field-value form-control" @focus="focusField('min')" @blur="blurField">
        </div>

        <label for="max">Maximum</label>
        <div class="field">
            <span class="field-value" v-show="!showField('max')" @click="focusField('max')">{{range.max}}</span>
            <input v-model="range.max" v-show="showField('max')" type="max" class="field-value form-control" @focus="focusField('max')" @blur="blurField">
        </div>
    </div>
</template>

<script>
    export default {
        name: "SafeZone",
        props:['safezone'],
        data(){
            return {
                range : {
                    min: this.safezone[0],
                    max: this.safezone[1]
                },
                editField : ''
            }
        },
        methods : {
            focusField(val){
                this.editField = val;
                // console.log("child says: "+val);
                // this.$emit('min');
            },
            blurField(){
                this.editField = '';
                this.$emit('editRange',this.range);
            },
            showField(val){
                return (this.range[val] == '' || this.editField == val)
            }
        }
    }
</script>

<style scoped>
    .safezone{
        padding: 15px 15px 15px 15px;
    }
</style>
