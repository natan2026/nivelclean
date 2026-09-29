import test from 'node:test';
import assert from 'node:assert/strict';
import {parseTelemetry,isFresh} from '../model.mjs';
const make = extra => ({timestamp:100000,status:'ok',distanceCm:55,calibrated:true,emptyCm:100,fullCm:10,...extra});
test('inverte distancia em nivel e limita extremos',()=>{assert.equal(parseTelemetry(make({})).percent,50);assert.equal(parseTelemetry(make({distanceCm:100})).percent,0);assert.equal(parseTelemetry(make({distanceCm:10})).percent,100);assert.equal(parseTelemetry(make({distanceCm:5})).percent,100);assert.equal(parseTelemetry(make({distanceCm:110})).percent,0);});
test('ausencia, falha e falta de calibracao nunca viram zero',()=>{assert.equal(parseTelemetry(null),null);assert.throws(()=>parseTelemetry(make({distanceCm:null})));assert.throws(()=>parseTelemetry(make({distanceCm:'55'})));assert.equal(parseTelemetry(make({status:'sem_eco',distanceCm:55})).distanceCm,null);assert.equal(parseTelemetry(make({status:'sem_eco'})).percent,null);assert.equal(parseTelemetry(make({calibrated:false})).percent,null);});
test('calibracao invalida e timestamps ausentes sao rejeitados',()=>{assert.throws(()=>parseTelemetry(make({fullCm:100})));assert.throws(()=>parseTelemetry(make({fullCm:1})));assert.throws(()=>parseTelemetry(make({timestamp:null})));assert.throws(()=>parseTelemetry(0));});
test('identifica leitura antiga e relogio futuro',()=>{assert.equal(isFresh(make({}),140000),true);assert.equal(isFresh(make({}),146000),false);assert.equal(isFresh(make({}),0),false);assert.equal(isFresh(null),false);});
