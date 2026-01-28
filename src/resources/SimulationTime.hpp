
#pragma once

struct SimulationTime
{
    double real_time;
    double real_dt;
    double sim_time;
    double sim_dt;

    double sim_time_factor; // sim_dt = real_dt / sim_time_factor
};
