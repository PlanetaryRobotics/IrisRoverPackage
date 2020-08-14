/*
 * Vuex Module for Splitting out the Aspects of the Central Store owned by the
 * CLI.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 2/5/2019
 * Last Updated: 5/12/2019, Colombo
 */
 // TODO: Limit on number of commands which can be in LogWaiting.
 // FIXME: Make ErrorState show up in Command.vue when changed (currently *.vue has no idea)
 // TODO: Create attempt counter and limit on number of automatic attempts for PUSH_TO_CLILOG.
 // TODO: Pass specific error string from PUSH_TO_CLILOG to CLI via CommandData.
 // TODO: Make ListView.vue for Dynamic headIdx and len capture + dynamic update of contents

import DB from '@/DBInterface/DBInterface.js'
import DBLazyList from '@/DBInterface/DBLazyList.js'
import Collections from '@/DBInterface/Collections.js'

import CommandStates from '@/data_classes/CommandStates.js'
import CommandErrors from '@/data_classes/CommandErrors.js'
import CommandData from '@/data_classes/CommandData.js'
import CommandOption from '@/data_classes/CommandOption.js'

export default {
  getters: {
    // Returns a function to force vuex to recompute on each call
    // (otherwise it uses a cached value which only changes when the
    // /value/ of state.CLI.Log changes):
    LogList: state => () => state.Log.list
  },
  state: {
    // List of CommandData Objects Held in the Log in the Database's Commands Collection:
    Log: new DBLazyList(Collections.Commands, Infinity, Infinity, 3, CommandData),
    // List of All Commands which have been Ordered to be Sent to the Database but
    // haven't been Successfully Committed Yet:
    LogWaiting: [],

    // List of all available command options (such as TurnLeft, MoveForward):
    CommandOptions: [
      new CommandOption(
         "MoveForward",
         "Moves the robot forward by a specified amount, with a specified speed and acceleration, after a specified delay",
         "Movement",
         [
           {name: "Distance",      units: "cm",      defaultVal: 0},
           {name: "Speed",         units: "cm/s",    defaultVal: 0},
           {name: "Acceleration",  units: "cm/s^2",  defaultVal: 0},
         ],
         "sample_action.js"
       ),
       new CommandOption(
          "MoveBackward",
          "Moves the robot backward by a specified amount, with a specified speed and acceleration, after a specified delay",
          "Movement",
          [
            {name: "Distance",      units: "cm",      defaultVal: 0},
            {name: "Speed",         units: "cm/s",    defaultVal: 0},
            {name: "Acceleration",  units: "cm/s^2",  defaultVal: 0},
          ],
          "sample_action.js"
       ),
       new CommandOption(
          "TurnLeft",
          "Turns the robot left by a specified amount, with a specified angular speed and acceleration, after a specified delay",
          "Movement",
          [
            {name: "Angle",         units: "deg",     defaultVal: 0},
            {name: "Speed",         units: "deg/s",   defaultVal: 0},
            {name: "Acceleration",  units: "deg/s^2", defaultVal: 0},
          ],
          "sample_action.js"
        ),
        new CommandOption(
           "TurnRight",
           "Turns the robot right by a specified amount, with a specified angular speed and acceleration, after a specified delay",
           "Movement",
           [
             {name: "Angle",         units: "deg",     defaultVal: 0},
             {name: "Speed",         units: "deg/s",   defaultVal: 0},
             {name: "Acceleration",  units: "deg/s^2", defaultVal: 0},
           ],
           "sample_action.js"
         )
    ]
  },

  mutations: {
    // Issue a command to the LogWaiting list to indicate that it is trying to
    // be sent but hasn't been sent yet.
    COMMAND_WAITING: (state, command) => {
      state.LogWaiting.push(command);
    },
    // Remove the command at the given index from the LogWaiting list to
    // indicate that it has been successfully sent to the server.
    COMMAND_RESOLVED: (state, idx) => {
      state.LogWaiting.splice(idx, 1);
    }
  },

  actions: {
    // Adds the Given CommandData Object to the CLI.Log DBLazyList.
    PUSH_TO_CLILOG: ({ commit, state }, command) => {
      // Prepare CommandData for Database (log info about state at time of send):
      Object.assign(command.data, {
        sendTime: new Date(),
        stateUI: CommandStates.UI.WAITING,
        stateFp: CommandStates.Fp.NOT_SENT
      });

      // Commit a copy of the command to the log of all commands waiting to be
      // sent to the database.
      let awaitingCommand = new CommandData(command.data);
      awaitingCommand.data.stateUI = CommandStates.UI.LOCAL_ONLY; // Identify that it has not yet been sent.
      commit('COMMAND_WAITING', awaitingCommand);
      let hash = awaitingCommand.hash;

      // Define some internal helper functions for attempting to push data to database:
      let alertErrorAndRetry, attemptSend, findIdx;
      findIdx = () => { // Find index of awaitingCommand in LogWaiting list
                        // (could change due to async nature of pushing)
        let idx = -1, i = 0;
        while(idx == -1, i < state.LogWaiting.length){
          if(state.LogWaiting[i].hash === hash){
            idx = i;
            break;
          }
          i++;
        }

        return idx;
      }
      alertErrorAndRetry = () => { // Flag that there has been an issue in connection
                                   // and try again in 1s.
        awaitingCommand.stateUI = CommandStates.UI.FAIL;
        awaitingCommand.errorUI = CommandErrors.UI.DB_NOCONNECT;
        hash = awaitingCommand.hash; // update hash for later find
        setTimeout(attemptSend, 1000); // retry in 1 sec.
      }
      attemptSend = () => {
        state.Log.push(command).then( lid => {
          let idx = findIdx();
          state.Log.onNextUpdate( () => {
            // Remove command from waiting once it has been successfully pushed and loaded back in:
            commit('COMMAND_RESOLVED', idx);
          });
          state.Log.needsUpdate = 1;
          state.Log.list; // Force update now

          // TODO FIXME: Simulating timeout of completed command rn:
          let execTime = command.data.args[0] / command.data.args[1] || 1; // execution time [s]
          setTimeout(() => {
            DB.update(Collections.Commands, lid, {stateFp: "SUCC_EXEC", stateUI: "SUCCESS"});
          }, 1000*execTime+500);
        },
        alertErrorAndRetry
        );
      }

      // Attempt to send command to database:
      attemptSend();
    }
  }
}
