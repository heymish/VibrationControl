#include "WebPages.h"

namespace WebPages {

String controlPage(uint8_t currentSpeed, uint32_t updateDelayMs) {
  String page = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Motor Controller</title>
<style>
:root{font-family:Arial,sans-serif;color:#172033;background:#eef2f7}
body{margin:0;padding:20px}.card{max-width:560px;margin:auto;background:#fff;padding:24px;border-radius:20px;box-shadow:0 10px 35px #1020281f}
h1,p{text-align:center}.speed{font-size:4rem;font-weight:700;color:#1769e0;text-align:center;margin:24px 0}input[type=range]{width:100%;accent-color:#1769e0}
.grid{display:grid;grid-template-columns:repeat(5,1fr);gap:8px;margin-top:18px}button,a{box-sizing:border-box;border:0;border-radius:12px;padding:13px;font-weight:700;cursor:pointer;text-decoration:none;text-align:center}
.quick{background:#e8f1ff;color:#1057bd}.stop{display:block;width:100%;margin-top:18px;background:#c62828;color:#fff}.setup{display:block;margin-top:12px;background:#52637a;color:#fff}
.status{margin-top:20px;padding:15px;background:#f8fafc;border:1px solid #d0d5dd;border-radius:12px}.row{display:flex;justify-content:space-between;padding:6px 0}.msg{text-align:center;min-height:24px;margin-top:12px;color:#157347}.error{color:#c62828}
@media(max-width:420px){body{padding:10px}.grid{grid-template-columns:repeat(3,1fr)}.speed{font-size:3.3rem}}
</style>
</head>
<body><main class="card">
<h1>Motor Controller</h1><p>HY-M0.2 speed control</p>
<div id="display" class="speed">)HTML";

  page += String(currentSpeed);
  page += R"HTML(%</div>
<input id="slider" type="range" min="0" max="100" value=")HTML";
  page += String(currentSpeed);
  page += R"HTML(" step="1" aria-label="Motor speed">
<div class="grid">
<button class="quick" onclick="quick(0)">0%</button><button class="quick" onclick="quick(25)">25%</button>
<button class="quick" onclick="quick(50)">50%</button><button class="quick" onclick="quick(75)">75%</button>
<button class="quick" onclick="quick(100)">100%</button>
</div>
<button class="stop" onclick="stopMotor()">STOP</button>
<a class="setup" href="/setup">Wi-Fi setup</a>
<div id="message" class="msg">Ready</div>
<div class="status">
<div class="row"><span>Network mode</span><strong id="mode">Loading</strong></div>
<div class="row"><span>Wi-Fi</span><strong id="ssid">Loading</strong></div>
<div class="row"><span>IP address</span><strong id="ip">Loading</strong></div>
<div class="row"><span>Signal</span><strong id="rssi">Loading</strong></div>
<div class="row"><span>PWM duty</span><strong id="duty">0</strong></div>
</div>
</main>
<script>
const slider=document.getElementById('slider'),display=document.getElementById('display'),message=document.getElementById('message');
let timer=null,running=false,pending=null;
function local(v){v=Math.max(0,Math.min(100,Number(v)));slider.value=v;display.textContent=v+'%'}
function msg(t,e=false){message.textContent=t;message.classList.toggle('error',e)}
async function send(v){pending=Math.max(0,Math.min(100,Number(v)));if(running)return;running=true;while(pending!==null){const x=pending;pending=null;try{const r=await fetch('/api/speed?value='+encodeURIComponent(x),{cache:'no-store'});const d=await r.json();if(!r.ok)throw new Error(d.error||'Request failed');local(d.speed);msg('Speed set to '+d.speed+'%')}catch(e){msg(e.message||'Controller unavailable',true)}}running=false}
slider.addEventListener('input',()=>{local(slider.value);clearTimeout(timer);timer=setTimeout(()=>send(slider.value),)HTML";
  page += String(updateDelayMs);
  page += R"HTML()});
function quick(v){clearTimeout(timer);local(v);send(v)}
async function stopMotor(){clearTimeout(timer);pending=null;try{const r=await fetch('/api/stop',{method:'POST',cache:'no-store'});const d=await r.json();if(!r.ok)throw new Error(d.error||'Stop failed');local(0);msg('Motor stopped')}catch(e){msg(e.message||'Controller unavailable',true)}}
async function status(){try{const r=await fetch('/api/status',{cache:'no-store'});const d=await r.json();if(!r.ok)throw new Error('Status failed');document.getElementById('mode').textContent=d.mode;document.getElementById('ssid').textContent=d.ssid||'Not connected';document.getElementById('ip').textContent=d.ip;document.getElementById('rssi').textContent=d.rssi===null?'Not available':d.rssi+' dBm';document.getElementById('duty').textContent=d.pwmDuty;if(!running&&pending===null)local(d.speed)}catch(e){msg('Status connection lost',true)}}
status();setInterval(status,2000);
</script></body></html>)HTML";
  return page;
}

String setupPage() {
  return R"HTML(<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Wi-Fi Setup</title>
<style>:root{font-family:Arial,sans-serif;background:#eef2f7;color:#172033}body{padding:20px}.card{max-width:560px;margin:auto;background:#fff;padding:24px;border-radius:20px;box-shadow:0 10px 35px #1020281f}label{display:block;margin:18px 0 6px;font-weight:700}input,select,button,a{box-sizing:border-box;width:100%;min-height:48px;padding:11px;border-radius:10px;font-size:1rem}button,a{display:block;margin-top:18px;border:0;background:#1769e0;color:#fff;font-weight:700;text-align:center;text-decoration:none}.danger{background:#c62828}.back{background:#52637a}.notice{padding:14px;background:#edf4ff;border-radius:10px}</style></head>
<body><main class="card"><h1>Wi-Fi setup</h1><div class="notice">Select a nearby network or enter its name manually. Saving restarts the controller.</div>
<form method="post" action="/save-wifi"><label for="networks">Nearby networks</label><select id="networks" onchange="choose(this.value)"><option value="">Scan results loading...</option></select>
<label for="ssid">Network name / SSID</label><input id="ssid" name="ssid" maxlength="32" required autocomplete="off">
<label for="password">Wi-Fi password</label><input id="password" name="password" type="password" maxlength="63" autocomplete="new-password">
<button type="submit">Save and connect</button></form>
<form method="post" action="/clear-wifi"><button class="danger" type="submit">Erase saved Wi-Fi details</button></form><a class="back" href="/">Back to motor control</a></main>
<script>function choose(v){if(v){document.getElementById('ssid').value=v;document.getElementById('password').focus()}}async function scan(){try{const r=await fetch('/api/networks',{cache:'no-store'});const d=await r.json();const s=document.getElementById('networks');s.innerHTML='<option value="">Enter network manually</option>';d.networks.forEach(n=>{const o=document.createElement('option');o.value=n.ssid;o.textContent=n.ssid+' ('+n.rssi+' dBm, '+(n.secure?'secured':'open')+')';s.appendChild(o)})}catch(e){document.getElementById('networks').innerHTML='<option value="">Scan unavailable; enter manually</option>'}}scan();</script></body></html>)HTML";
}

String savedPage(const String& escapedSSID, const String& apSSID) {
  return "<!doctype html><html><meta name='viewport' content='width=device-width,initial-scale=1'><body style='font-family:Arial;padding:24px'><h1>Wi-Fi saved</h1><p>The controller will restart and connect to <strong>" + escapedSSID + "</strong>.</p><p>If connection fails, reconnect to <strong>" + apSSID + "</strong>.</p></body></html>";
}

String erasedPage() {
  return "<!doctype html><html><meta name='viewport' content='width=device-width,initial-scale=1'><body style='font-family:Arial;padding:24px'><h1>Wi-Fi erased</h1><p>The controller will restart in setup mode.</p></body></html>";
}

} // namespace WebPages
