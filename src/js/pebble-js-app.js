// Definition des variables de la watchapp
var EtatAlarme = -1;
var Username = "koudac";
var Passaword = "geraldouc";

function iconFromWeatherId(weatherId) {
  if (weatherId < 11) {
    return 1;
  } else if (weatherId < 26) {
    return 2;
  } else if (weatherId > 25){
    return 3;
  } 
}

function fetchWeather(latitude, longitude) {
  var req = new XMLHttpRequest();
  req.open('GET', "http://api.openweathermap.org/data/2.5/station?id=6792&cnt=1", true); // +
  //  "lat=" + latitude + "&lon=" + longitude + "&cnt=1", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);

        var response = JSON.parse(req.responseText);
        var temperature = Math.round(response.last.main.temp - 273.15);
				var windSpeed = Math.round(response.last.wind.speed*3.6);
        var icon = iconFromWeatherId(windSpeed);
        var city = "Club Modeliste du sud";
        console.log(temperature);
				console.log(windSpeed);
        console.log(icon);
        console.log(city);
        Pebble.sendAppMessage({
          "WEATHER_ICON_KEY":icon,
          "WEATHER_TEMPERATURE_KEY":" " + temperature + "\u00B0C",
					"WEATHER_WIND_KEY":  windSpeed + " Km/h ",
          "WEATHER_CITY_KEY":city}
        );

      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "WEATHER_CITY_KEY":"Loc Unavailable",
    "WEATHER_TEMPERATURE_KEY":"N/A"
  });
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 

function ReadEtatAlarme(){
	console.log("Lecture de l'etat de l'alarme ("+ EtatAlarme +")")
	
}

function ChangeEtatAlarme(NewEtatAlarme){
	console.log("Modification de l'etat de l'alarme ("+ NewEtatAlarme +")")
	
}

Pebble.addEventListener("ready", function(e) {
  console.log("connect!" + e.ready);
	EtatAlarme = ReadEtatAlarme();
	console.log(e.type);
});

Pebble.addEventListener("appmessage", function(e) {
  console.log(e.type);
  console.log(e.payload.temperature);
  console.log("message!");
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("webview closed");
  console.log(e.type);
  console.log(e.response);
});


