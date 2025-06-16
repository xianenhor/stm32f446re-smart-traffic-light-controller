The traffic light system cope with 3 scenario: 

1. Scenario 1: when no car at all junction and no sensor hit, green light on 1.5s, TL run by sequence (TL1 --> TL2 --> TL3 --> TL4)
2. Scenario 2: when all car arrive at all of the junction and hit the sensor, run TL by sequence with the sensor hit

   -only first sensor hit - 15s
   
   -first sensor and second sensor hit - 30s
  
3. Scenario 3: overwrite the TL sequence if any car detected

   -only first sensor hit - 15s

   -first sensor and second sensor hit - 30s
