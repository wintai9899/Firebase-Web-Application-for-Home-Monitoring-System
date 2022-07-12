// DOM elements
let loginElement = document.getElementById('login-form');
let mainContentPage = document.getElementById("main-content");
let displayUserEmail = document.getElementById('user-email');
let authenticationBar = document.getElementById('authentication-bar');

let temperatureChartInputElement = document.getElementById("Tcharts-range");
let humidityChartInputElement = document.getElementById("Hcharts-range");
let pressureChartInputElement = document.getElementById("Pcharts-range");

//const chartsRangeInputElement = document.getElementById('charts-range');
// DOM elements for sensor readings
// const chartsDivElement = document.querySelector('#charts-div');
const temperatureElement = document.querySelector("#temperature");
const humidityElement = document.querySelector("#humidity");
const pressureElement = document.querySelector("#pressure");
const smokeElement = document.querySelector("#smoke");
const lightElement = document.querySelector("#light");
const updateElement = document.querySelector("#lastUpdate")
const timeElement = document.querySelector("#curTime");
// convert epochtime to JavaScripte Date object
function epochToJsDate(epochTime) {
  return new Date(epochTime * 1000);
}

// convert time to human-readable format YYYY/MM/DD HH:MM:SS
function epochToDateTime(epochTime) {
  var epochDate = new Date(epochToJsDate(epochTime));
  var dateTime = epochDate.getFullYear() + "/" +
    ("00" + (epochDate.getMonth() + 1)).slice(-2) + "/" +
    ("00" + epochDate.getDate()).slice(-2) + " " +
    ("00" + epochDate.getHours()).slice(-2) + ":" +
    ("00" + epochDate.getMinutes()).slice(-2) + ":" +
    ("00" + epochDate.getSeconds()).slice(-2);

  return dateTime;
}


// function to plot values on charts
function plotValues(chart, timestamp, value) {
  var x = epochToJsDate(timestamp).getTime();
  var y = Number(value);
  if (chart.series[0].data.length > 40) {
    chart.series[0].addPoint([x, y], true, true, true);
  } else {
    chart.series[0].addPoint([x, y], true, false, true);
  }
}

// Events when user login/logout
const setupUI = (user) => {

  // When the user is logged in
  if (user) {
    // hides the login page and display the content page
    loginElement.style.display = 'none';
    mainContentPage.style.display = 'block';
    authenticationBar.style.display = 'block';
    displayUserEmail.style.display = 'block';
    displayUserEmail.innerHTML = user.email;

    // User id
    var uid = user.uid;
    //console.log(uid);

    // Datapath toretrieve sensor readings 
    var dbPath = 'UsersData/' + uid.toString() + '/readings';

    var TchartPath = 'UsersData/' + uid.toString() + '/Tcharts/range';
    var HchartPath = 'UsersData/' + uid.toString() + '/Hcharts/range';
    var PchartPath = 'UsersData/' + uid.toString() + '/Pcharts/range';

    // Database references
    var dbRef = firebase.database().ref(dbPath);
    // Database chart references
    var tempChartRef = firebase.database().ref(TchartPath);
    var humidChartRef = firebase.database().ref(HchartPath);
    var pressureChartRef = firebase.database().ref(PchartPath);

    // CHARTS
    // Initialize number of values to plot on chart to zero

    var tempChartRange = 0;
    var humidChartRange = 0;
    var presChartRange = 0;

    var currentTime = new Date().toLocaleTimeString('en-GB', { hour: "numeric", minute: "numeric"});
    timeElement.innerHTML = currentTime;
    
    // plot the temperature chart according to the values specified by the user
    tempChartRef.on('value', snapshot => {
      tempChartRange = Number(snapshot.val());
      console.log(tempChartRange);
      // Delete all current data, refresh the chart 
      chartT.destroy();
      // create new chart
      chartT = createTemperatureChart();

      // Get latest readings and update the chart
      dbRef.orderByKey().limitToLast(tempChartRange).on('child_added', snapshot => {
        var jsonData = snapshot.toJSON();

        var temperature = jsonData.temperature;
        var timestamp = jsonData.timestamp;
        // Plot the values on the charts
        plotValues(chartT, timestamp, temperature);
      });
    });

    humidChartRef.on('value', snapshot => {
      humidChartRange = Number(snapshot.val());
      console.log(humidChartRange);

      chartH.destroy();
      chartH = createHumidityChart();
      dbRef.orderByKey().limitToLast(humidChartRange).on('child_added', snapshot => {
        var jsonData = snapshot.toJSON(); 
    
        var humidity = jsonData.humidity;
        var timestamp = jsonData.timestamp;

        plotValues(chartH, timestamp, humidity);
      });
    });

    pressureChartRef.on('value', snapshot => {
      presChartRange = Number(snapshot.val());
      console.log(presChartRange);
     
      chartP.destroy();
     
      chartP = createPressureChart();
      
      dbRef.orderByKey().limitToLast(presChartRange).on('child_added', snapshot => {
        var jsonData = snapshot.toJSON();
       
        var pressure = jsonData.pressure;
        var timestamp = jsonData.timestamp;
      
        plotValues(chartP, timestamp, pressure);
      });
    });


    temperatureChartInputElement.onchange = () => {
      tempChartRef.set(temperatureChartInputElement.value);
    }

    humidityChartInputElement.onchange = () => {
      humidChartRef.set(humidityChartInputElement.value);
    }

    pressureChartInputElement.onchange = () => {
      pressureChartRef.set(pressureChartInputElement.value);
    }

    
    // Get the latest readings
    dbRef.orderByKey().limitToLast(1).on('child_added', snapshot => {

      //get JSON data readings from database
      var jsonData = snapshot.toJSON(); 

      var temperature = jsonData.temperature;
      var humidity = jsonData.humidity;
      var pressure = jsonData.pressure;
      var smoke = jsonData.smoke;
      var luminosity = jsonData.luminosity;
      var timestamp = jsonData.timestamp;

      // Update Sensor Readings in cards
      temperatureElement.innerHTML = temperature;
      humidityElement.innerHTML = humidity;
      pressureElement.innerHTML = pressure;
      smokeElement.innerHTML = smoke;
      lightElement.innerHTML = luminosity;

      // Draw Gauges
      var temperatureGauge = createTemperatureGauge();
      var humidityGauge = createHumidityGauge();
      var pressureGauge = createPressureGauge();

      temperatureGauge.draw();
      humidityGauge.draw();
      pressureGauge.draw();

      temperatureGauge.value = temperature;
      humidityGauge.value = humidity;
      pressureGauge.value = pressure;

      updateElement.innerHTML = epochToDateTime(timestamp);
    });

    // IF USER IS LOGGED OUT
  } else {
    // toggle UI elements
    loginElement.style.display = 'block';
    authenticationBar.style.display = 'none';
    displayUserEmail.style.display = 'none';
    mainContentPage.style.display = 'none';
  }
}

//Code Reference: https://randomnerdtutorials.com/esp32-esp8266-firebase-gauges-charts/
