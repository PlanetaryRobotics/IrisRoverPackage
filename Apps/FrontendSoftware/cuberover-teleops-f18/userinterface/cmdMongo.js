/* 

    file: list of commandObjects. 

    Command object: 

    commandJSON
    userName
    stateFP
    stateUI
    errortypeFP : 
    errortypeUI : 
        Permission Denied
        timeout
    timeSent
    timeComplete

*/ 


/* 
    include file, node looks to whatever was exported. 

    export default class CommandObject; 

    import CommandObject 
*/ 

/*
    commandJSON = 
        cmd: name
        arg1: arg1
        arg2: 
*/



/* 
    void sendComand(string JSON cmd) 
    {
        creates command object 

        check user permission
        generate assignID
        assign cmdID
        writeMongo
    }
*/ 

function checkPermission(username)
{
    return true;
}

    
var cmdObjList = new Array();
var globalCmdID = 0;
var testID;
function sendCommand(jsonCmd, username, time)
{
    /* check user permission*/
    if(!checkPermission(username))
    {
        return;
    }
    /* get timer*/


    /* assign cmdID*/ 
    var cmdIDName = globalCmdID;
    testID = cmdIDName;
    globalCmdID = globalCmdID + 1;


    /* create command object */ 
    var cmdObj = 
    {
        cmdID : cmdIDName,
        commandJSON : jsonCmd,
        userName  : username,
        stateFP : "awaitingState",
        stateUI : "sent",
        errorTypeFP : "NA",
        errorTypeUI : "NA",
        timeSent : time,
        timeComplete : "NA",
    }

    /* for now cmdID can just be incrememntal counter*/
    /* cmdID = sha256(jsonCmd) */ 
    writeUIdb(cmdObj,"Commands");

    /* writeMongo*/ 

}


function writeUIdb(myobj, collection)
{
    var mongo = require('mongodb');
    var assert = require('assert');
    var MongoClient = mongo.MongoClient;
    var url = "mongodb+srv://CubeRoverAdmin:RedRover@cluster0-a0vk8.mongodb.net/UIPartition?retryWrites=true";
    MongoClient.connect(url, {useNewUrlParser: true}, function(err,db){
    
    if(err) throw err;
    console.log("Connected to UIPartition Database");
    
    var dbo = db.db("UIPartition");
    //var myobj = { name: "testR0ver", opcode:"0000", type:"command", status:"toRover"};
    dbo.collection(collection).insertOne(myobj,function(err,res)
        {
            if(err) throw err;
            console.log("1 document inserted");
            db.close();
        });
    
     if(!err) return true; 
    });
    
}


function readUIdb(myobj, collection, cmdIDname)
{
    var mongo = require('mongodb');
    var assert = require('assert');
    var MongoClient = mongo.MongoClient;
    var url = "mongodb+srv://CubeRoverAdmin:RedRover@cluster0-a0vk8.mongodb.net/UIPartition?retryWrites=true";
    MongoClient.connect(url, {useNewUrlParser: true}, function(err,db){
    
    if(err) throw err;
    console.log("Connected to UIPartition Database");
    
    var dbo = db.db("UIPartition");

    var query = { cmdID: cmdIDname};
    dbo.collection(collection).find(query).toArray(function(err, myobj) {
    if (err) throw err;
    console.log(result);
    db.close();

    });

    });

}

/*
    setTimeout(cmdID)
    {
        update errorTypeUI to timeout in Object CmdID
        read from mongo cmdID and update errorTypeUI to timeout
        writeToMongo 
    }
*/ 

function setTimeout(cmdIDname)
{
    /* read cmdID into obj*/ 
    var myquery = {cmdID : cmdIDname};
    var newValues = { $set: {errorTypeUI: "timeout"}};
    var mongo = require('mongodb');
    var assert = require('assert');
    var MongoClient = mongo.MongoClient;
    var url = "mongodb+srv://CubeRoverAdmin:RedRover@cluster0-a0vk8.mongodb.net/UIPartition?retryWrites=true";
    MongoClient.connect(url, {useNewUrlParser: true}, function(err,db){
    
    if(err) throw err;
    console.log("Connected to UIPartition Database");
    
    var dbo = db.db("UIPartition");

    dbo.collection("Commands").updateOne(myquery, newValues, function(err, res)
        {
            if (err) throw err;
            console.log("1 document updated");
            db.close();
        });

    });
}


function getStatus(cmdIDname)
{
    /* read cmdID into obj*/ 
    var myquery = {cmdID : cmdIDname};
    var newValues = { $set: {errorTypeUI: "timeout"}};
    var mongo = require('mongodb');
    var assert = require('assert');
    var MongoClient = mongo.MongoClient;
    var url = "mongodb+srv://CubeRoverAdmin:RedRover@cluster0-a0vk8.mongodb.net/UIPartition?retryWrites=true";
    MongoClient.connect(url, {useNewUrlParser: true}, function(err,db){
    
    if(err) throw err;
    console.log("Connected to UIPartition Database");
    
    var dbo = db.db("UIPartition");

    dbo.collection("Commands").updateOne(myquery, newValues, function(err, res)
        {
            if (err) throw err;
            console.log("1 document updated");
            db.close();
        });

    });
}



/* test by sending command*/ 

var command = { name: "Estop", opcode:"0000", type:"command", status:"toRover"};
    
console.log("About to send command");
sendCommand(command, "Sofia", "11:16 AM");

/* set timeout*/ 
console.log("About to set Timeout");
setTimeout(testID); 

/*
    checkCmdState(cmdID)
    {
        read mongo.cmd.StateFP into cmd(id).StateFP
        update 
    }

    array cmdObjects loadCommand(array[command ids])
    {
        create command objects. 
        parse cmdJSON-> cmdObjects. 
    }
    
*/ 