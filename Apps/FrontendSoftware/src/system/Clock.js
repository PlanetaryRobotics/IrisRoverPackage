'use strict';
/**
 * Defines the reference for a time system where all time is measured against a reference 'zeroTime'.
 * Includes some convenient handling functions but can also just be used as a string printer.
 * 
 * Most basic usage:
 ```
    const clock = new Clock();
    console.log(clock); // Logs the current time as a formatted string.
 ```
 * 
 * Author: Connor W. Colombo, CMU
 * Created: 08/07/2020
 * Last Update: 08/13/2020, Colombo
 */

const { DateTime } = require('luxon');

const { EventEmitter } = require('events'); // Different than Vue eventHub (since Clock exists outside of Vue renderer)
/* Events List:
  - tick: Called once every second. Used to update clock displays. Should not be used to time general events because the 
          time interval is not exact and because that's bad design and there's probably a better way to what you're trying.
          Arguments to callback:
        {
            // none
        }
*/

/**
 * Returns a luxon DateTime object constructed from the given json string
 * if the given json string is valid.
 */
function luxonFromJSON(json){
    return json ? DateTime.fromISO(json) : json;
}

export default class Clock{
    /*
    * NOTE: instance eventBuses are a bad idea for objects which may be regularly garbage 
    * collected (such as this which goes inside a `SystemData` `DBObject`) since:
    *   1. Any `interval` timers which emit events may prevent instances from getting garbage
    *      collected.
    *   2. If a `DBObject` is getting destroyed and replaced by another with new data and, thus,
    *      a new `eventBus`, all subscribers would have to be transferred from one to the other,
    *      which would be, at minimum, a great pain and require overhauling all `DBObject` 
    *      constructors and, by extension, `DBLazyList` to support event subscriber handover, which
    *      would likely bring along with it a not insignificant performance deficit for object loading.
    */
    static #eventBus = new EventEmitter();
    // Handle for the Node timer driving the tick events on the eventBus (generates a tick for all Clock subscribers):
    static #pendulum = setInterval(() => Clock.#eventBus.emit('tick'), 1000);

    #empty;

    constructor(args){
        const {
            code,
            name = code,
            zeroTime = DateTime.utc(),
            toStringUseStamp = false
        } = (args || {});

        this.code = code; //                                - Shorthand codename / abbreviation (usually one letter)
        this.name = name; //                                - Longform name
        this.zeroTime = zeroTime; //                        - Reference time (time considered to be X+0)
        this.toStringUseStamp = toStringUseStamp; //        - Whether to use the stamp in the toString override.
    
        this.#empty = args == undefined; //                 - Whether this Clock was created as a placeholder (no args)
    }

    // Static getter for private eventBus
    static get eventBus(){
        return Clock.#eventBus;
    }

    // Instance getter for fetching the private static eventBus from an instance (so the class doesn't have to be imported just for this)
    get eventBus(){
        return Clock.eventBus;
    }

    // Returns whether this Clock was created as a placeholder (no args)
    get empty(){
        return this.#empty;
    }

    /** Shorthand for `timeDiffFast()`. Returns all days,hours,minutes, and seconds since the zeroTime */
    get now(){
        return this.timeDiffFast();
    }

    /** Override for default toString */
    toString(){
        return this.full({includeStamp: this.toStringUseStamp});
    }

    /**
     * Gives the full formatted form version of the given time relative to this clock's zeroTime.
     * If no time is given, this current time is used.
     * If include_stamp is true, the code name will be used to add a stamp to the 
     * beginning of the result, in the format of, for example: "X+02:00:00"
     */
    full({t=DateTime.utc(), includeStamp=false} = {}){
        const {direction, days, hours, minutes, seconds} = this.timeDiffFast({t});

        return '' +
            (includeStamp ? (this.code || '') : '')
            + (direction < 0 ? '-' : '+')
            + ((days*24 + hours) + ':').padStart(3,'0')
            + (minutes + ':').padStart(3,'0')
            + Math.round(seconds).toString().padStart(2,'0');
    }

