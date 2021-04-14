#include "SteeringBehaviour.h"
//#include "Vehicle.h"
#include "AIManager.h"

SteeringBehaviour::SteeringBehaviour( Vehicle* vehicle ) :
	m_pVehicle( vehicle ),
	m_iFlags( 0 ),
	m_dWeightArrive( 1.0 ),
	m_deceleration( NORMAL )
{ }

Vector2D SteeringBehaviour::Calculate()
{
    //reset the steering force
    m_vSteeringForce.Zero();

    if ( On( ARRIVE ) )
    {
        Vector2D force = Arrive( m_pVehicle->World()->GetCrosshair(), m_deceleration ) * m_dWeightArrive;
        if ( !AccumulateForce( m_vSteeringForce, force ) )
            return m_vSteeringForce;
    }

    return m_vSteeringForce;
}

double SteeringBehaviour::ForwardComponent()
{
    return m_pVehicle->GetHeading().Dot( m_vSteeringForce );
}

double SteeringBehaviour::SideComponent()
{
    return m_pVehicle->GetSide().Dot( m_vSteeringForce );
}

bool SteeringBehaviour::AccumulateForce( Vector2D& RunningTot, Vector2D ForceToAdd )
{
    //calculate how much steering force the vehicle has used so far
    double MagnitudeSoFar = RunningTot.Length();

    //calculate how much steering force remains to be used by this vehicle
    double MagnitudeRemaining = m_pVehicle->GetMaxForce() - MagnitudeSoFar;

    //return false if there is no more force left to use
    if ( MagnitudeRemaining <= 0.0 ) return false;

    //calculate the magnitude of the force we want to add
    double MagnitudeToAdd = ForceToAdd.Length();

    //if the magnitude of the sum of ForceToAdd and the running total
    //does not exceed the maximum force available to this vehicle, just
    //add together. Otherwise add as much of the ForceToAdd vector is
    //possible without going over the max.
    if ( MagnitudeToAdd < MagnitudeRemaining )
    {
        RunningTot += ForceToAdd;
    }
    else
    {
        //add it to the steering force
        RunningTot += ( Vec2DNormalize( ForceToAdd ) * MagnitudeRemaining );
    }

    return true;
}

//--------------------------- Arrive -------------------------------------
//
//  This behavior is similar to seek but it attempts to arrive at the
//  target with a zero velocity
//------------------------------------------------------------------------
Vector2D SteeringBehaviour::Arrive( Vector2D targetPos, Deceleration deceleration )
{
    Vector2D ToTarget = targetPos - m_pVehicle->GetPosition();

    //calculate the distance to the target
    double dist = ToTarget.Length();

    if ( dist > 0 )
    {
        //because Deceleration is enumerated as an int, this value is required
        //to provide fine tweaking of the deceleration..
        const double DecelerationTweaker = 0.3;

        //calculate the speed required to reach the target given the desired
        //deceleration
        double speed = dist / ( ( double )deceleration * DecelerationTweaker );

        //make sure the velocity does not exceed the max
        speed = min( speed, m_pVehicle->GetMaxSpeed() );

        //from here proceed just like Seek except we don't need to normalize 
        //the ToTarget vector because we have already gone to the trouble
        //of calculating its length: dist. 
        Vector2D DesiredVelocity = ToTarget * speed / dist;

        return ( DesiredVelocity - m_pVehicle->GetVelocity() );
    }

    return Vector2D( 0, 0 );
}