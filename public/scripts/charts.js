// Create the charts when the web page loads
window.addEventListener('load', onload);

function onload(event){
  chartT = createTemperatureChart();
  chartH = createHumidityChart();
  chartP = createPressureChart();
}

// Create Temperature Chart
function createTemperatureChart() {
  var temperatureChart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-temperature',
      type: 'spline' 
    },
    series: [
      {
        name: 'BME680'
      }
    ],
    title: { 
      text: undefined
    },
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },

      series: { 
        color: '#189577' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' },
      title:{text:"Time"}
    },
    yAxis: {
      title: { 
        text: 'Temperature °C' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return temperatureChart;
}

// Create Humidity Chart
function createHumidityChart(){
  var humidityChart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-humidity',
      type: 'spline'  
    },
    series: [{
      name: ''
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#1ca1e0' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' },
      title:{text:"Time"}
    },
    yAxis: {
      title: { 
        text: 'Humidity (%)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return humidityChart;
}

// Create Pressure Chart
function createPressureChart() {
  var pressureChart = new Highcharts.Chart({
    chart:{ 
      renderTo:'chart-pressure',
      type: 'spline'  
    },
    series: [{
      name: 'BME680'
    }],
    title: { 
      text: undefined
    },    
    plotOptions: {
      line: { 
        animation: false,
        dataLabels: { 
          enabled: true 
        }
      },
      series: { 
        color: '#6f65bc' 
      }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' },
      title:{text:"Time"}
    },
    yAxis: {
      title: { 
        text: 'Pressure (hPa)' 
      }
    },
    credits: { 
      enabled: false 
    }
  });
  return pressureChart;
}

// Highcharts.js Library and demo: https://www.highcharts.com/