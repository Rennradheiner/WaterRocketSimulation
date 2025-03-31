#include <cstdlib>
#include <future>
#include <iostream>
#include <Rocket.hpp>
#include <thread>
#include <tuple>
#include <vector>

void printLogo()
{
    std::cout << "                    *     .--.		" << std::endl;
    std::cout << "                         / /  `		" << std::endl;
    std::cout << "        +               | |			" << std::endl;
    std::cout << "               '         \\ \\__,		" << std::endl;
    std::cout << "           *          +   '--'  *		" << std::endl;
    std::cout << "               +   /\\				" << std::endl;
    std::cout << "  +              .'  '.   *			" << std::endl;
    std::cout << "         *      /======\\      +		" << std::endl;
    std::cout << "               ;:.  _   ;				" << std::endl;
    std::cout << "               |:. (_)  |				" << std::endl;
    std::cout << "               |:.  _   |				" << std::endl;
    std::cout << "     +         |:. (_)  |          *	" << std::endl;
    std::cout << "               ;:.      ;				" << std::endl;
    std::cout << "             .' \\:.    / `.			" << std::endl;
    std::cout << "            / .-'':._.'`-. \\			" << std::endl;
    std::cout << "            |/    /||\\    \\|		" << std::endl;
    std::cout << "          _..--\"\"\"````\"\"\"--.._	" << std::endl;
    std::cout << "    _.-'``                    ``'-._	" << std::endl;
    std::cout << "  -'                                '-" << std::endl;
    std::cout << std::endl;
}

std::tuple<float, float> rocketSimulate(
    float const pressure, float const waterVolume_l, float const nozzleDiameter_mm, float const bottleVolume_l,
    float const emptyRocketMass, float const rocketDiameter_mm, float const dragCoefficient, float const dt,
    bool const printResult = false, bool const printVerbose = false)
{
    float maxRocketAltitude{0.0f};

    Rocket rocket(
        pressure, waterVolume_l, nozzleDiameter_mm, bottleVolume_l, emptyRocketMass, rocketDiameter_mm,
        dragCoefficient);

    while (rocket.getAltitude() >= 0.0f)
    {
        rocket.step(dt);
        if (printVerbose)
        {
            rocket.printVerbose();
        }
    }

    if (printResult)
    {
        rocket.printResult();
    }

    maxRocketAltitude = rocket.getMaxAltitude();

    return std::make_tuple(maxRocketAltitude, waterVolume_l);
}

// Thread function that uses promise to set the result
void threadFunction(
    std::promise<std::tuple<float, float>> resultPromise, float const pressure, float const waterVolume_l,
    float const nozzleDiameter_mm, float const bottleVolume_l, float const emptyRocketMass,
    float const rocketDiameter_mm, float const dragCoefficient, float const dt, bool const printResult,
    bool const printVerbose)
{
    try
    {
        auto result = rocketSimulate(
            pressure, waterVolume_l, nozzleDiameter_mm, bottleVolume_l, emptyRocketMass, rocketDiameter_mm,
            dragCoefficient, dt, printResult, printVerbose);
        resultPromise.set_value(result); // Set the result in the promise
    }
    catch (...)
    {
        resultPromise.set_exception(std::current_exception()); // Set the exception in the promise
    }
}