    /**
     * Helper function to computes the difference in Earth days, hours, minutes, and 
     * seconds between the zeroTime and the given time (about 40x faster than luxon native)
     * Also returns the direction of comparison. (If =1: t>zero, =-1: t<zero, =0: t==zero)
     */
    timeDiffFast({t=DateTime.utc()} = {}) {
        const zero = this.zeroTime.toJSDate();
        const tt = t.toJSDate();

        const diff = tt - zero;
        const direction = Math.sign(diff);
        let seconds = Math.abs(diff) / 1000;
    
        // Days:
        const days = Math.floor(seconds / 86400);
        seconds -= days * 86400;
    
        // Hours:
        const hours = Math.floor(seconds / 3600) % 24;
        seconds -= hours * 3600;
    
        // Minutes:
        const minutes = Math.floor(seconds / 60) % 60;
        seconds -= minutes * 60;

        return {direction, days, hours, minutes, seconds};
    }

    /**
     * Reads the Clock for the given luxon DateTime object in the given luxon timeUnits 
     * relative to this clock's zeroTime.
     * If no time is given, this current time is used.
     * If t is before zeroTime, ALL returned values will be negative
     * If clean is false, the time will not be rounded to the nearest incremement of the given units.
     * If no timeUnits are given, hours are used.
     * If timeUnits is an array, an object with one key for each value is returned 
     * (eg. ['days','hours'] -> {days: 1, hours: 2}).
     * 
     * Note, if multiple timeUnits are specified, the results won't be cumulative. That is, if the time is 
     * +1 day and 2 hours, units('hours') will return 50 but units(['days','hours']) will return {days: 1, hours: 2}.
     */
    read({timeUnits='hours', t=DateTime.utc(), clean=false} = {}){
        const dt = t.diff(this.zeroTime, timeUnits);
        if(typeof timeUnits === 'string' || timeUnits instanceof String){
            if(clean){
                return Math.round(dt[timeUnits]);
            } else{
                return dt[timeUnits];
            }
        } else{
            if(clean){
                return dt.toObject().map( t => Math.round(t) );
            } else{
                return dt.toObject();
            }
        }
    }
    /**
     * Gives the short form version of the given time in days relative to this clock's zeroTime.
     * If clean is false, the time will not be rounded to the nearest incremement of the given units.
     * If no time is given, this current time is used.
     */
    days({t=DateTime.utc(), clean=false} = {}){ return this.read({timeUnits: 'days', t, clean}); }
    /**
     * Gives the short form version of the given time in hours relative to this clock's zeroTime.
     * If clean is false, the time will not be rounded to the nearest incremement of the given units.
     * If no time is given, this current time is used.
     */
    hours({t=DateTime.utc(), clean=false} = {}){ return this.read({timeUnits: 'hours', t, clean}); }
    /**
     * Gives the short form version of the given time in minutes relative to this clock's zeroTime.
     * If clean is false, the time will not be rounded to the nearest incremement of the given units.
     * If no time is given, this current time is used.
     */
    minutes({t=DateTime.utc(), clean=false} = {}){ return this.read({timeUnits: 'minutes', t, clean}); }
    /**
     * Gives the short form version of the given time in seconds relative to this clock's zeroTime.
     * If clean is false, the time will not be rounded to the nearest incremement of the given units.
     * If no time is given, this current time is used.
     */
    seconds({t=DateTime.utc(), clean=false} = {}){ return this.read({timeUnits: 'seconds', t, clean}); }

    // Loads Object from JSON.
    static fromJSON(data){
        if(data){
            if(typeof data == "string"){
                data = JSON.parse(data);
            }
            data.zeroTime = luxonFromJSON(data.zeroTime);
            return new Clock(data);
        } else{
            return data;
        }
    } // #fromJSON

    // Used by JSON.stringify
    toJSON(){
        let jsonObject = Object.assign({}, this); // make a copy (ignores private members)
        jsonObject.zeroTime = this.zeroTime.toJSON();
        return JSON.stringify(jsonObject);
    } // #toJSON
}