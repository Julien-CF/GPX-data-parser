function RENAMEMELATER(selected, pathDropdown){
  // $(".dropDownOption").val(selected);

  if (pathDropdown === "NCDropdown") {
    $("table.view-panel tbody tr").remove();
  }
  $(".NCDropdown#" + pathDropdown + " option").remove();
  for(file of fileList){
    if(file.fileName === selected){
      console.log(file.fileName, file.numTracks);
      $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/getFileInfo',   //The server endpoint we are connecting to
        data: {
          file: file
        },
        success: function(data){
          console.log(data.trkList);
          console.log(data.rteList);
          let i = 1;
          for(track of data.trkList){
            let newElement = $(`
                <tr>
                  <th><button id="trackbutton">track ${i}</button></th>
                  <th>${track.name}</th>
                  <th>${track.numPoints}</th>
                  <th>${track.len}</th>
                  <th>${track.loop}</th>
                </tr>
              `);
              $('.track#' + pathDropdown).append($('<option>',{
                value: "track" + " " + i,
                text: track.name
              }));

              let x = track.name;
              let filename = document.getElementById("files");


              if (pathDropdown === "NCDropdown") {
                newElement.find("button").click(() => {
                  console.log("button pressed: " + x);
                  $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything
                    url: '/getTrackXtraData',   //The server endpoint we are connecting to
                    data: {
                      file: filename.value,
                      routeName: x
                    },
                    success: function(data){
                      for(indData of data.list){
                        alert("Name : " + indData.name + "\n" + "Value : " + indData.value);
                      }
                    },
                    fail: function(error) {
                      $('#blah').html("On page load, received error from server");
                      console.log(error);
                    }
                  });
                });
                let body = $("table.view-panel tbody");

                body.append(newElement);
              }
              i++;
          }
          i = 1;
          for(route of data.rteList){
            let newElement = $(`
                <tr>
                  <th><button id='routeButton'>route ${i}</button></th>
                  <th>${route.name}</th>
                  <th>${route.numPoints}</th>
                  <th>${route.len}</th>
                  <th>${route.loop}</th>
                </tr>
              `);
                $("#" + pathDropdown).append($('<option>',{
                  value: "route" + " " + i,
                  text: route.name
                }));
              if (pathDropdown === "NCDropdown") {
                let body = $("table.view-panel tbody");
                let x = route.name;
                let filename = document.getElementById("files");
                newElement.find("button").click(() => {
                  // console.log("button pressed: " + x + filename.value);
                  $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything
                    url: '/getRouteXtraData',   //The server endpoint we are connecting to
                    data: {
                      file: filename.value,
                      routeName: x
                    },
                    success: function(data){
                      for(indData of data.list){
                        alert("Name : " + indData.name + "\n" + "Value : " + indData.value);
                      }
                    },
                    fail: function(error) {
                      $('#blah').html("On page load, received error from server");
                      console.log(error);
                    }
                  });
                });

                body.append(newElement);
              }
              i++;
          }
        },
        fail: function(error) {
          $('#blah').html("On page load, received error from server");
          console.log(error);
        }
      });
    }
  }
}

function Status(){
  jQuery.ajax({
      type: 'get',            //Request type
      dataType: 'json',       //Data type - we will use JSON for almost everything
      url: '/status',   //The server endpoint we are connecting to
      data: {
      },
      success: function (data) {
          alert("Database has " + data.file + " files, " + data.route + " routes, and " + data.point + " points.");
      },
      fail: function(error) {
          // Non-200 return, do something with error
          $('#blah').html("On page load, received error from server");
          console.log(error);
      }
  });
}

// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {

     fileList = [];
    // On page-load AJAX Exa;mple
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/getFiles',   //The server endpoint we are connecting to
        data: {
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            // jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
            // //We write the object to the console to show that the request was successful
            if(data.files.length == 0){
              console.log("empty");
              let newElement = $(`
                  <tr>
                    <th>No files to be displayed</th>
                    <th>NA</th>
                    <th>NA</th>
                    <th>NA</th>
                    <th>NA</th>
                    <th>NA</th>
                  </tr>
                `);
                let body = $("table.file-logPanel tbody");

                body.append(newElement);
            }
            for(file of data.files){
              let newElement = $(`
                  <tr>
                    <th> <a href="${file.fileName}" download >${file.fileName}</a> </th>
                    <th>${file.version}</th>
                    <th>${file.creator}</th>
                    <th>${file.numWaypoints}</th>
                    <th>${file.numRoutes}</th>
                    <th>${file.numTracks}</th>
                  </tr>
                `);

                let body = $("table.file-logPanel tbody");

                $('.dropDownOption').append($('<option>',{
                  value: file.fileName,
                  text: file.fileName
                }));

                body.append(newElement);

                fileList.push(file);
            }
            // console.log(data.files);

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error);
        }
    });

      $("#login").submit((e) =>{
        let username = document.getElementById("username").value;
        let password = document.getElementById("password").value;
        let database = document.getElementById("database").value;

        console.log(username);
        console.log(password);
        console.log(database);

        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/login',   //The server endpoint we are connecting to
          data: {
            username: username,
            password: password,
            database: database
          },
          success: function(data){
            console.log("logged in");
          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      });

      document.getElementById("addDataBase").addEventListener("click", function(){
        console.log(fileList);

        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/addToDatabase',   //The server endpoint we are connecting to
          data: {
            fileList: fileList
          },
          success: function(data){
            console.log("database updated");
            Status();
          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      });

      document.getElementById("displayStatus").addEventListener("click", function(){
        Status();
      });


      document.getElementById("clearDatabase").addEventListener("click", function(){

        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/clearDatabase',   //The server endpoint we are connecting to
          data: {
          },
          success: function(data){
            console.log("database cleared");
            Status();
          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      });


    // let searchPathForm = document.getElementById("search");
    //
    // searchPathForm.addEventListener("click", (e) =>{
    //
    // });

    $("#search").submit((e) =>{
      e.preventDefault();

       let longitudeStart = document.getElementById("longitudeStart").value;
       let longitudeEnd = document.getElementById("longitudeEnd").value;

       let latitudeStart = document.getElementById("latitudeStart").value;
       let latitudeEnd = document.getElementById("latitudeEnd").value;

       let check = 1;

       let delta = document.getElementById("delta").value;

       if($.isNumeric(longitudeStart) && longitudeStart > -181 && longitudeStart < 181){
         console.log(longitudeStart);
       } else {
         alert("longitudes starting point is invalid");
         check = 0;
       }

       if($.isNumeric(longitudeEnd) && longitudeEnd > -181 && longitudeEnd < 181){
         console.log(longitudeEnd);
       } else {
         alert("longitudes ending point is invalid");
         check = 0;
       }


       if($.isNumeric(latitudeStart) && latitudeStart > -91 && latitudeStart < 91){
         console.log(latitudeStart);
       } else {
         alert("latitudes starting point is invalid");
         check = 0;
       }


       if($.isNumeric(latitudeEnd) && latitudeEnd > -91 && latitudeEnd < 91){
         console.log(latitudeEnd);
       } else {
         alert("latitudes ending point is invalid");
         check = 0;
       }


       if($.isNumeric(delta) && delta > -1){
        console.log(delta);
       } else {
        alert("delta is invalid");
        check = 0;
       }




       if(fileList.length == 0){
         check = 0;
         alert("No files are available");
       }
       // console.log(longitude);
       // console.log(latitude);
       // console.log(delta);


       if(check == 1){
         $.ajax({
           type: 'get',            //Request type
           dataType: 'json',       //Data type - we will use JSON for almost everything
           url: '/getPathsBetween',   //The server endpoint we are connecting to
           data: {
             listFiles: fileList,
             longitudeStart: longitudeStart,
             longitudeEnd: longitudeEnd,
             latitudeStart: latitudeStart,
             latitudeEnd: latitudeEnd,
             delta: delta
           },
           success: function(data){
             trackList = data.trackList;
             routeList = data.routeList;

             let i = 1;
             for(track of data.trackList){
               let newElement = $(`
                   <tr>
                     <th>track ${i}</th>
                     <th>${track.name}</th>
                     <th>${track.numPoints}</th>
                     <th>${track.len}</th>
                     <th>${track.loop}</th>
                   </tr>
                 `);

                 let body = $("table.inBetweenTable tbody");

                 body.append(newElement);
                 i++;
              }
              i = 0;
              for(route of data.routeList){
                let newElement = $(`
                    <tr>
                      <th>route ${i}</th>
                      <th>${route.name}</th>
                      <th>${route.numPoints}</th>
                      <th>${route.len}</th>
                      <th>${route.loop}</th>
                    </tr>
                  `);
                  let body = $("table.inBetweenTable tbody");

                  body.append(newElement);
                  i++;
              }
           },
           fail: function(error) {
             $('#blah').html("On page load, received error from server");
             console.log(error);
           }
         });
       }

    });

    function addRoutes(routesList, filename){
      $("table.databaseRouteTable tbody tr").remove();

      for(route of routesList){
        let newElement = $(`
            <tr>
              <th>${route.route_name}</th>
              <th>${route.route_len}</th>
              <th>${filename}</th>
            </tr>
          `);
          let body = $("table.databaseRouteTable tbody");

          body.append(newElement);
      }
    }

    function addPoints(pointsList){
      console.log("getting here");
      $("table.databasePointTable tbody tr").remove();

      for(point of pointsList){
        console.log(point);
        let newElement = $(`
            <tr>
              <th>${point.point_name}</th>
              <th>${point.latitude}</th>
              <th>${point.longitude}</th>
              <th>${point.route_id}</th>
              <th>${point.point_index}</th>
            </tr>
          `);
          console.log(newElement);
          let body = $("table.databasePointTable tbody");

          body.append(newElement);
      }
    }


    let query1 = $(".Query1");

    query1.click(() => {
      $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/Query1',   //The server endpoint we are connecting to
        data: {
          sort: $(".sortingDropDown").val()
        },
        success: function(data){
          console.log(data.list.length);
          addRoutes(data.list, "All");
          // for(let route of data.list){
          //   console.log(route.route_name + " " + route.route_len);
          // }
        },
        fail: function(error) {
          $('#blah').html("On page load, received error from server");
          console.log(error);
        }
      });
    });

    let query2 = $(".Query2");

    query2.click(() => {
      let filename = document.getElementById("filesQuery").value;

      if(filename === "options"){
        alert("Please select a filename");
      }else{
        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/Query2',   //The server endpoint we are connecting to
          data: {
            sort: $(".sortingDropDown").val(),
            filename: filename
          },
          success: function(data){
            console.log(data);
            // console.log(data.list.length);
            if(data.status == 1){
              addRoutes(data.list[0], filename);
              // for(let route of data.list[0]){
              //   console.log(route.route_name + " " + route.route_len);
              // }
            } else {
              alert("File selected is not yet in Database please update accordingly");
            }

          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      }

    });

    let query3 = $(".Query3");

    query3.click(() => {
      let filename = document.getElementById("filesQuery").value;

      if(filename === "options"){
        alert("Please select a filename");
      }else{
        let route = $("#NCDropdownQuery option:selected").text();

        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/Query3',   //The server endpoint we are connecting to
          data: {
            route: route,
            filename: filename
          },
          success: function(data){
            console.log(data);
            // console.log(data.list.length);
            if(data.status == 1){
              addPoints(data.list[0]);
              for(let point of data.list[0]){
                console.log(point.point_index + " " + point.route_id);
              }
            } else {
              alert("File or route selected is not yet in Database please update accordingly");
            }

          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      }

    });

    let query4 = $(".Query4");

    query4.click(() => {
      let filename = document.getElementById("filesQuery").value;
      console.log($(".sortingDropDown").val());
      let sort = $(".sortingDropDown").val();
      if(filename === "options"){
        alert("Please select a filename");
      }else{

        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/Query4',   //The server endpoint we are connecting to
          data: {
            sort: sort,
            filename: filename
          },
          success: function(data){
            console.log(data);
            // console.log(data.list.length);
            let pointList = [];
            if(data.status == 1){
              for(let route of data.list){
                // console.log(route);
                for(let i = 0; i < route.length; i++){
                  pointList.push(route[i]);
                  console.log(route[i]);
                }
              }
              console.log(pointList);
              addPoints(pointList);
            } else {
              alert("File selected is not yet in Database please update accordingly");
            }

          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      }

    });

    let query5 = $(".Query5");

    query5.click(() => {
      let filename = document.getElementById("filesQuery").value;

      if(filename === "options"){
        alert("Please select a filename");
      }else{
        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/Query5',   //The server endpoint we are connecting to
          data: {
            sort: $(".sortingDropDown").val(),
            filename: filename,
            n: document.getElementById("N amount").value,
            place: $(".topOrBottom option:selected").text()
          },
          success: function(data){
            console.log(data);
            // console.log(data.list.length);
            if(data.status == 1){
              listRoutes = data.list[0];
              if($(".sortingDropDown").val() === "name"){
                listRoutes.sort(function(a, b){
                  if(a.route_name.toUpperCase() < b.route_name.toUpperCase()){
                    return -1;
                  }
                  if(a.route_name.toUpperCase() > b.route_name.toUpperCase()){
                    return 1;
                  }
                  return 0;
                });
              } else if($(".sortingDropDown").val() === "length"){
                listRoutes.sort(function(a, b){
                  return (a.route_len - b.route_len);
                });
              }
              addRoutes(listRoutes, filename);
              // for(let route of listRoutes){
              //   console.log(route.route_name + " " + route.route_len);
              // }
            } else {
              alert("File selected is not yet in Database please update accordingly");
            }

          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      }

    });



    let dropdowns = $(".dropDownOption");

    dropdowns.change(() => {
      RENAMEMELATER(event.target.value, $(event.target).attr("for"));
    });

    let form = $("#rename");

    form.find(".submit").click((e) => {
      e.preventDefault();
      let data = new FormData(form[0]);
      let filename = document.getElementById("files").value;
      let toBeChanged = document.getElementById("NCDropdown");




      let oldname = $('#NCDropdown option:selected').text();
      let newName = document.getElementById('nameChange').value;
      console.log(filename);
      console.log(toBeChanged.value);
      console.log(oldname);
      console.log(newName);

      if(filename === "options"){
        alert("please select a file");
      } else{
        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/renameInFile',   //The server endpoint we are connecting to
          data: {
            filename: filename,
            type: toBeChanged.value,
            oldname: oldname,
            newName: newName
          },
          success: function(data){
            RENAMEMELATER($("#files").val(), $("#files").attr("for"));
            Status();
          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      }

    });


    let createForm = $('#create');

    createForm.find(".create").click((e) => {
      e.preventDefault();
      let data = new FormData(createForm[0]);
      let filename = document.getElementById('filename').value;
      let creator = document.getElementById('creator').value;
      console.log(filename);
      console.log(creator);


      let check = 1;

      for(file of fileList){
        console.log(file.fileName);
        if(file.fileName === filename){
          alert("Filename already in use");
          check = 0;
        }
      }
      if(creator.length == 0){
        alert("please indicate a creator");
        check = 0;
      }
      if(check == 1){
        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/createGpx',   //The server endpoint we are connecting to
          data: {
            filename: filename,
            creator: creator
          },
          success: function(data){
            if(data.valid == 0){
              alert("invalid filename");
            } else{
              location.reload();
            }
          },
          fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
          }
        });
      }

    });

    listNewWaypoints = "";

    let createWaypointForm = $('#createPoint');

    createWaypointForm.find(".addPoint").click((e) => {
      e.preventDefault();

      let latitude = document.getElementById('lat').value;
      let longitude = document.getElementById('lon').value;

      if($.isNumeric(latitude) && $.isNumeric(longitude)){
        listNewWaypoints += longitude + " " + latitude + "|";
      } else {
        alert("Error: points can only be integers");
      }
      console.log(listNewWaypoints);
    });

    let createRouteForm = $('#createRoute');

    createRouteForm.find(".addRoute").click((e) => {
      e.preventDefault();

      let newName = document.getElementById('routeName').value;

      if($("#files").val() === "options"){
        alert("Error: No file selected");
      } else{
        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/addRoute',   //The server endpoint we are connecting to
          data: {
            data: listNewWaypoints,
            filename: $("#files").val(),
            newName: newName
          },
          success: function(data){
            if(data.result == -1){
              alert("Something went wrong adding new route");
            } else{
              console.log("added route");
              Status();

              location.reload();
            }
          },
          fail: function(error) {
            $('#bconsole.log("added route");lah').html("On page load, received error from server");
            console.log(error);
          }
        });
      }

    });
    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
});
