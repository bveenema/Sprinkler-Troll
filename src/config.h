const pin_t SWITCH_PIN = D3;


uint8_t statsAddr = 0;
struct Stats {
  uint8_t version; // 0
  uint32_t duration; // seconds
  uint32_t deadline; // unix time (sunrise)
  uint32_t targetStartTime; // unix time
  uint32_t cityID; // Open Weather API city ID
};
Stats SprinklerStats;

Stats defaultStats = { // 15 minutes, 4/6/18 @ 6:30am EST, 4/6/18 @ 6:15am EST, Claremont NH
                      0,
                      900,
                      1522996200,
                      1522995300,
                      5084633
                    };
