import {test,before,after} from 'node:test';
import {readFile} from 'node:fs/promises';
import {initializeTestEnvironment,assertFails,assertSucceeds} from '@firebase/rules-unit-testing';
let env;
const path='nivelclean/devices/board/telemetry';
const sample=()=>({status:'ok',calibrated:true,timestamp:Date.now(),rssi:-60,version:'test',source:'simulacao',distanceCm:60,emptyCm:110,fullCm:10,levelPct:50,turbidity:{status:'not_configured'}});
before(async()=>{
 env=await initializeTestEnvironment({projectId:'demo-nivelclean',database:{host:'127.0.0.1',port:9000,rules:await readFile(new URL('../esp8266.rules.json',import.meta.url),'utf8')}});
 await env.withSecurityRulesDisabled(async ctx=>{await ctx.database().ref().set({nivelclean:{writers:{board:true,other:true},viewers:{viewer:{board:true}},devices:{board:{telemetry:sample()}}},Dados:{legacy:1},distancia:60});});
});
after(async()=>{await env?.cleanup();});
test('somente a placa autorizada escreve sua telemetria',async()=>{
 await assertSucceeds(env.authenticatedContext('board').database().ref(path).set(sample()));
 for(const uid of ['other','viewer','stranger'])await assertFails(env.authenticatedContext(uid).database().ref(path).set(sample()));
 await assertFails(env.unauthenticatedContext().database().ref(path).set(sample()));
 await assertFails(env.authenticatedContext('board').database().ref(path).remove());
});
test('visualizador so le a placa autorizada; anonimo e legado fechados',async()=>{
 await assertSucceeds(env.authenticatedContext('viewer').database().ref(path).once('value'));
 await assertSucceeds(env.authenticatedContext('board').database().ref(path).once('value'));
 await assertFails(env.authenticatedContext('other').database().ref(path).once('value'));
 for(const p of [path,'Dados','distancia']){
  await assertFails(env.unauthenticatedContext().database().ref(p).once('value'));
  await assertFails(env.unauthenticatedContext().database().ref(p).set(1));
 }
 await assertFails(env.authenticatedContext('viewer').database().ref('nivelclean/writers/viewer').set(true));
});
test('rejeita valor invalido, leitura antiga e turbidez inventada',async()=>{
 const ref=env.authenticatedContext('board').database().ref(path);
 for(const override of [{distanceCm:0},{timestamp:Date.now()-120000},{levelPct:101},{calibrated:false},{fullCm:120},{source:'inventada'},{turbidity:{status:'clean'}},{turbidity:{status:'not_configured',ntu:0}},{extra:42}])await assertFails(ref.set({...sample(),...override}));
});
test('falha remove leitura anterior; firmware autenticado antigo compativel',async()=>{
 const ref=env.authenticatedContext('board').database().ref(path);
 const failed={...sample(),status:'sem_eco'};delete failed.distanceCm;delete failed.levelPct;
 await assertSucceeds(ref.set(failed));
 await assertFails(ref.set({...failed,distanceCm:60}));
 const old=sample();delete old.source;delete old.turbidity;
 await assertSucceeds(ref.set(old));
});
