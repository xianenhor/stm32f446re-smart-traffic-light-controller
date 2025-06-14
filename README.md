The traffic light system cope with 4 scenario: 

1. Scenario 1: when no car at all junction and no sensor hit, green light on 1.5s, TL run by sequence
2. Scenario 2: when all car arrive at all of the junction and hit the sensor, run TL by sequence with the sensor hit
   only first sensor hit - 15s
   first sensor and second sensor hit - 30s
3. Scenario 3: rewrite sequence if any car detected
4. Scenario 4: each lane only gets to run 3 times max before the reserve is disabled

