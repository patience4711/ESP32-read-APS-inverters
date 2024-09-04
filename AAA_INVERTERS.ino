const char INVCONFIG_START[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<script type='text/javascript'>

function showSubmit() {
document.getElementById("sub").style.display = "inline-block";
}

function submitFunction(a) {
document.getElementById('formulier').submit();
}
</script>
<style>
.cap {
  font-weight:bold; 
  Background-color:lightgreen;
 }
div.overlay {
  display: block;
  width: 100%;
  height: 100%;
  background-color: rgba(0,0,0,0.7);
  z-index: 0;
  text-align: center;
  vertical-align: middle;
  line-height: 300px;
}
</style>
<script>
function cl() {
window.location.href='/MENU';
}
</script>
</head>
<body onload='%LOADBAG%'>

<div id='msect'><div id='bo'><div>
<div id='menu' style='height:96px' >
<a href='/MENU' class='close'>&times;</span>
<a href='/INV?welke=0' style='display:%none'0%>inv. 0</a>
<a href='/INV?welke=1' style='display:%none'1%>inv. 1</a>
<a href='/INV?welke=2' style='display:%none'2%>inv. 2</a>
<a href='/INV?welke=3' style='display:%none'3%>inv. 3</a>
<a href='/INV?welke=4' style='display:%none'4%>inv. 4</a>
<a href='/INV?welke=5' style='display:%none'5%>inv. 5</a>
<a href='/INV?welke=6' style='display:%none'6%>inv. 6</a>
<a href='/INV?welke=7' style='display:%none'7%>inv. 7</a>
<a href='/INV?welke=8' style='display:%none'8%>inv. 8</a>
<a href='/INV?welke=99' style='color:#66ff33; display:%none'99%>add</a>
</div>
<center><div class='divstijl' style='height:64vh;'><center>
<form id='formulier' method='get' action='inverterconfig' oninput='showSubmit()' onsubmit="return confirm('sure to save this inverter?')">
    %<FORMPAGE>%
    <br>
  </div>
  </center>
  <form id='formular' method='get' action='/INV_DEL'></form>
  <div id='menu'>
    <div id='pairknop' style='display:%none'p% >
    <a class='groen' href='/PAIR'  onclick="return confirm('Are you sure you want to pair this inverter?')">pair</a>
    <a href='#' onclick='delFunction("/SW=BACK")'>delete</a>
    </div> 
    <a href='#' onclick='helpfunctie()'>help</a>
    <a href="#" id="sub" style='background:green; display: none' onclick='submitFunction("/sw=BACK")'>save</a><br>
</div>
</ul>
<br>
  
</div>
<script type="text/javascript" src="INVSCRIPT"></script>
</body></html>
 )=====";

 const char INVERTER_GENERAL[] PROGMEM = R"=====(
<div id='inverter0' style='display:block'>
    <table><tr><td style='width:160px;'><h4>INVERTER {nr}</h4>
    <td style='width:70px;'><h4>STATUS:</h4><td>
    <input style='width:100px;' class='inp3' value='unpaired' readonly></tr></table>
        
    <br>
    <table style="background-color: lightgreen; padding:10px;">
    
    <tr><td class="cap" style="width:100px;">SERIALNR<td><input class='inp4' id='iv' name='iv' minlength='12' maxlength='12' required value='000000'></input>
    <tr><td class="cap">TYPE<td><select name='invt' class='sb1' id='sel' onchange='myFunction()'>
    <option value='0' invtype_0>YC600</option>
    <option value='2' invtype_2>DS3</option>
    <option value='1' invtype_1>QS1</option></select>
    </tr>
    <tr><td class="cap" >NAME<td class="cap" ><input class='inp4' id='il' name='il' maxlength='12' value='{location}'></input>
    <tr><td class="cap" >DOM. IDX<td class="cap" ><input class='inp2' name='mqidx' value='{idx}' size='4' length='4'></td></tr>
    <tr><td class="cap" >PANELS: 
    <td style='width: 230px;'>
    1&nbsp;<input type='checkbox' name='pan1' #1check>
    2&nbsp;<input type='checkbox' name='pan2' #2check>
    <span id='invspan'>
    3&nbsp;<input type='checkbox' name='pan3' #3check>
    4&nbsp;<input type='checkbox' name='pan4' #4check></tr></td>
    </span>

</table></form></div>
)=====";

