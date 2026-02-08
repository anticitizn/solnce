class SOICalculationSystem : public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<MassiveBody>();
    }

    void Update()
    {
        for (auto e : entities)
        {
            auto& body = coordinator.GetComponent<MassiveBody>(e);

            if (!coordinator.HasComponent<OrbitComponent>(e))
            {
                // No orbit component means this is the system root, so it has infinite SOI
                body.soi = std::numeric_limits<double>::infinity();
                continue;
            }

            const auto& orbit = coordinator.GetComponent<OrbitComponent>(e);

            // sanity check
            // if (orbit.parentBodyId == 0 || !coordinator.HasComponent<MassiveBody>(orbit.parentBodyId))
            // {
            //     body.soi = std::numeric_limits<double>::infinity();
            //     continue;
            // }

            const auto& parent = coordinator.GetComponent<MassiveBody>(orbit.parentBodyId);

            if (orbit.a <= 0.0 || body.mass <= 0.0 || parent.mass <= 0.0)
            {
                body.soi = std::numeric_limits<double>::infinity();
                continue;
            }

            body.soi = orbit.a * std::pow(body.mass / parent.mass, 0.4);
        }
    }
};
