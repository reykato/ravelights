var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var eventData = event.data;
    console.log(eventData);
    //document.getElementById('state').innerHTML = eventData;
    if (eventData == 'on' || eventData == 'off') {
      document.getElementById('toggle').checked = (eventData == 'on');
    } else if (eventData[0] == "#") {
      document.getElementById('hueRange').value = eventData;
    } else if (eventData[0] == "V") {
      // do nothing because there is no battery
    } else if (eventData != "DB_HIT") {
      document.getElementById('brightnessSlider').value = eventData;
    }
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('toggle').addEventListener('click', sendMessage.bind(null, 'toggle'));
    document.getElementById('hueRange').addEventListener('input', onHueChange);
    document.getElementById('mode0').addEventListener('click', sendMessage.bind(null, 'mode0'));
    document.getElementById('mode1').addEventListener('click', sendMessage.bind(null, 'mode1'));
    document.getElementById('mode2').addEventListener('click', sendMessage.bind(null, 'mode2'));
    document.getElementById('mode3').addEventListener('click', sendMessage.bind(null, 'mode3'));
    document.getElementById('manualhit').addEventListener('click', sendMessage.bind(null, 'DB_HIT'));
    document.getElementById('brightnessSlider').addEventListener('input', onBrightnessChange);
  }
  function sendMessage(msg) {
    console.log(msg);
    websocket.send(msg);
  }
  function onHueChange() {
    var color = document.getElementById('hueRange').value;
    console.log(color);
    websocket.send(color);
  }
  function onBrightnessChange() {
    var brightness = document.getElementById('brightnessSlider').value;
    console.log(brightness);
    websocket.send(brightness);
  }