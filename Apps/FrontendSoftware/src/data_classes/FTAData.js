'use strict';
/*
 * Defines the graph connectivity and weights of the Sankey chart used in the 
 * Failure Tree Analysis (FTA) component. This data gets updated by a backend
 * process which is monitoring telemetry signals for faults.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/14/2020
 * Last Update: 08/14/2020, Colombo
 */

import DBObject from './DBObject.js';

import sankeyDefault from '@/components/FTA/sankey_default.json';

export default class FTAData extends DBObject{
    constructor(inputData){
        super(inputData);
    } // ctor

    // Returns the Default Data this Object Should Contain
    static defaultData(){
        return {
            ...sankeyDefault
        };
    }
  
} // class: FTAData
