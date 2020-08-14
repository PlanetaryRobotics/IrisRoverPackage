'use strict';
/*
 * Basic implementation of an Enum for places where values must be confined to a
 * known set.
 * Sample Usage:

    const StateType = new Enum("STATE_A", "STATE_B", "STATE_C");
    state1 = StateType.STATE_A;
    state2 = StateType.STATE_B;
    state3 = StateType.fromString("STATE_C");
    badState = StateType.fromString("STATE_XYZ");
    assert(state1 === StateType.STATE_A);
    assert(state3 === StateType.STATE_C);
    assert(!!state2);
    assert(!badState);
    assert(badState === StateType.NOT_A_TYPE);

 * Author: Connor W. Colombo, CMU
 * Last Update: 4/26/2019, Colombo
 */

/* Creates an Enum Type with the only valid enumerators being those strings in
the arguments given to the constructor */
export default class Enum{
  constructor(...names){
    this.enumerators = {
      NOT_A_TYPE: 0 // Not unique. Evaluates to False
    };
    names.forEach( n => this.enumerators[n] = Symbol(n) );
    Object.assign(this, this.enumerators); // Clone enumerator values for direct access
    Object.freeze(this.enumerators); // Prevent changes
  }

  /* Checks to see if the given string, str, matches an Enum name. If so, it
  returns the matching enumator entry; otherwise, returns NOT_A_TYPE.
  */
  fromString(str){
    let retVal = this.enumerators.NOT_A_TYPE
    if (str !== this.enumerators.NOT_A_TYPE) {
      for (const e in this.enumerators) {
        // If a valid type (not NOT_A_TYPE) and symbol string matches given string:
        if (this.enumerators[e] && String(this.enumerators[e]).slice(7, -1) === str) {
          retVal = this.enumerators[e]
          break
        }
      }
    }

    return retVal
  } // #fromString

  // Returns if the Given Object is a Valid Enumerator Symbol of this Type
  contains(obj){
    let contained = false;
    const values = Object.values(this.enumerators);
    for(let val of values){
      contained = val === obj;
      if(contained){
        break;
      }
    }
    return contained;
  }

  // Returns Whether the Given Object is a Valid Member of this Enumerator.
  // 'obj' can be either an Enumerator Symbol or String.
  isValid(obj){
    return obj && (this.contains(obj) || this.fromString(obj));
  }

  // Converts the given enumerator Symbol into a string of just the name.
  static toString(obj){
    return obj ? String(obj).slice(7, -1) : undefined;
  }
  toString(o){ return Enum.toString(o); } // Allow for calling on instance as well (for backwards compatibility)
} // Class Enum
