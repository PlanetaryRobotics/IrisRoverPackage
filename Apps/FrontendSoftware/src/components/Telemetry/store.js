import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default ({
  state: {
    title: 'Data Log',
    links:[],
    flipBooks: {
      "rawData": [
        ["2017-03-01T01:00:00", 1, 3],
        ["2017-03-02T01:00:00", 2, 6],
        ["2017-03-03T01:00:00", 2, 6],
        ["2017-03-04T01:00:00", 2, 6],
        ["2017-03-05T01:00:00", 2, 6],
        ["2017-03-06T01:00:00", 4, 6],
        ["2017-03-07T01:00:00", 9, 7],
        ["2017-03-08T01:00:00", 18, 14],
        ["2017-03-09T01:00:00", 23, 16],
        ["2017-03-10T01:00:00", 24, 16],
        ["2017-03-11T01:00:00", 24, 16],
        ["2017-03-12T01:00:00", 24, 16],
        ["2017-03-13T01:00:00", 25, 18],
        ["2017-03-14T01:00:00", 26, 19],
        ["2017-03-15T01:00:00", 30, 21],
        ["2017-03-16T01:00:00", 32, 23],
        ["2017-03-17T01:00:00", 32, 23],
        ["2017-03-18T01:00:00", 32, 23],
        ["2017-03-19T01:00:00", 32, 23],
        ["2017-03-20T01:00:00", 32, 23],
        ["2017-03-21T01:00:00", 32, 25],
        ["2017-03-22T01:00:00", 32, 26],
        ["2017-03-23T01:00:00", 32, 28],
        ["2017-03-24T01:00:00", 32, 29],
        ["2017-03-25T01:00:00", 32, 29],
        ["2017-03-26T01:00:00", 32, 29],
        ["2017-03-27T01:00:00", 32, 29],
        ["2017-03-28T01:00:00", 35, 29],
        ["2017-03-29T01:00:00", 35, 29],
        ["2017-03-30T01:00:00", 35, null],
        ["2017-03-31T01:00:00", 35, null]
      ]
    }
  },
  getters:{
    countLinks: state => {
      return state.links.length; // get info that is not directly presented by the state objects
    }
  },
  mutations: {
    TELEMETRY_ADD_DATA:(state, link) =>{
      state.links.push(link);
    }, // optionally send in data
    TELEMETRY_REMOVE_LINK:(state,link) =>{
      state.links.splice(link,1);
    },// async involves actions
    TELEMETRY_REMOVE_ALL:(state)=>{
      state.links = [];
    }
  },
  actions: {
    removeLink:(context, link)=>{
      context.commit("TELEMETRY_REMOVE_LINK", link);
    },
    removeAll({commit}){
      return new Promise((resolve, reject) => {
        setTimeout(() =>{
          commit('TELEMETRY_REMOVE_ALL') // its the same as calling context.commit, calling a mutation
          resolve()
        }, 1500)
      })
    }
  }
})
