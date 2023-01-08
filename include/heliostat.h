#ifndef HELIOSTAT_H
#define HELIOSTAT_H

#include <vector>

#include "environment.h"
#include "receiver.h"
#include "vec3d.h"

namespace hypl
{
    class Heliostat
    {
 
    public:

        enum IdealEfficiencyType { CosineOnly, CosineAndTransmittance, AllFactors };

        struct TrackingInfo {
            double ideal_efficiency;            
            int aiming_at_receiver_id;
        };

        Heliostat(Environment& environment, std::vector<Receiver>& receivers, vec3d center);

        //Accessors
        Environment const& environment() const { return m_environment; }
        std::vector<Receiver> const& receivers() const { return m_receivers; }
        vec3d const& center() const { return m_center; }
        std::vector<double> const& transmittance() const { return m_transmittance; }
        std::vector<double> const& slant_range() const { return m_slant_range; }
        std::vector<vec3d> const& reflected_unit_vector() const { return m_reflected_unit_vector; }

        //Mutators
        void set_center(vec3d center) { m_center = center; update(); }
        void update();

        //Public functions
        TrackingInfo Track(vec3d& sun_vector, double sun_subtended_angle, IdealEfficiencyType ideal_efficiency_type) const;
        double Spillage(int receiver_id, double sun_subtended_angle) const;
        double ReceiverShadowing(int receiver_id, vec3d& sun_vector) const;

        double m_annual_ideal_efficiency;

    private:
        Environment& m_environment;
        std::vector<Receiver>& m_receivers;
        vec3d m_center;

        std::vector<double> m_transmittance;
        std::vector<double> m_slant_range;
        std::vector<vec3d> m_reflected_unit_vector;
    };
}
#endif // HELIOSTAT_H