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
  * Last Update: 08/29/2020, Colombo
 */

import utils from '@/utils.js';

export default class CommandOption{
    /* Constructs a new CommandOption from the given data.
   The format of params is an array of objects containing the elements:
   {name, defaultVal, units}.
   */
    constructor(name, desc, type, params){
        // Validate Format:
        if(arguments.length < 5){
            console.error('Too Few Arguments to CommandOption Constructor.');
        }
        for(let i in params){
            if(!(
                'name' in params[i]
         && 'defaultVal' in params[i]
         && 'units' in params[i]
            )){
                console.error('Invalid Format of The Following Parameter Given to CommandOption Constructor.');
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
        this.formattedUnits = this.params.map( p => utils.formatUnits(p.units) );
    } // ctor
} // class: CommandObject