// **********************************************************************************
//                         script
// **********************************************************************************

const char INV_SCRIPT[] PROGMEM = R"=====(
function showFunction() {
  //alert("showFunction");
  document.getElementById("invspan").style.display = "inline";
}

function hideFunction() {
  //alert("showFunction");
  document.getElementById("invspan").style.display = "none";
}

function myFunction(){
 if(document.getElementById("sel").value == 1 ) { 
    showFunction();
 } else {
   hideFunction();
 }
}

function delFunction(a) {
  if(confirm("are you sure to delete this inverter ?")) {  
  document.getElementById('formular').submit();   
  }
}

)====="; 




//*******************************************************************************************
//             prepare for saving the data
// *****************************************************************************************
void handleInverterconfig(AsyncWebServerRequest *request) 
{ 
  // form action = handleInverterconfig
  // we only collect the data for this specific inverter
  // collect the serverarguments
   strcpy(Inv_Prop[iKeuze].invLocation, request->arg("il").c_str());
   strcpy(Inv_Prop[iKeuze].invSerial, request->arg("iv").c_str());
   Inv_Prop[iKeuze].invType = request->arg("invt").toInt(); //values are 0 1 2  
   Inv_Prop[iKeuze].invIdx = request->arg("mqidx").toInt(); //values are 0 1  
// the selectboxes
   char tempChar[1] = "";
   if(request->hasParam("pan1")) { Inv_Prop[iKeuze].conPanels[0] = true;} else { Inv_Prop[iKeuze].conPanels[0] = false;}  // mqselect
   if(request->hasParam("pan2")) { Inv_Prop[iKeuze].conPanels[1] = true;} else { Inv_Prop[iKeuze].conPanels[1] = false; }

   Inv_Prop[iKeuze].conPanels[2] = false;
   Inv_Prop[iKeuze].conPanels[3] = false;
   //we only collect this when type = 1
   if(Inv_Prop[iKeuze].invType == 1) {
   if(request->hasParam("pan3")) { Inv_Prop[iKeuze].conPanels[2] = true;}    
   if(request->hasParam("pan4")) { Inv_Prop[iKeuze].conPanels[3] = true;}    
   }
   //DebugPrintln("checked panels are : " + String(Inv_Prop[iKeuze].conPanels[0])+ String(Inv_Prop[iKeuze].conPanels[2])+ String(Inv_Prop[iKeuze].conPanels[2])+ String(Inv_Prop[iKeuze].conPanels[3]));
   //is this a addition?
   String bestand = "/Inv_Prop" + String(iKeuze) + ".str"; // /Inv_Prop0.str
   consoleOut("going to write " + bestand ); 
   //initial their both 0
   writeStruct(bestand, iKeuze); // alles opslaan in SPIFFS
   if(iKeuze == inverterCount) 
   {
    inverterCount += 1;
    consoleOut("we appended, inverterCount now : " + String(inverterCount)); 
    }
   
   basisConfigsave();  // save inverterCount

   consoleOut("\ninverterCount after edit (saved) = " + String(inverterCount));  
   consoleOut("list of the files we have after edit");
   printInverters();
   confirm();
   request->send(200, "text/html", toSend);
}

//*******************************************************************************************
//             delete an inverter
// *****************************************************************************************
void handleInverterdel(AsyncWebServerRequest *request) 
{ 
  // form action = handleInverterconfig
  // we only collect the data for this specific inverter
  // read the serverargs and copy the values into the variables

   String bestand = "/Inv_Prop" + String(iKeuze) + ".str"; // /Inv_Prop0.str
   consoleOut("remove file " + bestand ); 
 
   if(SPIFFS.exists(bestand) ) SPIFFS.remove(bestand);
   
   consoleOut("list of the files we have after removed one");
   printInverters();
   inverterCount -= 1;
   basisConfigsave();  // save inverterCount   
//   // now we may have a gap in the file order
//   // check if we have one and remove it
   remove_gaps();
      //Serial.println(F("list of the files after remove gaps"));
    
    printInverters(); 
    
    consoleOut("inverterCount after removal = " + String(inverterCount));

    confirm();
    request->send(200, "text/html", toSend);
}

