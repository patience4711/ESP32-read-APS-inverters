
const char ECU_EVENTS[] PROGMEM = R"=====(
<!DOCTYPE HTML><html><head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  
  <style>
    html {
      font-family: Arial; 
      display: inline-block; 
      text-align: center;
    }
    p { 
      font-size: 1.2rem;
    }
    body {  
      margin: 0;
    }
    .topnav { 
      overflow: hidden; 
      background-color: #50B8B4; 
      color: white; 
      font-size: 1rem; 
    }
    .content { 
      padding: 20px; 
    }
    .card { 
      background-color: white; 
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); 
    }
    .cards { 
      max-width: 800px; 
      margin: 0 auto; 
      display: grid; 
      grid-gap: 2rem; 
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { 
      font-size: 1.4rem;  
    }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>BME280 WEB SERVER (SSE)</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> p0</p>
        <p><span class="reading"><span id="p0"></span> W</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-tint" style="color:#00add6;"></i> p1</p>
        <p><span class="reading"><span id="p1"></span> W</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-angle-double-down" style="color:#e1e437;"></i> p2</p>
        <p><span class="reading"><span id="p3"></span> W</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');

 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);

 source.addEventListener('message', function(e) {
  console.log("message", e.data);
  var obj = JSON.parse(e.data);
  var e = obj.p0;
  console.log("obj.p0 = " + e);
  document.getElementById("temp").innerHTML = e;
 }, false);

// source.addEventListener('temperature', function(e) {
//  console.log("temperature", e.data);
//  //document.getElementById("temp").innerHTML = e.data;
// }, false);
//
// source.addEventListener('humidity', function(e) {
//  console.log("humidity", e.data);
//  document.getElementById("hum").innerHTML = e.data;
// }, false);
//
// source.addEventListener('pressure', function(e) {
//  console.log("pressure", e.data);
//  document.getElementById("pres").innerHTML = e.data;
// }, false);
}
</script>
</body>
</html>
)=====";
