function RENAMEMELATER(dropdown){
  $("table.view-panel tbody tr").remove();
  $("#NCDropdown option").remove();
  for(file of fileList){
    if(file.fileName === dropdown.value){
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
              $('#NCDropdown').append($('<option>',{
                value: "track" + " " + i,
                text: track.name
              }));
              let x = track.name;
              let filename = document.getElementById("files");

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
              $('#NCDropdown').append($('<option>',{
                value: "route" + " " + i,
                text: route.name
              }));
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

                $('#files.dropDownOption').append($('<option>',{
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


       let delta = document.getElementById("delta").value;
       // console.log(longitude);
       // console.log(latitude);
       // console.log(delta);

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
    });

    let dropdown = document.getElementById("files");

    dropdown.addEventListener("change", () => {
      RENAMEMELATER(dropdown);
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
            RENAMEMELATER(dropdown);
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

      if(dropdown.value === "options"){
        alert("Error: No file selected");
      } else{
        $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/addRoute',   //The server endpoint we are connecting to
          data: {
            data: listNewWaypoints,
            filename: dropdown.value,
            newName: newName
          },
          success: function(data){
            if(data.result == 0){
              alert("Something went wrong adding new route");
            } else{
              console.log("added route");
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
