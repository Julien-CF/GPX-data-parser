'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************

let gpxparser = ffi.Library("./libgpxparser.so", {
  "getGPXInfo": ["string", ["string"]],
  "getTrackInfo": ["string", ["string"]],
  "getRouteInfo": ["string", ["string"]],
  "getRouteOtherData": ["string", ["string", "string"]],
  "getTrackOtherData": ["string", ["string", "string"]],
  "rewriteFile": ["int", ["string", "string", "string", "string"]],
  "createNewGpxDoc": ["int", ["string", "string"]],
  "addRoutetoGpxDoc": ["int", ["string", "string", "string"]],
  "routesInPath": ["string", ["double", "double", "double", "double", "int", "string"]],
  "tracksInPath": ["string", ["double", "double", "double", "double", "int", "string"]]
})

//Sample endpoint
app.get('/getFiles', function(req , res){


  let fileList = [];
  fs.readdirSync('./uploads/').forEach(file => {

    let curFile = gpxparser.getGPXInfo("./uploads/" + file);
    if(curFile != "error"){
      let jsonObject = JSON.parse(curFile);
      let numPoints =
      jsonObject.fileName = file;
      fileList.push(jsonObject);
      console.log(jsonObject);
    }

  })
  // console.log(fileList);

  res.send({
    files: fileList
  });
});

app.get('/getFileInfo', function(req , res){
  let trackList = [];
  let routeList = [];

    let curTrack = gpxparser.getTrackInfo("./uploads/" + req.query.file.fileName);
    let curRoute = gpxparser.getRouteInfo("./uploads/" + req.query.file.fileName);
    let trackJson = JSON.parse(curTrack);
    let routeJson = JSON.parse(curRoute);
    console.log(trackJson);
    console.log(routeJson);
    // trackList.push(trackJson);

  res.send({
    trkList: trackJson,
    rteList: routeJson
  });
});

app.get('/getRouteXtraData', function(req , res){
  let temp = gpxparser.getRouteOtherData("./uploads/" + req.query.file, req.query.routeName);
  let routeOtherData = JSON.parse(temp);
  console.log(routeOtherData);
  res.send({
    list: routeOtherData
  });
});

app.get('/getTrackXtraData', function(req , res){
  let temp = gpxparser.getTrackOtherData("./uploads/" + req.query.file, req.query.routeName);
  let trackOtherData = JSON.parse(temp);
  console.log(trackOtherData);
  res.send({
    list: trackOtherData
  });
});

app.get('/renameInFile', function(req , res){
  let check = gpxparser.rewriteFile("./uploads/" + req.query.filename, req.query.type, req.query.newName, req.query.oldname);
  console.log("got interval");
  res.send({
    valid: check
  });
});

app.get('/createGpx', function(req , res){
  let check;
  let token = req.query.filename.split(".");
  if(token.length == 2){
    // console.log("made it");
    if(token[1] === "gpx"){
      check = gpxparser.createNewGpxDoc("./uploads/" + req.query.filename, req.query.creator);
      // check = 1;
    } else{
      check = 0;
    }
  } else{
    check = 0;
  }
  console.log(check);
  res.send({
    valid: check
  });
});

app.get('/addRoute', function(req , res){
  console.log(req.query.filename);
  console.log(req.query.data);
  console.log(req.query.newName);

  let check = gpxparser.addRoutetoGpxDoc("./uploads/" + req.query.filename, req.query.newName, req.query.data);

  res.send({
    result: check
  });
});

app.get('/getPathsBetween', function(req , res){

  let files = req.query.listFiles;
  let longitudeStart = req.query.longitudeStart;
  let longitudeEnd = req.query.longitudeEnd;
  let latitudeStart = req.query.latitudeStart;
  let latitudeEnd = req.query.latitudeEnd;
  let delta = req.query.delta;
  console.log("----------------");
  console.log(longitudeStart);
  console.log(latitudeStart);
  console.log(latitudeEnd);
  console.log(longitudeEnd);
  console.log(delta);


  let routeList = [];
  let trackList = [];
  let file;
  let file2;
  for(file of files){
    let temp = gpxparser.routesInPath(latitudeStart, longitudeStart, latitudeEnd, longitudeEnd, delta, "./uploads/" + file.fileName);
    let routeTemp = JSON.parse(temp);
    for(file2 of routeTemp){
      routeList.push(file2);
    }
  }

  for(file of files){
    let temp = gpxparser.tracksInPath(latitudeStart, longitudeStart, latitudeEnd, longitudeEnd, delta, "./uploads/" + file.fileName);
    let trackTemp = JSON.parse(temp);
    for(file2 of trackTemp){
      trackList.push(file2);
    }
  }
  console.log(routeList);
  console.log(trackList);
  res.send({
    trackList: trackList,
    routeList: routeList
  });
});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
