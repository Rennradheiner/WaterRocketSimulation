// Rocket class

class Rocket
{
  public:
    Rocket(float pressure, float waterVolume_l, float nozzleDiameter_mm, float bottleVolume_l, float emptyRocketMass);

    void step(float const dt);

    float getAltitude() const { return m_rocketAltitude_m; }

    float getMaxAltitude() const { return m_maxAltitude; }

    void logData();

    void printVerbose();

    void printResult();

  private:
    float calcNozzleSpeed();

    void updateWaterVolume(float const dt);

    void updatePressure();

  private:
    float m_pressure_pa;
    float m_waterVolume_m3;
    float m_prevWaterVolume_m3;
    float const m_nozzleDiameter_mm;
    float const m_bottleVolume_m3;
    float const m_nozzleArea;
    float m_rocketSpeed_ms;
    float m_previousRocketSpeed_ms;
    float m_rocketAltitude_m;
    float const m_emptyRocketMass_kg;
    float m_nozzleSpeed_ms;
    float m_elapsedTime_s;
    float m_maxSpeed;
    float m_ascendDuration;
    float m_maxAltitude;
    float m_burnDuration;
    float m_flightDuration;
    float m_endPressure;

  private:
    static constexpr float WATER_DENSITY{1000.0f}; // kg/m^3
    static constexpr float GRAVITY{9.81f};
    static constexpr float BAR_2_PASCAL{100000.0f};
    static constexpr float LITER_2_CUBICMETERS{0.001f};
    static constexpr float HEAT_CAPACITY_RATIO_AIR{1.4f};
};
