// O UART virtual do Cirkit recebe no maximo 128 bytes por envio.
// Comandos curtos evitam perder o final do token; nada e persistido aqui.
export function simulatorCommands(uid, token, epochSeconds = Math.floor(Date.now() / 1000)) {
  if (!/^[A-Za-z0-9_-]{1,128}$/.test(uid) || uid.length > 80) throw new Error('UID incompatível com o Monitor Serial.');
  if (typeof token !== 'string' || token.length < 100 || token.length >= 4000 || !/^[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+$/.test(token)) throw new Error('Sessão inválida. Entre novamente.');
  if (!Number.isInteger(epochSeconds) || epochSeconds < 1767225600 || epochSeconds >= 2145916800) throw new Error('Confira a data e a hora do computador.');
  const commands = [`TIME ${epochSeconds};`, `AUTH_BEGIN ${uid};`];
  for (let offset = 0; offset < token.length; offset += 70) commands.push(`AUTH_PART ${token.slice(offset, offset + 70)};`);
  commands.push('AUTH_END;');
  return commands;
}
