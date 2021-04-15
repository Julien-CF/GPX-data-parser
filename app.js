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
  "tracksInPath": ["string", ["double", "double", "double", "double", "int", "string"]],
  "getPointInfo": ["string", ["String", "int"]]
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

app.get('/renameInFile', async function(req , res){
  let check = gpxparser.rewriteFile("./uploads/" + req.query.filename, req.query.type, req.query.newName, req.query.oldname);
  console.log(req.query.type);

  let type = req.query.type;
  let temp = type.split(" ");
  console.log(res);


  let connection;
  if(user.user != 'usernameGoesHere' && check != -1 && temp[0] === "route"){
    try{
        // create the connection
        connection = await mysql.createConnection(user);
        console.log("connected succesful");

        const [rows1, fields1] = await connection.execute(`SELECT * FROM FILE WHERE file_name='${req.query.filename}'`);
        console.log(rows1.length);
        if(rows1.length > 0){
          await connection.execute(`UPDATE ROUTE SET route_name='${req.query.newName}' WHERE route_name='${req.query.oldname}'`);

        } else {
          console.log("changes made locally");
        }

    }catch(e){
      console.log("Query error: "+e);
    }finally {
      if (connection && connection.end) connection.end();
    }

  }


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

app.get('/addRoute', async function(req , res){
  console.log(req.query.filename);
  console.log(req.query.data);
  console.log(req.query.newName);

  let check;
  check = gpxparser.addRoutetoGpxDoc("./uploads/" + req.query.filename, req.query.newName, req.query.data);
  console.log("check: ");
  console.log(check);

  let connection;
  if(user.user != 'usernameGoesHere' && check != -1){

    try{
        // create the connection
        connection = await mysql.createConnection(user);
        console.log("connected succesful");

        const [rows1, fields1] = await connection.execute(`SELECT * FROM FILE WHERE file_name='${req.query.filename}'`);
        console.log(rows1.length);
        if(rows1.length > 0){
          await connection.execute(`INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES ("${req.query.newName}", "${check}", "${rows1[0].gpx_id}")`);


          const [rows2, fields2] = await connection.execute(`SELECT * FROM ROUTE WHERE gpx_id='${rows1[0].gpx_id}'`);
          let pointInfoString = gpxparser.getPointInfo("./uploads/" + req.query.filename, rows2.length);
          let pointInfo = JSON.parse(pointInfoString);
          console.log("HEREEEEEEEEE");
          console.log(pointInfo);
          let index2 = 0;
          for(let point of pointInfo){
            let insertion = await connection.execute(`INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) VALUES ("${index2}", "${point.latitude}", "${point.longitude}", "${point.name}", "${rows2[rows2.length - 1].route_id}")`);
            index2++;
          }


        } else {
          console.log("changes made locally");
        }



    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
    }
  }

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

const mysql = require('mysql2/promise');


let user = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : 'usernameGoesHere',
	password : 'passwordGoesHere',
	database : 'databaseNameGoesHere'
};

app.get('/login', async function(req , res){
  let connection;
  user.user = req.query.username;
  user.password = req.query.password;
  user.database = req.query.database;

  console.log(user.user);
  console.log(user.password);
  console.log(user.database);
  try{
      // create the connection
      connection = await mysql.createConnection(user);
      connection.execute("create table IF NOT EXISTS FILE (gpx_id int auto_increment,  file_name VARCHAR(15) not null,  ver DECIMAL(2,1) not null, creator varchar(256) not null, primary key(gpx_id) )");
      connection.execute("create table IF NOT EXISTS ROUTE (route_id int auto_increment,  route_name VARCHAR(256),  route_len FLOAT(15, 7) not null, gpx_id INT not null, primary key(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)");
      connection.execute("create table IF NOT EXISTS POINT (point_id int auto_increment,  point_index int not null,  latitude DECIMAL(11,7) not null, longitude DECIMAL(11,7) not null, point_name varchar(256), route_id int not null, primary key(point_id), FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)");


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({

  });
});

app.get('/addToDatabase', async function(req , res){
  let connection;

  try{
      // create the connection
      connection = await mysql.createConnection(user);
      let file;
      for(file of req.query.fileList){
        const [rows1, fields1] = await connection.execute(`SELECT * FROM FILE WHERE file_name='${file.fileName}'`);

        if(rows1.length == 0){
          console.log("we are good");
          let insertion = await connection.execute(`INSERT INTO FILE (file_name, ver, creator) VALUES ("${file.fileName}", "${file.version}", "${file.creator}")`);
          let routeListstring = gpxparser.getRouteInfo("./uploads/" + file.fileName);
          let routeList = JSON.parse(routeListstring);
          console.log(routeList);
          const [rows, fields] = await connection.execute(`SELECT FILE.gpx_id FROM FILE WHERE file_name='${file.fileName}'`);
          console.log(rows[0].gpx_id);
          let route;
          for(route of routeList){
            let insertion = await connection.execute(`INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES ("${route.name}", "${route.len}", "${rows[0].gpx_id}")`);
          }
          const [rows3, fields3] = await connection.execute(`SELECT * FROM ROUTE WHERE gpx_id='${rows[0].gpx_id}'`);
          console.log(rows3);
          let index = 1;
          for(let curRows of rows3){
            let pointInfoString = gpxparser.getPointInfo("./uploads/" + file.fileName, index);
            let pointInfo = JSON.parse(pointInfoString);
            console.log(pointInfo);
            let index2 = 0;
            for(let point of pointInfo){
              let insertion = await connection.execute(`INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) VALUES ("${index2}", "${point.latitude}", "${point.longitude}", "${point.name}", "${curRows.route_id}")`);
              index2++;
            }

            index++;
          }

        }
      }



  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({

  });
});

app.get('/clearDatabase', async function(req , res){
  let connection;

  try{
      // create the connection
      connection = await mysql.createConnection(user);
      await connection.execute(`DELETE FROM FILE`);


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
  });
});

app.get('/status', async function(req , res){
  let connection;
  let file;
  let route;
  let point;
  try{
      // create the connection
      connection = await mysql.createConnection(user);
      const [rows, fields] = await connection.execute(`select * FROM FILE`);
      const [rows2, fields2] = await connection.execute(`select * FROM ROUTE`);
      const [rows3, fields3] = await connection.execute(`select * FROM POINT`);

      file = rows.length;
      route = rows2.length;
      point = rows3.length;



  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    file: file,
    route: route,
    point: point
  });
});

app.get('/Query1', async function(req , res){
  let connection;
  let listfiles;
  console.log(req.query.sort);
  try{
      // create the connection
      connection = await mysql.createConnection(user);

      if(req.query.sort === "name"){
        listfiles = await connection.execute(`select * from ROUTE ORDER BY route_name ASC`);
      } else if(req.query.sort === "length") {
        listfiles = await connection.execute(`select * from ROUTE ORDER BY route_len ASC`);
      }

      console.log(listfiles);


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    list: listfiles[0]
  });
});

app.get('/Query2', async function(req , res){
  let connection;
  let listfiles = null;
  let status = -1;
  console.log(req.query.sort);
  try{
      // create the connection
      connection = await mysql.createConnection(user);
      const [rows1, fields1] = await connection.execute(`SELECT * FROM FILE WHERE file_name='${req.query.filename}'`);

      if(rows1.length == 0){
        status = -1;
      } else {
        if(req.query.sort === "name"){
          listfiles = await connection.execute(`select * from ROUTE WHERE gpx_id='${rows1[0].gpx_id}' ORDER BY route_name ASC`);
          status = 1;
        } else if(req.query.sort === "length") {
          listfiles = await connection.execute(`select * from ROUTE WHERE gpx_id='${rows1[0].gpx_id}' ORDER BY route_len ASC`);
          status = 1;
        }
      }


      console.log(listfiles);


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    list: listfiles,
    status: status
  });
});

app.get('/Query3', async function(req , res){
  let connection;
  let listPoints = null;
  let status = -1;
  console.log("Here " + req.query.route);
  try{
      // create the connection
      connection = await mysql.createConnection(user);
      const [rows1, fields1] = await connection.execute(`SELECT * FROM FILE WHERE file_name='${req.query.filename}'`);

      if(rows1.length == 0){
        status = -1;
      } else {
        let routes;
        console.log(rows1[0].gpx_id);
        const [rows, field] = await connection.execute(`select * from ROUTE WHERE gpx_id='${rows1[0].gpx_id}' AND route_name='${req.query.route}'`);
        status = 1;
        if(rows[0].length == 0){
          status = -1;
        } else {
          console.log("Here " + rows[0].route_name);
          listPoints = await connection.execute(`select * FROM POINT WHERE route_id='${rows[0].route_id}' ORDER BY point_index`);
        }


      }


      console.log(listPoints);


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    list: listPoints,
    status: status
  });
});

