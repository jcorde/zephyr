Echo client app copy with the following changes:

1. Removed UDP references and unused files.
2. Added wait_event(int) routine on echo-client.c to detect connection events.
3. Reconnect if connection lost.
