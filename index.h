#ifndef INDEX_H
#define INDEX_H
const char MAIN_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PORTAL S3</title>
    <style>
        body { font-family: 'Segoe UI', sans-serif; background: #0a0a0c; color: #fff; margin: 0; padding: 10px; text-align: center; }
        .card { background: #16161d; border-radius: 16px; padding: 15px; margin-bottom: 15px; border: 1px solid #2d2d35; box-shadow: 0 4px 10px rgba(0,0,0,0.3); }
        h3 { margin: 0 0 10px 0; color: #00f2ff; text-transform: uppercase; font-size: 1rem; }
        button { width: 100%; padding: 14px; border-radius: 12px; border: none; font-weight: bold; margin: 5px 0; cursor: pointer; font-size: 1rem; }
        .btn-on { background: #00f2ff; color: #000; box-shadow: 0 0 10px rgba(0,242,255,0.3); }
        .btn-off { background: #333; color: #fff; }
        input, select { width: 100%; padding: 12px; margin: 8px 0; background: #222; color: #fff; border: 1px solid #444; border-radius: 8px; box-sizing: border-box; font-size: 1rem; }
        #canvas { background: #000; border: 1px solid #444; touch-action: none; width: 100%; max-width: 400px; image-rendering: pixelated; border-radius: 4px; }
        .ai-btn { background: linear-gradient(45deg, #a855f7, #00f2ff); color: white; }
    </style>
</head>
<body>
    <div class="card">
        <h3>Zasilanie i Efekty</h3>
        <button class="btn-on" onclick="send('power=1')">WŁĄCZ</button>
        <button class="btn-off" onclick="send('power=0')">WYŁĄCZ</button>
        <label>Jasność</label>
        <input type="range" min="10" max="255" value="150" onchange="send('bri='+this.value)">
        <select onchange="send('anim='+this.value)">
            <option value="0">Statyczny Kolor</option>
            <option value="2">Matrix (Deszcz)</option>
            <option value="17">Lawa (Magma)</option>
            <option value="100">Tryb Rysowania</option>
        </select>
        <input type="color" onchange="setColor(this.value)" value="#00f2ff" style="height:40px">
    </div>

    <div class="card">
        <h3>Budzik i Harmonogram</h3>
        <input type="time" id="at">
        <button class="btn-off" style="background:#6d28d9" onclick="setAlarm()">USTAW BUDZIK</button>
        <hr style="border-color:#333">
        <div style="display:grid; grid-template-columns: 1fr 1fr; gap:10px">
            <div><input type="time" id="ont"><button class="btn-off" onclick="setSch('on')">Auto ON</button></div>
            <div><input type="time" id="offt"><button class="btn-off" onclick="setSch('off')">Auto OFF</button></div>
        </div>
        <button class="btn-off" style="background:#b91c1c; margin-top:10px" onclick="send('stop=1')">STOP ALARM</button>
    </div>

    <div class="card">
        <h3>Panel Rysowania (28x7)</h3>
        <canvas id="canvas" width="28" height="7"></canvas>
        <button class="btn-off" onclick="clearC()">WYCZYŚĆ</button>
    </div>
    
    <div class="card">
        <h3>AI Gemini</h3>
        <input type="text" id="prompt" placeholder="Opisz wzór (np. ogień, las)...">
        <button class="ai-btn" onclick="askAI()">Generuj Wzór</button>
    </div>

    <script>
        const cv = document.getElementById('canvas');
        const ctx = cv.getContext('2d');
        const apiKey = ""; // Tutaj API Key jest wstrzykiwany, ale w kodzie ESP zostaw pusty

        function send(p) { fetch('/set?' + p).catch(e => console.log(e)); }
        
        function setColor(hex) {
            let r = parseInt(hex.slice(1,3), 16), g = parseInt(hex.slice(3,5), 16), b = parseInt(hex.slice(5,7), 16);
            send(`r=${r}&g=${g}&b=${b}`);
        }

        function setAlarm() {
            let t = document.getElementById('at').value.split(':');
            if(t.length == 2) {
                send(`alarmH=${t[0]}&alarmM=${t[1]}&alarmOn=1`);
                alert("Budzik ustawiony na " + t[0] + ":" + t[1]);
            }
        }

        function setSch(type) {
            let el = document.getElementById(type=='on' ? 'ont' : 'offt');
            let t = el.value.split(':');
            if(t.length == 2) {
                send(`${type}H=${t[0]}&${type}M=${t[1]}&${type}En=1`);
                alert("Harmonogram zapisany");
            }
        }

        // --- OBSŁUGA RYSOWANIA ---
        let drawing = false;
        function getPos(e) {
            const r = cv.getBoundingClientRect();
            const cx = e.touches ? e.touches[0].clientX : e.clientX;
            const cy = e.touches ? e.touches[0].clientY : e.clientY;
            return {
                x: Math.floor((cx - r.left) / (r.width / 28)),
                y: Math.floor((cy - r.top) / (r.height / 7))
            };
        }
        function draw(e) {
            if(!drawing) return;
            const p = getPos(e);
            // Zabezpieczenie przed wyjściem poza zakres
            if(p.x < 0 || p.x >= 28 || p.y < 0 || p.y >= 7) return;
            
            ctx.fillStyle = "#00f2ff"; // Domyślny kolor rysowania (cyjan)
            ctx.fillRect(p.x, p.y, 1, 1);
            
            // Wysyłamy cyan (0, 242, 255)
            fetch(`/draw?x=${p.x}&y=${p.y}&r=0&g=242&b=255`);
        }
        
        cv.addEventListener('mousedown', (e)=>{ drawing=true; send('anim=100'); draw(e); });
        window.addEventListener('mouseup', ()=>{ drawing=false; });
        cv.addEventListener('mousemove', draw);
        
        cv.addEventListener('touchstart', (e)=>{ e.preventDefault(); drawing=true; send('anim=100'); draw(e); });
        window.addEventListener('touchend', ()=>{ drawing=false; });
        cv.addEventListener('touchmove', (e)=>{ e.preventDefault(); draw(e); });

        function clearC() {
            ctx.clearRect(0, 0, 28, 7);
            fetch('/draw?clear=1');
        }

        // --- AI GEMINI ---
        async function askAI(){
            const p = document.getElementById('prompt').value;
            if(!p) return;
            // Prosty request do API (wymaga klucza w URL, tu placeholder)
            // W prawdziwym wdrożeniu klucz byłby w zmiennej środowiskowej
            alert("Funkcja wymaga aktywnego klucza API w kodzie strony.");
        }

        // --- SYNCHRONIZACJA CZASU (FIX) ---
        // Obliczamy czas lokalny w sekundach (Unix Timestamp + Offset strefy)
        // getTimezoneOffset zwraca minuty różnicy od UTC (np. -60 dla UTC+1)
        const localTs = Math.floor((Date.now() - (new Date().getTimezoneOffset() * 60000)) / 1000);
        fetch('/sync?ts=' + localTs);
        
    </script>
</body>
</html>
)=====";
#endif