app.get('/Query4', async function(req , res){
  let connection;
  let listPoints = [];
  let status = -1;
  console.log("Here " + req.query.filename);
  try{
      // create the connection
      connection = await mysql.createConnection(user);
      const [rows1, fields1] = await connection.execute(`SELECT * FROM FILE WHERE file_name='${req.query.filename}'`);

      if(rows1.length == 0){
        status = -1;
      } else {
        status = 1;
        let routeList;
        if(req.query.sort === "name"){
          const [rows2, fields2] = await connection.execute(`select * from ROUTE WHERE gpx_id='${rows1[0].gpx_id}' ORDER BY route_name ASC`);
          routeList = rows2;


        } else if(req.query.sort === "length") {
          const [rows2, fields2] = await connection.execute(`select * from ROUTE WHERE gpx_id='${rows1[0].gpx_id}' ORDER BY route_len ASC`);
          routeList = rows2;

        }

        console.log("RouteList : \n" + routeList);

        for(let route of routeList){
          console.log(route);
          const [rows3, fields3] = await connection.execute(`select * FROM POINT WHERE route_id='${route.route_id}' ORDER BY point_index`);
          listPoints.push(rows3);
        }


      }


      console.log(listPoints);


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    list: listPoints,
    status: status
  });
});

app.get('/Query5', async function(req , res){
  let connection;
  let listfiles = null;
  let status = -1;
  console.log(req.query.place);
  try{
      // create the connection
      connection = await mysql.createConnection(user);
      const [rows1, fields1] = await connection.execute(`SELECT * FROM FILE WHERE file_name='${req.query.filename}'`);

      if(rows1.length == 0){
        status = -1;
      } else {
        if(req.query.place === "TOP"){
          listfiles = await connection.execute(`select * from ROUTE WHERE gpx_id='${rows1[0].gpx_id}' ORDER BY route_len DESC LIMIT ${req.query.n}`);
          status = 1;
        } else if(req.query.place === "BOTTOM") {
           listfiles = await connection.execute(`select * from ROUTE WHERE gpx_id='${rows1[0].gpx_id}' ORDER BY route_len ASC LIMIT ${req.query.n}`);
           status = 1;
        }
      }


      console.log(listfiles);


  }catch(e){
      console.log("Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();
  }

  res.send({
    list: listfiles,
    status: status
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
