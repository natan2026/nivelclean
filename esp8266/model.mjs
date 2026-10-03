export const STALE_MS = 45000;
export function parseTelemetry(data) {
  if (data === null) return null;
  if (!data || typeof data !== 'object' || Array.isArray(data)) throw new Error('Formato inesperado no Firebase. Use o firmware ESP8266 deste projeto.');
  if (!Number.isFinite(data.timestamp) || data.timestamp <= 0 || !['ok','sem_eco'].includes(data.status) || typeof data.calibrated !== 'boolean') throw new Error('Medição incompleta. Verifique o firmware e o caminho no Firebase.');
  const validDistance = Number.isFinite(data.distanceCm) && data.distanceCm >= 2 && data.distanceCm <= 400;
  if (data.status === 'ok' && !validDistance) throw new Error('Distância inválida recebida do dispositivo.');
  let percent = null;
  if (data.status === 'ok' && data.calibrated) {
    if (!(Number.isFinite(data.emptyCm) && Number.isFinite(data.fullCm) && data.fullCm >= 2 && data.emptyCm <= 400 && data.emptyCm > data.fullCm)) throw new Error('Calibração inválida recebida da placa.');
    percent = Math.max(0, Math.min(100, 100 * (data.emptyCm - data.distanceCm) / (data.emptyCm - data.fullCm)));
  }
  const source = data.source ?? 'unknown';
  if (!['esp8266','simulacao','unknown'].includes(source)) throw new Error('Origem da medição inválida.');
  const turbidity = data.turbidity ?? {status:'not_configured'};
  if (!turbidity || turbidity.status !== 'not_configured' || Object.keys(turbidity).some(k=>k!=='status')) throw new Error('Sensor de turbidez ainda não configurado nesta versão.');
  return {...data, source, turbidity, distanceCm: data.status === 'ok' ? data.distanceCm : null, percent};
}
export function isFresh(sample, now = Date.now()) {
  return Boolean(sample && now - sample.timestamp <= STALE_MS && sample.timestamp - now <= 60000);
}

export function levelClass(percent) {
  if (!Number.isFinite(percent) || percent < 0 || percent > 100) return 'Sem leitura';
  return percent <= 20 ? 'Nível baixo' : percent >= 95 ? 'Nível alto' : 'Nível médio';
}
