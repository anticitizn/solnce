#pragma once

#include <chrono>
#include <src/ui/Window.hpp>
#include <src/ecs/ECS.hpp>
#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/MassiveBody.hpp>

extern Coordinator coordinator;

static inline void SimMsToCivil(uint64_t sim_ms,
                                int& out_year, int& out_month, int& out_day,
                                int& out_hour, int& out_minute, double& out_second)
{
    using namespace std::chrono;

    // Epoch starts at 2026-01-01 00:00:00
    const sys_time<milliseconds> tp = sys_days{year{2026}/month{1}/day{1}} + milliseconds{sim_ms};

    const auto dp = floor<days>(tp);
    const year_month_day ymd{dp};
    const hh_mm_ss<milliseconds> hms{tp - dp};

    out_year   = int(ymd.year());
    out_month  = int(unsigned(ymd.month()));
    out_day    = int(unsigned(ymd.day()));
    out_hour   = int(hms.hours().count());
    out_minute = int(hms.minutes().count());

    // seconds with milliseconds as fraction
    out_second = double(hms.seconds().count()) + (double(hms.subseconds().count()) / 1000.0);
}


class TimeWindow : public Window
{
public:
    virtual void Draw() override
    {
        if (!display)
            return;

        ImGui::Begin("Time Control");

        auto& simTime = coordinator.GetResource<SimulationTime>();
        
        int y, mo, d, h, mi;
        double s;
        SimMsToCivil(simTime.sim_time, y, mo, d, h, mi, s);

        ImGui::Text("%02d.%02d.%04d %02d:%02d:%06.3f", d, mo, y, h, mi, s);
        if (ImGui::Button("||"))
        {
            simTime.sim_time_factor = 0;
        }

        ImGui::SameLine();
        if (ImGui::Button("1x"))
        {
            simTime.sim_time_factor = 1;
        }

        ImGui::SameLine();
        if (ImGui::Button("100x"))
        {
            simTime.sim_time_factor = 100;
        }

        ImGui::SameLine();
        if (ImGui::Button("100000x"))
        {
            simTime.sim_time_factor = 100000;
        }

        ImGui::End();
    }
};
