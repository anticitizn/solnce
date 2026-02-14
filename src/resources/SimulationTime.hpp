
#pragma once

struct SimulationTime
{
    uint64_t real_time; // ms
    uint64_t real_dt;   // ms
    uint64_t sim_time;  // ms
    uint64_t sim_dt;    // ms

    uint64_t sim_time_factor; // sim_dt = real_dt / sim_time_factor
};
