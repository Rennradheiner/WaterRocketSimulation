#include <cmath>
#include <iostream>
#include <numbers>
#include <Rocket.hpp>
#include <stdexcept>

using std::numbers::pi_v;

Rocket::Rocket(
    float pressure, float waterVolume_l, float nozzleDiameter_mm, float bottleVolume_l, float emptyRocketMass,
    float rocketDiameter_mm, float dragCoefficient)
    : m_pressure_pa{pressure * BAR_2_PASCAL}
    , m_waterVolume_m3{waterVolume_l * LITER_2_CUBICMETERS}
    , m_prevWaterVolume_m3{}
    , m_nozzleDiameter_mm{nozzleDiameter_mm}
    , m_bottleVolume_m3{bottleVolume_l * LITER_2_CUBICMETERS}
    , m_nozzleArea{pi_v<float> * (m_nozzleDiameter_mm / 1000.0f / 2.0f) * (m_nozzleDiameter_mm / 1000.0f / 2.0f)}
    , m_frontalArea{pi_v<float> * (rocketDiameter_mm / 1000.0f / 2.0f) * (rocketDiameter_mm / 1000.0f / 2.0f)}
    , m_dragCoefficient{dragCoefficient}
    , m_rocketSpeed_ms{0.0f}
    , m_previousRocketSpeed_ms{0.0f}
    , m_rocketAltitude_m{0.0f}
    , m_emptyRocketMass_kg{emptyRocketMass}
    , m_nozzleSpeed_ms{}
    , m_elapsedTime_s{0.0f}
    , m_maxSpeed{0.0f}
    , m_ascendDuration{0.0f}
    , m_maxAltitude{0.0f}
    , m_burnDuration{0.0f}
    , m_flightDuration{0.0f}
    , m_endPressure{pressure * BAR_2_PASCAL}
{
    if (bottleVolume_l < waterVolume_l)
    {
        throw std::runtime_error("water volume > bottle volume");
    }
}

void Rocket::step(float const dt)
{
    if (dt > 0.1)
    {
        throw std::runtime_error("time step must be <= 0.1");
    }

    m_nozzleSpeed_ms = calcNozzleSpeed();
    float const propulsionForce{m_nozzleSpeed_ms * m_nozzleSpeed_ms * WATER_DENSITY * m_nozzleArea};
    float const rocketMass{m_emptyRocketMass_kg + m_waterVolume_m3 * WATER_DENSITY};
    float const aeroDrag{calcAeroDrag(m_rocketSpeed_ms)};

    m_elapsedTime_s          = m_elapsedTime_s + dt;
    m_previousRocketSpeed_ms = m_rocketSpeed_ms;
    m_rocketSpeed_ms         = m_rocketSpeed_ms + (((propulsionForce + aeroDrag) / rocketMass) - GRAVITY) * dt;
    m_rocketAltitude_m       = m_rocketAltitude_m + m_rocketSpeed_ms * dt;
    m_prevWaterVolume_m3     = m_waterVolume_m3;

    updateWaterVolume(dt);
    updatePressure();
    logData();
}

void Rocket::logData()
{
    if (m_rocketSpeed_ms > 0.0f)
    {
        m_ascendDuration = m_elapsedTime_s;
    }

    m_maxAltitude = std::max(m_maxAltitude, m_rocketAltitude_m);
    m_maxSpeed    = std::max(m_maxSpeed, m_rocketSpeed_ms);

    if (m_prevWaterVolume_m3 > 0.0f)
    {
        m_burnDuration = m_elapsedTime_s;
        m_endPressure  = m_pressure_pa;
    }

    if (m_rocketAltitude_m >= 0.0f)
    {
        m_flightDuration = m_elapsedTime_s;
    }
}

void Rocket::printVerbose()
{
    std::cout << "==> time: " << m_elapsedTime_s << "  speed: " << m_rocketSpeed_ms
              << "  altitude: " << m_rocketAltitude_m << "  nozzle speed: " << m_nozzleSpeed_ms
              << "  water volume: " << m_waterVolume_m3 / LITER_2_CUBICMETERS << std::endl;
}

void Rocket::printResult()
{
    std::cout << "==> max speed: " << m_maxSpeed << " m/s" << std::endl;
    std::cout << "==> ascendDuration: " << m_ascendDuration << " s" << std::endl;
    std::cout << "==> max altitude: " << m_maxAltitude << " m" << std::endl;
    std::cout << "==> burn duration: " << m_burnDuration << " s" << std::endl;
    std::cout << "==> flight duration: " << m_flightDuration << " s" << std::endl;
    std::cout << "==> end pressure: " << m_endPressure / BAR_2_PASCAL << " bar" << std::endl;
}

float Rocket::calcNozzleSpeed()
{
    if (m_waterVolume_m3 > 0.0f)
    {
        return std::sqrt(2.0f * m_pressure_pa / WATER_DENSITY);
    }
    return 0.0f;
}

int sign(float const val)
{
    if (val > 0)
        return 1;
    if (val < 0)
        return -1;
    return 0;
}

float Rocket::calcAeroDrag(float const speed)
{
    float const aeroDrag{0.5f * speed * speed * AIR_DENSITY * m_dragCoefficient * m_frontalArea * (-sign(speed))};
    return aeroDrag;
}

void Rocket::updateWaterVolume(float const dt)
{
    if (m_waterVolume_m3 > 0.0f)
    {
        m_waterVolume_m3 = m_waterVolume_m3 - m_nozzleSpeed_ms * m_nozzleArea * dt;
    }

    if (m_waterVolume_m3 <= 0.0f)
    {
        m_waterVolume_m3 = 0.0f;
    }
}

void Rocket::updatePressure()
{
    float const previousAirVolume{m_bottleVolume_m3 - m_prevWaterVolume_m3};
    float const airVolume{m_bottleVolume_m3 - m_waterVolume_m3};
    m_pressure_pa = m_pressure_pa * std::pow((previousAirVolume / airVolume), HEAT_CAPACITY_RATIO_AIR);

    if (m_pressure_pa < 0.0f)
    {
        m_pressure_pa = 0.0f;
    }
}