void printInverters() { 
      if(diagNose == 0 ) return;     
      consoleOut(F(" ****** excisting inverter files ******"));
      for (int x=0; x < inverterCount+1; x++) 
      {
      String bestand = "/Inv_Prop" + String(x) + ".str";
      
      if(SPIFFS.exists(bestand)) 
          {
              consoleOut("filename: " + bestand);
              printStruct(bestand);
          }
         
      }
}

// say we have
// Inv_prop0.str
// Inv_prop1.str

// Inv_prop3.str
// Inv_Prop4.str

// after we found 3 and not 2 we have
// Inv_prop0.str
// Inv_prop1.str
// inv_prop2.str
// Inv_prop3.str we renamed this one so the gap moved
// Inv_Prop4.str

// remove the gaps
void remove_gaps() {
String bestand_1;
String bestand_2;
bool found = false;  

// say we have Inv_Prop0, Inv_Prop1, Inv_Prop3, Inv_Prop4, Inv_Prop5
// this are 5 files 
// there can only a gap of 1 inverter(can only remove 1 at a time) 
// if we know the inverterCount we can search for a gap and put the last file in it
// so if we are missing Inv_Prop1, we know that we have Inv_prop5  
  
  inverterCount = readInverterfiles(); // this should be 5 in the case above
  for(int i=0; i < inverterCount; i++ ) { // 0 1 2 3 4
  bestand_1 = "/Inv_Prop" + String(i) + ".str";
  // if this file not exixts we know that there must be a file "?inv_Prop inverterCount.str
  if( !SPIFFS.exists(bestand_1) ) {
      consoleOut("found a gap" + bestand_1);  
      bestand_2 = "/Inv_Prop" + String(inverterCount) + ".str"; // the last file
      if( !SPIFFS.exists(bestand_2) ) consoleOut("error, " + bestand_2 + " not exists");  
   // if we rename the last file to the gap, it keeps the old content
   // so we just cope the struct and write that to spiffd
      consoleOut("copy the last struct " + bestand_2 + " to " + bestand_1);
      
      structCopy(i, inverterCount);
      writeStruct(bestand_1, i); // write the copied struct
      SPIFFS.remove(bestand_2);
      //SPIFFS.rename(bestand_2, bestand_1); // file 2 becomes file 1
      return;  
      }
  }
  consoleOut("no gaps found");
} 

   
//  for(int i=0; i < 10; i++ ) 
//  {
//      bestand_1 = "/Inv_Prop" + String(i) + ".str";
//      bestand_2 = "/Inv_Prop" + String(i+1) + ".str";
//      //Serial.println("bestand_1 = " + bestand_1);
//      //Serial.println("bestand_2 = " + bestand_2);
//      if(!SPIFFS.exists(bestand_1) && SPIFFS.exists(bestand_2)) 
//      {
//      //Serial.println(bestand_1 + " not exist and " + bestand_2 + " exists"); 
//        found = true;
//        SPIFFS.rename(bestand_2, bestand_1); // file 2 becomes file 1
//      //Serial.println("renamed " + bestand_1);
//        printInverters();    
//      }
//  }
//  // we remove the last file
//  if (found) 
//    {
//    bestand_1 = "/Inv_Prop" + String(inverterCount) + ".str"; 
//    if(!SPIFFS.exists(bestand_1) ) SPIFFS.remove(bestand_1);
//    }
//}
// ********************************************************************
//                     processor
// *********************************************************************
String processor(const String& var)
{
//
  if(var == "LOADBAG") 
  {
    consoleOut(F("found LOADBAG"));
    if(Inv_Prop[iKeuze].invType == 1) 
      {
      return F("showFunction()"); 
      } else {
      return F("hideFunction()");  
      }
  }
// make the menu items visible --> works
  for(int x=0; x<9; x++) { // for every button we have to set the visibility
     String placeholder = "none'" + String(x);
     //Serial.println("placeholder = " + placeholder);
       if(var == "none'" + String(x) ) { 
        if (x < inverterCount) { return F("inline-block'"); } else { return F("none'"); }
       }
  }
//   
   if(inverterCount < 9) {
    // show the add button 
    if(var == "none'99") return F("inline-block'");   
   } else {
    if(var == "none'99") return F("none'");
   }
  
  if(var == "<FORMPAGE>"){
    consoleOut(F("found FORMPAGE"));
    return(toSend);  
  }

  if(var == "none'p") {
    String bestand = "/Inv_Prop" + String(iKeuze) + ".str";
    if(SPIFFS.exists(bestand)) return "inline-block'"; else return "none'";
  }

return String(); //return empty when no match
}


