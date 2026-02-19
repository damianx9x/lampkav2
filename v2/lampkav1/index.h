#ifndef INDEX_H
#define INDEX_H

const char MAIN_PAGE[] PROGMEM = R"=====(

<!DOCTYPE html>

<html lang="pl">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>PORTAL LAMP S3</title>
<style>
:root { --p: #00f2ff; --s: #bc13fe; --bg: #0a0a0c; --card: #16161a; }
body { font-family: sans-serif; background: var(--bg); color: #fff; margin: 0; padding: 15px; text-align: center; }
.card { background: var(--card); border-radius: 20px; padding: 20px; margin-bottom: 15px; border: 1px solid #222; }
h2 { font-size: 14px; text-transform: uppercase; color: #888; margin-top: 0; letter-spacing: 2px; }
button { width: 100%; padding: 15px; border-radius: 12px; border: none; font-weight: bold; margin: 5px 0; cursor: pointer; transition: 0.3s; }
.btn-p { background: var(--p); color: #000; }
.btn-s { background: var(--s); color: #fff; }
.btn-off { background: #333; color: #fff; }
input, select { width: 100%; padding: 12px; margin: 8px 0; background: #222; color: #fff; border: 1px solid #444; border-radius: 8px; box-sizing: border-box; }
.row { display: flex; gap: 10px; align-items: center; }
input[type="range"] { accent-color: var(--p); }
.color-preview { height: 50px; border-radius: 10px; cursor: pointer; border: 2px solid #fff; }
</style>
</head>
<body>
<h1>PORTAL S3</h1>

<div class="card">
    <div class="row">
        <button class="btn-p" onclick="send('power=1')">WŁĄCZ</button>
        <button class="btn-off" onclick="send('power=0')">WYŁĄCZ</button>
    </div>
</div>

<div class="card">
    <h2>Jasność i Efekty</h2>
    <input type="range" min="5" max="255" value="150" onchange="send('bri='+this.value)">
    <select onchange="send('anim='+this.value)">
        <option value="0">Neon Warm</option>
        <option value="2">Matrix Rain</option>
        <option value="11">DNA Helix</option>
        <option value="13">Equalizer</option>
        <option value="14">Radar Scan</option>
        <option value="15">Stranger Things</option>
        <option value="17">Lawa</option>
        <option value="18">Vortex</option>
        <option value="20">Reaktor</option>
    </select>
    <input type="color" onchange="c(this.value)" value="#00f2ff">
</div>

<div class="card">
    <h2>Budzik</h2>
    <div class="row">
        <input type="time" id="at" value="07:00">
        <select id="as" style="width: 150px;">
            <option value="0">Klasyk</option>
            <option value="1">Poranek</option>
            <option value="2">Mario</option>
            <option value="5">SciFi</option>
        </select>
    </div>
    <div class="row">
        <button class="btn-s" onclick="sA()">Ustaw Budzik</button>
        <button class="btn-off" onclick="send('almSnd='+document.getElementById('as').value+'&preview=1')">Testuj</button>
    </div>
    <button class="btn-off" style="background:#422; margin-top:10px" onclick="send('stopAlarm=1')">Zatrzymaj Alarm</button>
</div>

<div class="card">
    <h2>Automatyzacja</h2>
    <div class="row"><input type="time" id="ot"> <button class="btn-off" onclick="sH('on')">ON</button></div>
    <div class="row"><input type="time" id="ft"> <button class="btn-off" onclick="sH('off')">OFF</button></div>
</div>

<script>
    function send(p) { fetch('/set?' + p); }
    function c(h) { 
        let r=parseInt(h.slice(1,3),16), g=parseInt(h.slice(3,5),16), b=parseInt(h.slice(5,7),16); 
        send(`r=${r}&g=${g}&b=${b}`); 
    }
    function sA() {
        let t = document.getElementById('at').value.split(':');
        let s = document.getElementById('as').value;
        send(`alarmH=${t[0]}&alarmM=${t[1]}&alarmOn=1&almSnd=${s}`);
        alert("Budzik zapisany");
    }
    function sH(x) {
        let t = document.getElementById(x=='on'?'ot':'ft').value.split(':');
        send(`${x}H=${t[0]}&${x}M=${t[1]}&${x}En=1`);
        alert("Zapisano harmonogram");
    }
    fetch('/sync?ts=' + Math.floor(Date.now()/1000));
</script>


</body>
</html>
)=====";

#endif