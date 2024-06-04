# Web Server to host your data and receive the values from sensors connected

Before you upload this sketch to your ESP, you shoudl go through a few steps:
1. add a wifi network to the file `wifi_config.json` located in `/data/static`;
2. upload the sketch to the microcontroller;
3. go to `http://esp8266-web-server.local/`;
4. upload your HTML file (can be in both formats: .html and .html.gz);
5. upload any other resources needed for your page and DON'T FORGET to add `<link href="/css" rel="stylesheet" />` and/or `<script src="/js"></script>` (use defer or async if you put the script tag in the head of your document but want it to be executed after the DOM tree has been initialised);
6. go to `http://esp8266-web-server.local/view` to see your page in action