int main()
{
    system("cls");
    printLogo();

    float const START_PRESSURE_BAR{3.0f};
    float const START_WATER_VOLUME_L{0.8f};
    float const NOZZLE_DIAMETER_MM{9.0f};
    float const BOTTLE_VOLUME_L{1.0f};
    float const EMPTY_ROCKET_MASS_KG{0.3};
    float const TIME_STEP{0.001f};
    float const ROCKET_DIAMETER_MM{100.0f};
    float const DRAG_COEFFICIENT{0.1f};

    int const ITERATIONS{100};

    try
    {
        auto [defaultValuesMaxAltitude, defaultWaterVolume] = rocketSimulate(
            START_PRESSURE_BAR, START_WATER_VOLUME_L, NOZZLE_DIAMETER_MM, BOTTLE_VOLUME_L, EMPTY_ROCKET_MASS_KG,
            ROCKET_DIAMETER_MM, DRAG_COEFFICIENT, TIME_STEP);

        std::cout << "*********************************************************" << std::endl;
        std::cout << "* Results with default values                           *" << std::endl;
        std::cout << "*********************************************************" << std::endl;
        std::cout << "==> max altitude: " << defaultValuesMaxAltitude << " m" << std::endl;
        std::cout << "==> water volume: " << defaultWaterVolume << " l" << std::endl;
        std::cout << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cout << "<<<<<<<<<<<<<<<<<< ";
        std::cerr << "Exception: " << e.what();
        std::cout << " >>>>>>>>>>>>>>>>>>" << std::endl;
        std::cout << std::endl;
    }

    float dragIndicatorMaxAltitude{0.0f};
    float dragIndicatorStartWaterVolume{0.0f};
    bool optimizedResultsAvailable{false};

    std::vector<std::unique_ptr<std::thread>> threads;
    std::vector<std::future<std::tuple<float, float>>> futures;

    for (auto i = 1; i < ITERATIONS + 2; i++) // +2 to check that the exception is thrown
    {
        float const startWaterVolume{BOTTLE_VOLUME_L / ITERATIONS * i};

        std::promise<std::tuple<float, float>> resultPromise;
        futures.push_back(resultPromise.get_future());
        threads.emplace_back(std::make_unique<std::thread>(
            threadFunction, std::move(resultPromise), START_PRESSURE_BAR, startWaterVolume, NOZZLE_DIAMETER_MM,
            BOTTLE_VOLUME_L, EMPTY_ROCKET_MASS_KG, ROCKET_DIAMETER_MM, DRAG_COEFFICIENT, TIME_STEP, false, false));
    }

    for (auto& thread : threads)
    {
        thread->join();
    }

    for (auto& future : futures)
    {
        try
        {
            auto [maxAltitude, startWaterVolume] = future.get();
            if (maxAltitude > dragIndicatorMaxAltitude)
            {
                dragIndicatorMaxAltitude      = maxAltitude;
                dragIndicatorStartWaterVolume = startWaterVolume;
                optimizedResultsAvailable     = true;
            }
        }
        catch (std::exception const& e)
        {
            std::cout << "<<<<<<<<<<<<<<<<<< ";
            std::cerr << "Exception: " << e.what();
            std::cout << " >>>>>>>>>>>>>>>>>>" << std::endl;
            std::cout << std::endl;
        }
    }

    if (optimizedResultsAvailable)
    {
        std::cout << "*********************************************************" << std::endl;
        std::cout << "* Optimized results                                     *" << std::endl;
        std::cout << "*********************************************************" << std::endl;
        std::cout << "==> max altitude: " << dragIndicatorMaxAltitude << " m" << std::endl;
        std::cout << "==> water volume: " << dragIndicatorStartWaterVolume << " l" << std::endl;
        std::cout << std::endl;
        std::cout << "*********************************************************" << std::endl;
        std::cout << "* Optimized results details                             *" << std::endl;
        std::cout << "*********************************************************" << std::endl;

        try
        {
            rocketSimulate(
                START_PRESSURE_BAR, dragIndicatorStartWaterVolume, NOZZLE_DIAMETER_MM, BOTTLE_VOLUME_L,
                EMPTY_ROCKET_MASS_KG, ROCKET_DIAMETER_MM, DRAG_COEFFICIENT, TIME_STEP, true);
            std::cout << std::endl;
        }
        catch (std::exception const& e)
        {
            std::cout << "<<<<<<<<<<<<<<<<<< ";
            std::cerr << "Exception: " << e.what();
            std::cout << " >>>>>>>>>>>>>>>>>>" << std::endl;
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "*********************************************************" << std::endl;
        std::cout << "* No optimized results available                        *" << std::endl;
        std::cout << "*********************************************************" << std::endl;
    }

    return 0;
}
