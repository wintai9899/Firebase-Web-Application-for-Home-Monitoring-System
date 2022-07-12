// Create Temperature Gauge
function createTemperatureGauge() {
    var temperatureGauge = new LinearGauge({
        renderTo: 'gauge-temperature',
        width: 120,
        height: 350,
        units: "Temperature C",
        minValue: 0,
        startAngle: 90,
        ticksAngle: 180,
        maxValue: 40,
        colorValueBoxRect: "#189577",
        colorValueBoxRectEnd: "#189577",
        colorValueBoxBackground: "#ffffff",
        valueDec: 2,
        valueInt: 2,
        majorTicks: [
            "0",
            "5",
            "10",
            "15",
            "20",
            "25",
            "30",
            "35",
            "40"
        ],
        minorTicks: 4,
        strokeTicks: true,
        highlights: [{
            "from": 30,
            "to": 40,
            "color": "rgba(200, 50, 50, .75)"
        }],
        colorPlate: "#fff",
        colorBarProgress: "#CC2936",
        colorBarProgressEnd: "#049faa",
        borderShadowWidth: 0,
        borders: false,
        needleType: "arrow",
        needleWidth: 2,
        needleCircleSize: 7,
        needleCircleOuter: true,
        needleCircleInner: false,
        animationDuration: 1500,
        animationRule: "linear",
        barWidth: 10,
    });
    return temperatureGauge;
}

// Create Humidity Gauge
function createHumidityGauge() {
    var humidityGauge = new RadialGauge({
        renderTo: 'gauge-humidity',
        width: 250,
        height: 250,
        units: "Humidity (%)",
        minValue: 0,
        maxValue: 100,
        colorValueBoxRect: "#1ca1e0",
        colorValueBoxRectEnd: "#1ca1e0",
        colorValueBoxBackground: "#ffffff",
        valueInt: 2,
        majorTicks: [
            "0",
            "10",
            "20",
            "30",
            "40",
            "50",
            "60",
            "70",
            "80",
            "90",
            "100"

        ],
        minorTicks: 5,
        strokeTicks: true,
        highlights: [{
                "from": 0,
                "to": 50,
                "color": "#40ff00"
            },
            {
                "from": 50,
                "to": 70,
                "color": "#e0bb12"
            },
            {
                "from": 70,
                "to": 100,
                "color": "#ff0011"
            }
        ],
        colorPlate: "#fff",
        borderShadowWidth: 0,
        borders: false,
        needleType: "line",
        colorNeedle: "#000000",
        colorNeedleEnd: "#000000",
        needleWidth: 2,
        colorNeedleCircleOuter: "#000000",
        needleCircleOuter: true,
        needleCircleInner: true,
        animationDuration: 2000,
        animationRule: "linear",
        needleCircleSize: 3,

    });
    return humidityGauge;
}

// Create Pressure Gauge
function createPressureGauge() {
    var pressureGauge = new RadialGauge({
        renderTo: 'gauge-pressure',
        width: 250,
        height: 250,
        units: "Humidity (%)",
        minValue: 950,
        maxValue: 1050,
        colorValueBoxRect: "#6f65bc",
        colorValueBoxRectEnd: "#6f65bc",
        colorValueBoxBackground: "#ffffff",
        valueInt: 2,
        majorTicks: [
            "950",
            "960",
            "970",
            "980",
            "990",
            "1000",
            "1010",
            "1020",
            "1030",
            "1040",
            "1050"

        ],
        minorTicks: 5,
        strokeTicks: true,
        highlights: [{
                "from": 1000,
                "to": 1030,
                "color": "#40ff00"
            },
            {
                "from": 950,
                "to": 1000,
                "color": "#e0bb12"
            },
            {
                "from": 1030,
                "to": 1050,
                "color": "#e0bb12"
            }
        ],
        colorPlate: "#fff",
        borderShadowWidth: 0,
        borders: false,
        needleType: "line",
        colorNeedle: "#000000",
        colorNeedleEnd: "#000000",
        needleWidth: 2,
        colorNeedleCircleOuter: "#000000",
        needleCircleOuter: true,
        needleCircleInner: true,
        animationDuration: 2000,
        animationRule: "linear",
        needleCircleSize: 3,

    });
    return pressureGauge;
}

// Code referene: https://randomnerdtutorials.com/esp32-web-server-gauges/
// Gauge.js library and demo: https://bernii.github.io/gauge.js/



