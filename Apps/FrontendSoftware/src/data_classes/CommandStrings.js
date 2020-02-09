/*
 * Strings Relevant to Messages within a Command
 */

import Errors from './CommandErrors.js'

const errorStrings = {};
errorStrings[Errors.Fp.TRANSMISSION] = `There might have been an error in transmission and
  this command might not have been received by the rover`;
errorStrings[Errors.Fp.EXECUTION] =  `The rover was not able to successfully execute this command.`;

errorStrings[Errors.UI.DB_NOCONNECT] = `Application could not connect to server of database
  from which commands are forwarded to the rover. Please check your internet connection.`
errorStrings[Errors.UI.TIMEOUT] = "This command timed out.";
errorStrings[Errors.UI.PERMISSON] = `You do not have permissions to send commands.
  This command was not sent.`;

export default errorStrings;
