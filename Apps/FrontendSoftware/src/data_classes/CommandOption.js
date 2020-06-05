'use strict';
/*
 * Defines Standard Form for Data that Should be Contained by a Command Option.
 * This allows for easy and consistent formatting of Command Data.
 * A master list of CommandOptions is to be held in the CLI store.
 *
 * An example of a CommandOption is:
 * CommandOption(
    "Forward",
    "Moves the robot forward by a specified amount, with a specified speed and acceleration, after a specified delay",
    "Movement",
    [
      {name: "Distance",      units: "cm",      defaultVal: 0, type: "uint8"},
      {name: "Speed",         units: "cm/s",    defaultVal: 0, type: "uint8"},
      {name: "Acceleration",  units: "cm/s^2",  defaultVal: 0, type: "uint8"},
    ]
  )
  *
  * Author: Connor W. Colombo, CMU
  * Created: 1/25/2019
  * Last Update: 06/05/2020, Colombo
 */

export default class CommandOption{
   /* Constructs a new CommandOption from the given data.
   The format of params is an array of objects containing the elements:
   {name, defaultVal, units}.
   */
   constructor(name, desc, type, params){
     // Validate Format:
     if(arguments.length < 5){
       console.error("Too Few Arguments to CommandOption Constructor.");
     }
     for(let i in params){
       if(!(
         "name" in params[i]
         && "defaultVal" in params[i]
         && "units" in params[i]
       )){
         console.error("Invalid Format of The Following Parameter Given to CommandOption Constructor.");
         console.log(params[i]);
       }
     }

     // Everything checks out (or errors have been served), set values:
     this.name = name; //             - Command Name
     this.desc = desc; //             - Description of Command
     this.type = type; //             - Type of Command (ex. Movement, or Data Request)
     this.params = params;  //        - Parameters (arguments) which Command Takes
     this.useCount = 0; //            - Number of Times this Command Has Been Used

     // Create Cosmetically Formatted Units:
     this.formattedUnits = this.params.map( p => this.formatUnits(p.units) );
   } // ctor

   /* Returns the Given Unit String as a Cosmetically Formatted HTML.
    * Acceptable formats are:
       ... a^x b^y / c^z d^m ...
       ... a^x b^y c^-z d^-m ...
       ... a^x b^y / c^z / d^m ...
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
 } // class: CommandObject
