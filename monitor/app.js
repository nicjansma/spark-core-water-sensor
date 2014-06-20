//
// Imports
//
var spark = require('sparknode');
var fs = require('fs');
var twilio = require('twilio');

//
// Config
//
if (!fs.existsSync('./config.json')) {
    console.error('Please create a config.json -- check config.json.sample');
    process.exit();
}

var config = require('./config.json');

console.log('Auth Token: ' + config.sparkAccessToken);
console.log('Device ID:  ' + config.sparkDeviceID);

//
// Spark Core Config
//
var core = new spark.Core({
    accessToken: config.sparkAccessToken,
    id: config.sparkDeviceID
});

//
// Twilio Config
//
var twilioClient = require('twilio')(config.twilioAccountSID, config.twilioAuthToken);

//
// Events
//
core.on('online', function() {
    console.log('Device is online');
    sendTextMessage('Water alarm device is online');
});

core.on('alarm', function(data) {
    console.log('Alarm: ' + data.data);
    sendTextMessage('Water alarm: ' + data.data);
});

core.on('error', function(data) {
    console.error('Error: ' + JSON.stringify(data));
});

//
// Functions
//
function sendTextMessage(message) {
    twilioClient.sendMessage({
        to: config.smsToPhoneNumber,
        from: config.smsFromPhoneNumber,
        body: message
    }, function(err, responseData) {
        if (err) {
            console.error(err);
        } else {
            console.log("'" + message + "' sent to " + config.smsToPhoneNumber);
        }
    });
}