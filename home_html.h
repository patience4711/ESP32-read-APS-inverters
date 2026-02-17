//<link rel="stylesheet" type="text/css" href="/STYLESHEET">

const char home_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32-C3 DIMMER</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link href="data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAu29YA0Q8YALTd3gBm4eMA0+blAIHh4wCa2tsAdd/gALUUnQC24+IAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARERERERERERJkZmRGZkZlEkhIhIhIhKUSSFmFmFmEpRJIVUVUVUSlEkhV3EXcRGUSSZQAAB1YpRJJlczMHVilEiGWIOAdWKESIZYgIh1aIRIhliHiIiIhEiIiIVYhYiUSIZohmaCgpRIgiiCIoiClEiJmImZmJmUREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" rel="icon" type="image/x-icon"/>

<style>
body {font-family:arial; font-size: 12px;}
#demoLPM:hover {
  cursor: pointer;
  background-color: yellow;
  font-size: 20px;
}
.btlabel {text-align:center; color: white;font-size:25px;}

.bt1 {
  font-size:26px;
  width: 60%;
  height: 199px;
  padding: 10px;
  margin:12px;
  text-align:center;
  border-radius:8px;
  color:white;
  background: #8742f5;
  box-shadow: 0 6px #302442;
  display: block;
  }


.bt2 {
  font-size:16px;
  padding: 10px;
  margin:6px;
  text-align:center;
  border-radius:8px;
  color:white;
  background: #8742f5;
  box-shadow: 0 6px #302442;
  display: block;
  }
.nav { background: #eee; padding: 10px; }
.nav a { margin-right: 10px; cursor: pointer; color: blue; text-decoration: none; font-size:20px;}
.container { max-width: 500px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }

.slider {
  -webkit-appearance: none;
  width: 70%;
  height: 25px;
  background: #dcffec;
  outline: none;
  border-radius: 10px;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}
.slider:hover {
  opacity: 1;
}
.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 25px;
  height: 25px;
  border-radius: 50%;
  background: #4CAF50;
  cursor: pointer;
}
.slider::-moz-range-thumb {
  width: 25px;
  height: 25px;
  border-radius: 50%;
  background: #4CAF50;
  cursor: pointer;
}
@media only screen and (max-width: 600px) {

}
</style>
</head>
<body onload="updateUI()">
<div class='container'><center>

<div class='nav'>
<a href="/MENU" style="float:right">menu</a>
<br><br></div>
<br>
<h2>ESP32C3 DIMMER</h2>
<br>
<span style='font-size:11px; margin:auto; display:table; color:blue;'>powered by Hansiart</span>
<br><br>
<button class="bt1" id="toggleBtn" onclick="toggleValue()">on/off</button>
<br>
<center><input type="range" id="duty" name="DT" min="0" max="100" value="{DT}" class="slider" onchange="handlePwm(false)">

<br><br><br><button class="bt2" onClick='handlePwm(true)' id='saveBt'>dim value : <span id="demoLPM"</span></button>
</div><br><br>
</div>

<script>

document.addEventListener("visibilitychange", function() {
    if (!document.hidden){
        console.log("Browser tab is visible")
        updateUI(); // update the page 
    } 
});

// poll every 3 seconds for changes
setInterval(function() {
    fetch('/changed')
        .then(response => response.text())
        .then(state => {
            // Stel dat je server "1" stuurt als de lamp aan is
            if(state === "1") {
                updateUI(true); 
            } else {
                updateUI(false);
            }
        });
}, 3000);
// deze functie reageert op de sliderverandering

function handlePwm(shouldSave) {
  console.log("savePwm");
  var sldr = document.getElementById('duty');
  var veld = document.getElementById('demoLPM');

  //veld.style.color = "red";
  const val = sldr.value;
  const url = shouldSave ? `/submitPwm?pwmVal=${val}&save=1` : `/submitPwm?pwmVal=${val}`;
  // Stuur de waarde naar de ESP32
  fetch(url)
    .then(response => {
        if (response.ok) {
            if (shouldSave) {
                alert("Default dim value saved!"); 
                window.location.href = "/"; 
            } else {
                updateUI(); // Zet de knop op "ON" omdat de lamp brandt
            }
        }
    })
    .catch(err => {
        console.error("Fetch error:", err);
    });
}



// 1. Deze functie doet alleen het visuele werk
function applyDataToUI(data) {
    // Dimmer knop
    const isActive = (data.state === 1);
    const btn = document.getElementById('toggleBtn');
    if (btn) {
        btn.style.backgroundColor = isActive ? "green" : "red";
        btn.innerText = isActive ? "ON" : "OFF";
    }

    // Slider & Label
    const slider = document.getElementById('duty');
    const veld = document.getElementById('demoLPM');
    if (slider) slider.value = data.duty;
    if (veld) veld.innerText = data.duty;

    console.log("UI bijgewerkt met:", data);
}

// 2. Haal status op (bijv. bij laden pagina)
function updateUI() {
    fetch('/status')
        .then(r => r.json())
        .then(data => applyDataToUI(data))
        .catch(err => console.error("Status fout:", err));
}


// get state with page load
window.onload = () => {
    updateUI(); // Je eerdere dimmer status
};


// 3. De toggle knop
function toggleValue() {
    fetch('/toggle')
        .then(r => r.json()) // Zorg dat de ESP32 hier ook JSON stuurt!
        .then(data => applyDataToUI(data))
        .catch(err => console.error("Toggle fout:", err));
}



</script>
</body></html>

)rawliteral";