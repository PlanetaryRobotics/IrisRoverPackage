'use strict';
/**
 * Allows for loading inline SVG Icons on the fly from the Cosmos Icon Pack
 * without needing to update file directories or write `fs` loads while 
 * ensuring that no icons are loaded more than once and no unused icons are 
 * loaded.
 * 
 * At first run, this script catalogs all existing SVG icons in the Flexible 
 * Cosmos Icon Directory (defined in settings below), standardizes their Names, 
 * and store their addresses in the `iconRegistry`.
 * 
 * Then, this script returns a function which, when called and given , checks to 
 * see if the icon exists by consulting the `iconRegistry` and if it exists either
 * returns the icon from the `iconCache` or loads it into the `iconCache` then 
 * returns it.
 * 
 *  ** Icon names given to the CosmosIconPack function must adhere to strict 
 *  camelCase. Even if the filename isn't camelCase, its reference will be 
 *  standardized. **
 * 
 * The objective of this package is to simplify the requirement and 
 * usage of Cosmos icons and prevent any icon which is used in multiple
 * places from having to be loaded multiple times. In addition, this 
 * allows for new icons to be easily added and for the storage scheme 
 * of cosmos icons to be easily changed in the future by eliminating 
 * direct path references to the icons.
 * 
 * Standards:
 * ----
 * For all icons in this pack to work as expected, the must be have 
 * flexible size. In practice what this means is editing the svg output 
 * from the creation tool (i.e. Figma) to make sure that the svg's `width` 
 * and `height` are both 100%.
 * Note: there should be no need for you to edit the `viewBox` property (this
 * is what will keep the proportions correct.)
 * So, in short, all SVG's in the flex Cosmos icon directory should start like:
 * 
   <svg width="100%" height="100%" viewBox="whatever-dont-change-it" ...
 * 
 * Usage:
 * ----
 * In a Vue Component:
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
 * 
 * Author: Connor W. Colombo, CMU
 * Created: 08/01/2020
 * Last Update: 08/02/2020, Colombo
 */

/* global __static */ // <- keep eslint from complaining about the __static directory
 const fs = require('fs');
 const path = require('path');

 //
 // SETTINGS:
 //
 const dir = './icons/cosmos-icons/flex'; // Directory containing Cosmos icons relative to the public (__static) directory
 const ext = 'svg'; // File extension of the icons
 const missingName = 'missingIcon'; // Name of the placeholder icon used when the requested icon is invalid.


 //
 // SETUP:
 //
 const dirpath = path.join(__static, dir);
 
 // Map of the properly formatted iconNames of all existing icons to their corresponding file addresses:
 const iconRegistry = new Map();
 // Icon Cache in the form of a Map from iconNames to inline SVG content for all existing icons which have been requested at least once:
 const iconCache = new Map();


 //
 // FUNCTIONALITY:
 //
 // Grab all files with ext in dir:
 const files = fs.readdirSync(dirpath).filter( f => f.match(new RegExp(`.${ext}$`)) );

 // Catalog All Existing Icons, Standardize their Names, and Store their Addresses:
 files.forEach( fn => {
    const name = formatName(fn);
    // Check for possible name collisions:
    if(iconRegistry.get(name)){
        // report name collision
        console.error(`
            [COSMOS-ICON-PACK] 
            Icon name collision. Both '${fn}' and '${iconRegistry.get(name)}' map 
            to '${name}' in camelCase. Check if these are really the same icon 
            and one needs to be deleted or if they're different and one needs 
            to be renamed.
        `);
    } else {
        // .. if none, register the icon.
        iconRegistry.set(name, fn);
    }
 });

 /**
  * Creates a Properly Formatted camelCase Name from the Given Filename.
  * Splits on spaces, -, --, _, and allows for names that are already camel 
  * case or contain some camelCase.
  */ 
 function formatName(filename){
    let name = filename.replace(new RegExp(`(./|.${ext})`), '');
    let words = name.split(/[\s-_]+/);
    words = words.map( (word, i) => word.charAt(0)[i ? 'toUpperCase' : 'toLowerCase']() + word.slice(1) );
    return words.join('');
 }

 /**
  * Checks to see if the requested icon is in the cache and, if it is, returns it.
  * If not, it loads it, caches it, and returns it.
  * 
  * NOTE: For reactivity reasons, this function will get called by Vue a lot and in 
  * the very vast majority of cases it will be for rerequests of the same icons; so, 
  * MAKE SURE the path that serves up cached icons is as fast as reasonably possible, 
  * even if at the slight expense of other paths.
  * Some priority should also be given to ensuring that the return path for the missing 
  * icon uses proper caching and is not prohibitively expensive (so the interface 
  * doesn't grind to a halt if a dev names an icon incorrectly).
  */
 function cachedLoad(iconName) {
    // Check to see if this icon has already been requested and cached:
    if(iconCache.has(iconName)){
        // ... if so, return it:
        return iconCache.get(iconName);
    } else {
        // ... not, find out why:
        // Check whether the requested icon exists; if not, return the missingIcon instead.
        if(!iconRegistry.has(iconName)){
            console.error(`
                [COSMOS-ICON-PACK] 
                Requested icon "${iconName}" does not exist in the CosmosIconPack. 
                Ensure the icon is an SVG and exists in Cosmos Flex Icon directory, 
                which on this machine can be found at:\n ${dirpath} .
            `);
            if(iconName !== missingName){
                return cachedLoad(missingName); // Display the missing icon instead.
            } else{ 
                console.error(`
                    [COSMOS-ICON-PACK] 
                    Missing icon couldn't be located in the directory. Ensure the 
                    directory is in the right place. Icons should be located at 
                    public/${dir} which can  be found on this machine at:\n 
                    ${dirpath} .
                `);
            }
        }

        // If not missing from the registry, Load it, cache it, then return it:
        const addr = iconRegistry.get(iconName);
        let icon;
        try {
            icon = fs.readFileSync(path.join(dirpath, addr), 'utf8');
            iconCache.set(iconName, icon);
        } catch(e){
            console.error(`
                [COSMOS-ICON-PACK] 
                Requested icon "${iconName}" exists in the iconRegistry but couldn't 
                be loaded. Check for data corruption.
            `);
            if(iconName !== missingName){
                return cachedLoad(missingName);
            } else{ 
                console.error(`
                    [COSMOS-ICON-PACK] 
                    Missing icon failed to load. Check for file- or even 
                    directory-wide data corruption.
                `);
                icon = `<svg>MISSING ICON IS MISSING</svg>`;
            }
        }
        return icon;
    }
 }


 export default cachedLoad;
