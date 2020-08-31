'use strict'
/*
 * A collection of small generally useful utility functions.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/29/2020
 * Last Update: 08/30/2020, Colombo
 */

 import { lookup as mimeLookup } from 'mime-types'
 import FileType from 'file-type'

 const fs = require('fs');
 const nodeUtil = require('util');

 export default {
    /** 
     * Throws an error with the given name. When used a used as a default value for a 
     * parameter in destructuring, this has the effect of requiring the given parameter 
     * by throwing an error if the default is used.
     * Usage:
     * ```
     * function func({
     *  someParam = "default value",
     *  someRequiredParam = requireParam('someRequiredParam') // throws an error if a value is not given
     * } = {}){
     *  // Get funcy
     * }
     * ```
     * 
     * @param {String} name Name of the Parameter being required
     */
    requireParam(name){
        throw new Error(`Missing Parameter ${name}`);
    },

    /**
     * Promise-wrapped version of Node's `readFile` which is compatible with async/await.
     * @see fs.readFile in Node for args and returns.
     */
    readFilePromise: nodeUtil.promisify(fs.readFile),

    /**
     * Synchronously loads the given file and returns it as a Buffer of bytes.
     * 
     * @param {(string|Buffer)} file URI of File or Buffer preloaded from fs.
     */
    async readFileAsBytes(file){
        let data;
        if(file instanceof Buffer){
            data = file;
        } else{
            data = await this.readFilePromise(file);
        }
        return data;
    },

    /**
     * Synchronously loads the given image file and returns it as a Base64 URL.
     * 
     * @param {(string|Buffer)} file URI of File or Buffer preloaded from fs.
     */
    async readImageAsBase64URL(file){
        let mime, str;
        if(file instanceof Buffer){
            mime = (await FileType.fromBuffer(file)).mime;
            str = file.toString('base64');
        } else {
            mime = mimeLookup(file);
            str = await this.readFilePromise(file, {encoding: 'base64'});
        }

        if(mime && mime.split('/')[0] === 'image'){
            return `data:${mime};base64,${str}`;
        } else {
            return "";
        }
    },

    /** 
     * Formats the Given Unit String as a Cosmetically Formatted HTML.
     * Acceptable formats are:
        ... a^x b^y / c^z d^m ...
        ... a^x b^y c^-z d^-m ...
        ... a^x b^y / c^z / d^m ...
     * @param {string} str Units String
     * @returns String formatted as HTML.
    */
    formatUnits(str){
    // Insert Special Unit Symbols:
    str = str.replace("deg", "&deg;");
    str = str.replace("ohm", "&Omega;");
    // Separate Numerator and Denominator:
    let ds = str.split('/'); // Slash Delimited Strings
    let ufn = ds[0]; // UnFormatted Numerator
    let ufd = ""; // UnFormatted Denominator
    if(ds.length > 1){
        for(let i=1; i<ds.length; i++){ // Support multiple slashes
        ufd += ds[i];
        }
    }

    let num = "";
    let denom = "";

    // Format Powers of Each Term in Numerator:
    ufn.split(' ').forEach( term => { // Isolate each term (ex. "m" or "s^-2")
        if(term !== ""){
        let pcs = term.split('^'); // Split term into pieces
        if( (pcs[1]||"").includes('-') ){ // This term should actually be in the denominator
            pcs[1] = pcs[1].replace('-',"");
            denom += `${pcs[0]}<sup>${pcs[1]||""}</sup>`;
        } else{
            num += `${pcs[0]}<sup>${pcs[1]||""}</sup>`;
        }
        }
    });

    // Format Powers of Each Term in Denominator (if there is a denominator):
    if(ufd){
        ufd.split(' ').forEach( term => { // Isolate each term (ex. "m" or "s^-2")
        let pcs = term.split('^'); // Split term into pieces
        denom += `${pcs[0]}<sup>${pcs[1]||""}</sup>`;
        });
    }

    if(num == ""){
        num = "&nbsp;<sup></sup>";
    }

    return "&hairsp;" + (denom ? `<span style="font-size:0.75em;"><sup>${num}</sup>/<sub>${denom}</sub></span>` : `<span style="font-size:0.95em">${num}</span>`);
    }
 };