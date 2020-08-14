 # Cosmos Icon Pack
 
 ## Background

 The `CosmosIconPack` allows for loading inline SVG icons on the fly from the Cosmos Icon Pack without needing to update file directories or write `fs` loads while ensuring that no icons are loaded more than once and no unused icons are loaded.
 
 ## Using Icons

 ### The Basics:
 All you need to do is `import CosmosIconPack from '@/styles/CosmosIconPack.js'`, insert it into your component's `data` function (assuming you want to use it in the `template`), then call the function inside the `v-html` of a div as follows:
 ```
<div class="class-for-styling" v-html="CosmosIconPack('nameOfIcon') />
 ```
 Then just use the class to style the `width`, `height`, and `stroke` of the icon how you want. See example below for more details.

 ### Note:
 Icon names given to the CosmosIconPack function must adhere to strict **camelCase**.
 Even if the filename isn't camelCase, its reference will be standardized. The idea here is to simply any references to icon filenames. So, an icon with any of the following file names map will map to `iconName`:
 - 'iconName.svg'
 - 'icon name.svg'
 - 'icon Name.svg'
 - 'Icon Name.svg'
 - 'icon-name.svg'
 - 'icon-Name.svg'
 - 'Icon-Name.svg'
 - 'icon_name.svg'
 - 'icon_Name.svg'
 - 'Icon_Name.svg'

 ### In a Vue Component:
 ```
    <template>
        <div class="icon" v-html="CosmosIconPack('nameOfIcon')" />
    </template>

    <script>
        import CosmosIconPack from '@/styles/CosmosIconPack.js'

        export default {
            data(){
                return {
                    CosmosIconPack, // allows access from inside Vue template (see above)
                }
            }
        }
    </script>

    <style lang="scss">
        .icon {
            width: 2rem; // Remember: these icons are flexible and will fill their container unless you specify a size
            height: 2rem;
            stroke-width: 2px; // Also, the icon stroke is arbitrary until you set it
            stroke: red; // You can set the color too!
        }
    </style>
```

 ## Adding New Icons

 New icons should be placed in the `CosmosIconPack` directory for flexible icons which can be found at `public/icons/cosmos-icons/flex`.

 For all icons in this pack to work as expected, the must be have 
 flexible size. In practice what this means is editing the svg output 
 from the creation tool (i.e. Figma) to make sure that the svg's `width` 
 and `height` are both 100%.
 Note: there should be no need for you to edit the `viewBox` property (this
 is what will keep the proportions correct.)
 So, in short, all SVG's in the flex Cosmos icon directory should start like:
 
 ```
   <svg width="100%" height="100%" viewBox="whatever-dont-change-it" ...
 ```



 **Note:** Styles applied inside the SVG will override any styles you try to apply outside of the svg (namely, `stroke` and `fill`). So, you may have to remove any `stroke` or `fill` properties from the SVG file depending on what you want to do with the icon.
 
 It's **heavily advised** that you not directly modify an existing icon to remove `stroke` or `fill` (in case some else is using it) and instead **make a copy** and rename it.

 For example, if you need to remove the `stroke` from `Cartesian Grid.svg`, make a copy and call it something like `Cartesian Grid - Strokeless.svg`, remove the `stroke` property there and access it with `CosmosIconPack('cartesianGridStrokeless')`.