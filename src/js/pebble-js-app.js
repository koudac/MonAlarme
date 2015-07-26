// Definition des variables de la watchapp
var EtatAlarme = 0;
var Username = "koudac";
var Password = "sxC8U5q6";

const ACTION_MSG = 0x3;
const DATA_MSG = 0x5;

const CHANGE_STATE = 0x6;
const REFRESH_STATE = 0x7;


function ReadEtatAlarme(){

	var req = new XMLHttpRequest();
	req.withCredentials = true;
	req.open('GET', "http://bureau.runasyst.fr:3500/RpiWebServeur/GetEtatAlarme.php", true);
	req.setRequestHeader("Authorization", "Basic "+ Base64.encode(Username +":" + Password));
	
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        //console.log(req.responseText);

				// Stock l'etat recu dans la variable EtatAlarme
        EtatAlarme = req.responseText;
				
				console.log("Lecture de l'etat de l'alarme ("+ EtatAlarme +")");
				
				// Envoie la nouvelle valeur de l'etat de l'alarme au prog en C
        Pebble.sendAppMessage({"ETAT_ALARME_KEY":EtatAlarme});

      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
	
}

function ChangeEtatAlarme(NewEtatAlarme){
	console.log("Modification de l'etat de l'alarme ("+ NewEtatAlarme +")");
	
	var EtatAlarmeText = "passage";
	if(NewEtatAlarme == 3 ) EtatAlarmeText = "passage";
	else if(NewEtatAlarme == 2 ) EtatAlarmeText = "off";
	else if(NewEtatAlarme == 1 ) EtatAlarmeText = "on";
	
	var req = new XMLHttpRequest();
	req.withCredentials = true;
	req.open('GET', "http://bureau.runasyst.fr:3500/RpiWebServeur/ChangeAlarmState.php?Etat="+EtatAlarmeText, true);
	req.setRequestHeader("Authorization", "Basic "+ Base64.encode(Username +":" + Password));
	
	req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {

				console.log("Etat modifier correctement...");

				ReadEtatAlarme();
				
      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
}

Pebble.addEventListener("ready", function(e) {
  console.log("connect!" + e.ready);
	EtatAlarme = ReadEtatAlarme();
});

Pebble.addEventListener("appmessage", function(e) {
	var msg = JSON.parse(JSON.stringify(e.payload));
	console.log('Received message: ' + JSON.stringify(e.payload));
	
	// Si c'est un message d'action alors...
	if(msg.MSG_TYPE == ACTION_MSG){
		switch(msg.ACTION_ID){
			case CHANGE_STATE:
				console.log('KeyPressed value: ' + msg.ACTION_DATA );
	
				// Change l'etat de l'alarme
  			ChangeEtatAlarme(msg.ACTION_DATA);
				break;
				
			case REFRESH_STATE:
				ReadEtatAlarme();
				break;
				
			default:
				console.log('Action non géré...');
				break;
		} 
	}
	else if(msg.MSG_TYPE == DATA_MSG){
			console.log("data msg non implémenté...");
	}
	else console.log("Message non géré...");
	
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("webview closed");
  console.log(e.type);
  console.log(e.response);
});


