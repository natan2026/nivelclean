import test from 'node:test';
import assert from 'node:assert/strict';
import {simulatorCommands} from '../simulator-session.mjs';

const token = 'a'.repeat(40) + '.' + 'b'.repeat(1500) + '.' + 'c'.repeat(100);
test('sessão longa atravessa o limite de 128 bytes sem truncamento', () => {
  const commands = simulatorCommands('device_test', token, 1791068400);
  assert.equal(commands[0], 'TIME 1791068400;');
  assert.equal(commands[1], 'AUTH_BEGIN device_test;');
  assert.equal(commands.at(-1), 'AUTH_END;');
  assert.ok(commands.every(command => Buffer.byteLength(command + '\n') < 128));
  assert.equal(commands.slice(2, -1).map(command => command.slice(10, -1)).join(''), token);
});
test('rejeita delimitadores injetados e relógio inválido', () => {
  assert.throws(() => simulatorCommands('uid;SAIR', token, 1791068400));
  assert.throws(() => simulatorCommands('uid', token + ';SAIR', 1791068400));
  assert.throws(() => simulatorCommands('uid', token, 0));
});
