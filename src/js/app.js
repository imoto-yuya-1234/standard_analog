Pebble.addEventListener("ready",
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
		var url = "https://imoto-yuya-1234.github.io/standard_analog/index_rot.html";
    Pebble.openURL(url);
		console.log("Showing configuration page: " + url);
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
		
		var configData = JSON.parse(decodeURIComponent(e.response));
  	console.log('Configuration page returned: ' + JSON.stringify(configData));

  	var dict = {};
		
  	var invert = configData['invert'];  	
    dict['KEY_INVERT'] = configData['invert'] ? 1 : 0;  // Send a boolean as an integer
		
  	var connection = configData['connection'];  	
    dict['KEY_CONNECTION'] = configData['connection'] ? 1 : 0;  // Send a boolean as an integer
		
		var show_ticks = configData['show_ticks'];
		dict['KEY_SHOW_TICKS'] = configData['show_ticks'] ? 1 : 0;
		
		var show_day = configData['show_day'];
		dict['KEY_SHOW_DAY'] = configData['show_day'] ? 1 : 0;
		
		var show_second = configData['show_second'];
		dict['KEY_SHOW_SECOND'] = configData['show_second'] ? 1 : 0;
  	
		var show_battery = configData['show_battery'];
		dict['KEY_SHOW_BATTERY'] = configData['show_battery'] ? 1 : 0;
		
		var lang = configData['lang'];
		dict['KEY_LANG'] = configData['lang'];
		
		var rotate = configData['rotate'];
		dict['KEY_ROTATE'] = configData['rotate'] ? 1 : 0;
  	
		// Send to watchapp
		Pebble.sendAppMessage(dict, function() {
			console.log('Send successful: ' + JSON.stringify(dict));
		}, function() {
			console.log('Send failed!');
		});
  }
);