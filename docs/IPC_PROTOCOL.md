# WBSJ WE local IPC — protocol v1

- Endpoint: `WBSJ_WE_RecordingService_v1`
- Transport: Windows named pipe with a dedicated ACL (Authenticated Users read/write; SYSTEM full control; remote clients rejected); QLocalServer is used for Unix development hosts
- Encoding: UTF-8 JSON
- Framing: newline-delimited JSON (one request and one response per line)
- Maximum request buffer: 8 KiB

## Request

```json
{"protocol":1,"command":"status"}
```

or

```json
{"protocol":1,"command":"ping"}
```

## Status response

```json
{
  "ok": true,
  "state": "Online",
  "message": "Service is running. Camera and recording engines are not enabled in Phase 0.",
  "version": "0.2.0",
  "started_at": "2026-11-10T10:00:00Z",
  "configured_cameras": 0,
  "active_recorders": 0,
  "phase": 0
}
```

The timestamp above is an example schema value, not a live service response.

## Security boundary

Only `status` and `ping` are accepted. The Phase 0 response contains no credentials, video, personal data, camera addresses or control operations. The Windows endpoint is configured for local clients so the desktop can communicate with a service running under the service account. Before any state-changing command is added, replace this broad local status ACL with a restricted named-pipe security descriptor, authenticate the client identity, authorize each operation and audit every command.
