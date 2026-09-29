import {parseTelemetry, isFresh} from './model.mjs';
const DB = 'https://nivelclean-2bc3b-default-rtdb.firebaseio.com';
const STORAGE = 'nivelclean.esp8266.connection.v1';
const $ = id => document.getElementById(id);
const number = (v, digits=1) => v.toLocaleString('pt-BR', {minimumFractionDigits:digits,maximumFractionDigits:digits});
let config = null, session = null, sample = null, demo = false, busy = false, generation = 0, timer = null, networkError = '', inFlight = new Set();
const validConfig = c => c && typeof c.apiKey === 'string' && /^[A-Za-z0-9_-]{15,100}$/.test(c.apiKey) && /^[A-Za-z0-9_-]{1,128}$/.test(c.deviceUid);
try { const stored = JSON.parse(localStorage.getItem(STORAGE)); if (validConfig(stored)) config = stored; } catch {}
if(config){$('apiKey').value=config.apiKey;$('deviceUid').value=config.deviceUid;$('setupSummary').textContent='Configuração salva · entre para acompanhar';}
function status(title,text,badge,kind=''){$('statusTitle').textContent=title;$('statusText').textContent=text;$('connection').textContent=badge;$('connection').className='badge '+kind;}
function clearValues(){ $('percentage').textContent='—';$('distance').textContent='—';$('rssi').textContent='—';$('wifiDescription').textContent='Aguardando o dispositivo';$('levelLabel').textContent='Sem leitura';$('water').style.height='0%';$('gauge').removeAttribute('aria-valuenow');$('gauge').setAttribute('aria-valuetext','Sem leitura');$('gaugeLabel').textContent='SEM DADOS';$('levelDescription').textContent='Nenhuma medição atual disponível.'; }
function render(){
  clearValues();
  $('updated').textContent=sample?new Date(sample.timestamp).toLocaleTimeString('pt-BR'):'—';
  $('age').textContent=sample?new Date(sample.timestamp).toLocaleDateString('pt-BR')+' · há '+Math.max(0,Math.floor((Date.now()-sample.timestamp)/1000))+' s':'Nenhum dado recebido ainda.';
  $('readingOrigin').textContent=demo?'Demonstração · dados simulados':'Medição pelo HC-SR04';
  if(networkError){status('Não foi possível atualizar',networkError,'Sem conexão','error');return;}
  if(!sample){if(session)status('Aguardando a primeira medição','Confira o Wi-Fi, o UID da placa e o Monitor Serial do Arduino IDE.','Aguardando sensor');else status('Vamos conectar seu reservatório','Informe os dados do Firebase abaixo. O painel só exibirá valores reais quando a placa enviar uma medição válida.',config?'Aguardando acesso':'Aguardando configuração');return;}
  if(!isFresh(sample)){status('Dispositivo sem atualização','A última medição tem mais de 45 segundos ou o relógio está incorreto. Verifique a placa e sua conexão.','Leitura desatualizada','warn');$('levelLabel').textContent='Desatualizado';return;}
  if(Number.isFinite(sample.rssi)){$('rssi').textContent=sample.rssi+' dBm';$('wifiDescription').textContent=sample.rssi>=-60?'Sinal forte':sample.rssi>=-75?'Sinal razoável':'Sinal fraco';}
  if(sample.status!=='ok'){status('O sensor não recebeu um eco válido','Confira as ligações, a posição do sensor e a superfície da água. Falha de leitura não significa caixa vazia.','Sensor sem leitura','warn');$('levelLabel').textContent='Sem eco';return;}
  $('distance').textContent=number(sample.distanceCm);
  if(sample.percent===null){status('Falta calibrar o reservatório','A distância já está disponível. No nivelclean_config.h, informe as distâncias com a caixa vazia e cheia e confirme CALIBRATED.','Sensor conectado','live');$('levelDescription').textContent='Calibre a caixa para obter o percentual.';$('levelLabel').textContent='Sem calibração';return;}
  const p=sample.percent;
  $('percentage').textContent=number(p,0);$('water').style.height=p+'%';$('gauge').setAttribute('aria-valuenow',p.toFixed(1));$('gauge').setAttribute('aria-valuetext',number(p,0)+' por cento');$('gaugeLabel').textContent='';
  $('levelLabel').textContent=p<=20?'Nível baixo':p>=95?'Nível alto':'Nível normal';
  $('levelDescription').textContent=p<=20?'O reservatório está com pouca água.':p>=95?'O reservatório está próximo do nível máximo.':'Seu reservatório está sendo monitorado.';
  if(demo)status('Você está no modo de demonstração','Arraste o controle para testar o indicador. Estes valores não vêm da sua caixa d’água.','Dados simulados','warn');
  else status('Medição atualizada',p<=20?'Nível de água abaixo de 20%.':p>=95?'Nível de água igual ou acima de 95%.':'Recebendo as medições do ESP8266 pelo Firebase.','Sensor conectado','live');
}
async function request(url, options={}){
  const controller=new AbortController();inFlight.add(controller);const timeout=setTimeout(()=>controller.abort(),15000);
  try{const r=await fetch(url,{...options,signal:controller.signal,cache:'no-store',referrerPolicy:'no-referrer'});const data=await r.json().catch(()=>null);if(!r.ok){const code=data?.error?.message || data?.error || 'HTTP '+r.status;const err=new Error(typeof code==='string'?code:'HTTP '+r.status);err.http=r.status;throw err;}return data;}
  finally{clearTimeout(timeout);inFlight.delete(controller);}
}
function friendly(e){
  if(e.name==='AbortError')return 'A conexão demorou demais. Tente novamente.';
  if(/INVALID_LOGIN|INVALID_PASSWORD|EMAIL_NOT_FOUND|USER_DISABLED/.test(e.message))return 'Confira o e-mail, a senha e se o usuário está ativo no Firebase.';
  if(/API_KEY|key not valid/.test(e.message))return 'A chave de API Web está incorreta. Copie a chave do projeto Firebase.';
  if(/OPERATION_NOT_ALLOWED|CONFIGURATION_NOT_FOUND/.test(e.message))return 'Ative o acesso por E-mail/senha em Authentication no Firebase.';
  if(/TOO_MANY_ATTEMPTS/.test(e.message))return 'Muitas tentativas de acesso. Aguarde alguns minutos.';
  if(e.http===401||e.http===403||/Permission denied/i.test(e.message))return 'Acesso não autorizado. Confira as regras e a permissão deste usuário para o UID da placa.';
  if(/Failed to fetch|NetworkError|Load failed/.test(e.message))return 'Não foi possível acessar o Firebase. Confira a internet e as restrições da chave de API.';
  return e.message || 'Não foi possível conectar.';
}
function disconnect(){generation++;clearTimeout(timer);for(const c of inFlight)c.abort();session=null;sample=null;busy=false;networkError='';$('logoutButton').hidden=true;$('refreshButton').disabled=true;$('loginButton').disabled=false;$('loginButton').textContent='Conectar reservatório';}
async function poll(){
  if(!session||demo||busy)return;
  busy=true;const g=generation;clearTimeout(timer);$('refreshButton').disabled=true;
  try{
    if(Date.now()>=session.expiresAt-60000){const d=await request('https://securetoken.googleapis.com/v1/token?key='+encodeURIComponent(config.apiKey),{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams({grant_type:'refresh_token',refresh_token:session.refreshToken}).toString()});if(g!==generation)return;if(!d?.id_token||!d.refresh_token)throw new Error('A sessão expirou. Entre novamente.');session={token:d.id_token,refreshToken:d.refresh_token,expiresAt:Date.now()+Number(d.expires_in)*1000};}
    const data=await request(DB+'/nivelclean/devices/'+encodeURIComponent(config.deviceUid)+'/telemetry.json?auth='+encodeURIComponent(session.token));if(g!==generation)return;sample=parseTelemetry(data);networkError='';
  }catch(e){if(g===generation)networkError=friendly(e);}
  finally{if(g===generation){busy=false;$('refreshButton').disabled=!session;render();if(session&&!demo)timer=setTimeout(poll,10000);}}
}
$('configForm').addEventListener('submit',e=>{e.preventDefault();const next={apiKey:$('apiKey').value.trim(),deviceUid:$('deviceUid').value.trim()};if(!validConfig(next)){$('formMessage').textContent='Confira a chave de API e o UID. Eles não devem conter espaços.';return;}disconnect();demo=false;$('demoPanel').hidden=true;$('demoButton').textContent='Testar visual';config=next;try{localStorage.setItem(STORAGE,JSON.stringify(config));$('formMessage').textContent='Configuração salva. Agora entre com o usuário do painel.';}catch{$('formMessage').textContent='Configuração pronta para esta sessão. O navegador não permitiu salvá-la.';}$('setupSummary').textContent='Configuração pronta · entre para acompanhar';render();});
$('loginForm').addEventListener('submit',async e=>{e.preventDefault();if(!config){$('formMessage').textContent='Salve a chave de API e o UID da placa primeiro.';return;}disconnect();demo=false;$('demoPanel').hidden=true;$('demoButton').textContent='Testar visual';const g=generation;const email=$('email').value.trim(),password=$('password').value;$('password').value='';$('loginButton').disabled=true;$('loginButton').textContent='Conectando…';$('formMessage').textContent='Autenticando no Firebase…';render();
  try{const d=await request('https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key='+encodeURIComponent(config.apiKey),{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({email,password,returnSecureToken:true})});if(g!==generation)return;if(!d?.idToken||!d.refreshToken)throw new Error('Resposta de autenticação inválida.');session={token:d.idToken,refreshToken:d.refreshToken,expiresAt:Date.now()+Number(d.expiresIn)*1000};$('logoutButton').hidden=false;$('setupSummary').textContent='Sessão conectada';$('formMessage').textContent='Acesso realizado.';$('setup').open=false;await poll();}
  catch(err){if(g===generation){$('formMessage').textContent=friendly(err);networkError=friendly(err);render();}}
  finally{if(g===generation){$('loginButton').disabled=false;$('loginButton').textContent='Conectar reservatório';}}
});
$('logoutButton').addEventListener('click',()=>{disconnect();$('setup').open=true;$('formMessage').textContent='Você saiu do painel.';$('setupSummary').textContent='Configuração salva · entre para acompanhar';render();});
$('refreshButton').addEventListener('click',poll);
function demoSample(){const p=Number($('demoSlider').value);$('demoOutput').textContent=p+'%';sample=parseTelemetry({status:'ok',calibrated:true,timestamp:Date.now(),distanceCm:110-p,emptyCm:110,fullCm:10,rssi:-54});render();}
$('demoButton').addEventListener('click',()=>{const next=!demo;disconnect();demo=next;$('demoPanel').hidden=!demo;$('demoButton').textContent=demo?'Encerrar teste':'Testar visual';$('setup').open=!demo;$('formMessage').textContent='';if(demo)demoSample();else render();});
$('demoSlider').addEventListener('input',demoSample);
setInterval(()=>{if(demo){sample.timestamp=Date.now();}render();},1000);
render();