// construct the form and write in a file toSend
void inverterForm() {
    int verklikker = 0;
    if (inverterCount >= 88 ) // if we add this = 99
    { 
        verklikker = 88;
        inverterCount -= verklikker; // restore the original inverterCount
    }
    inverterCount += verklikker; // add 88 again
    // now we have 3 situations
    // inverterCount == 0, show the page currently no inverters
    // iKeuze < invertercount, we have an existing inverter
    // iKeuze == invertercount, we are adding a new inverter
    // if we clicked the add button then invertercount is at least 88
    if( inverterCount != 0 ) {
   
    // **********************************************************************
    //        construct the inverterpage with actual data
    // **********************************************************************
        if (inverterCount >= 88 ) inverterCount -= 88; // restore inverterCount
        toSend = FPSTR(INVERTER_GENERAL);  
        // is there a file iKeuze then
        String bestand = "/Inv_Prop" + String(iKeuze) + ".str";
        if(SPIFFS.exists(bestand)) 
       {
        consoleOut("File exists " + bestand);
        //the file exists so we can display the values 
        toSend.replace("{nr}" , String(iKeuze)); // vervang inverter nummer not available
        toSend.replace("000000", String(Inv_Prop[iKeuze].invSerial)); // handled by the script
        toSend.replace("{location}", String(Inv_Prop[iKeuze].invLocation));
        toSend.replace("{idx}", String(Inv_Prop[iKeuze].invIdx));
        // the selectboxes
        if (Inv_Prop[iKeuze].conPanels[0]) { toSend.replace("#1check", "checked");}
        if (Inv_Prop[iKeuze].conPanels[1]) { toSend.replace("#2check", "checked");}
                
        if(Inv_Prop[iKeuze].invType != 1 ) { // when the type = yc600 (0) or ds3 (2)
              
            toSend.replace("onload='showFunction()", "onload='hideFunction()" );
            if(Inv_Prop[iKeuze].invType == 0) 
            { 
              toSend.replace("invtype_0", "selected");
            } else {
             toSend.replace("invtype_2", "selected");  
           }
        } else { // inv type == 1 
          
          //Serial.println(" inverter type = 1");
          toSend.replace("invtype_1", "selected");
           if (Inv_Prop[iKeuze].conPanels[2]) { toSend.replace("#3check", "checked");}
           if (Inv_Prop[iKeuze].conPanels[3]) { toSend.replace("#4check", "checked");}
        }
        
        if(String(Inv_Prop[iKeuze].invID) != "0000") 
        {
           toSend.replace("unpaired", String(Inv_Prop[iKeuze].invID) );
        }

        } else {
        // the file does not exist so we show an empty page
        consoleOut("File does not exist");
        toSend.replace("invtype_2", "selected");
        toSend.replace("000000", "");
        toSend.replace("{location}", "");
        toSend.replace("{idx}", "0");
        }

    } else { // so if inverterCount == 0 we present this page
     toSend = "<br><br><br><h3>currently no inverters</h3>"; 
    }
// now we have toSend ready to include in the inverterpage
}

void structCopy(int a, int b) {

//  char invLocation[13] = "N/A";
//  char invSerial[13]   = "000000000000";
//  char invID[5]        = "0000";
//  int  invType         = 0;
//  int  invIdx          = 0;
//  bool conPanels[4]    = {true,true,true,true}; 

   //copy all the values of struct 1 to struct 2  
   strcpy(Inv_Prop[a].invLocation, Inv_Prop[b].invLocation);
   strcpy(Inv_Prop[a].invSerial,   Inv_Prop[b].invSerial);
   strcpy(Inv_Prop[a].invID,   Inv_Prop[b].invID);
   Inv_Prop[a].invType      = Inv_Prop[b].invType;
   Inv_Prop[a].invIdx       = Inv_Prop[b].invIdx;
   Inv_Prop[a].conPanels[0] = Inv_Prop[b].conPanels[0];
   Inv_Prop[a].conPanels[1] = Inv_Prop[b].conPanels[1];
   Inv_Prop[a].conPanels[2] = Inv_Prop[b].conPanels[2];
   Inv_Prop[a].conPanels[3] = Inv_Prop[b].conPanels[3];
   // now write file a and remove file b
}
