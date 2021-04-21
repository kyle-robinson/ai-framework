#include "SteeringBehaviour.h"
#include "Math/Transformations.h"
#include "AIManager.h"

#define ADD_TO_NET_FORCE if ( !AccumulateForce( m_vSteeringForce, netForce ) ) return m_vSteeringForce;

SteeringBehaviour::SteeringBehaviour( Vehicle* vehicle ) : m_pVehicle( vehicle )
{
    // wander position lies on a circle
    float theta = RandFloat() * TwoPi;
    m_vWanderTarget = Vector2D( m_fWanderRadius * cos( theta ), m_fWanderRadius * sin( theta ) );

    m_pPath = new Path();
}

SteeringBehaviour::~SteeringBehaviour()
{
    delete m_pPath;
    m_pPath = nullptr;
}

Vector2D SteeringBehaviour::CalculateSteeringBehaviours()
{
    // reset steering force
    m_vSteeringForce.Zero();
    Vector2D netForce;

    // add to netForce for each steering behaviour that is active
    if ( On( OBSTACLE ) )
    {
        netForce = ObstacleAvoidance( m_pVehicle->Manager()->GetObstacles() ) * 10.0f;
        ADD_TO_NET_FORCE;
    }
    if ( On( ARRIVE ) )
    {
        netForce = Arrive( m_pVehicle->Manager()->GetCrosshair()->GetPosition() );
        ADD_TO_NET_FORCE;
    }
    if ( On( SEEK ) )
    {
        netForce = Seek( m_pVehicle->Manager()->GetCrosshair()->GetPosition() );
        ADD_TO_NET_FORCE;
    }
    if ( On( FLEE ) )
    {
        netForce = Flee( m_bFleeCar ? m_pVehicle->Manager()->GetRedCar()->GetPosition()
            : m_pVehicle->Manager()->GetCrosshair()->GetPosition() );
        ADD_TO_NET_FORCE;
    }
    if ( On( PURSUIT ) )
    {
        assert( m_pTargetAgent && "Pursuit target not assigned!" );
        netForce = Pursuit( m_pTargetAgent );
        ADD_TO_NET_FORCE;
    }
    if ( On( WANDER ) )
    {
        netForce = Wander() * 10.0f;
        ADD_TO_NET_FORCE;
    }
    if ( On( FOLLOW_PATH ) )
    {
        netForce = FollowPath() * 10.0f;
        ADD_TO_NET_FORCE;
    }

    // return total netForce on vehicle
    return m_vSteeringForce;
}

bool SteeringBehaviour::AccumulateForce( Vector2D& netForce, Vector2D newForce )
{
    // calculate how much force has been used, and how much remains to be used
    float magSoFar = netForce.Length();
    float magRemaining = m_pVehicle->GetMaxForce() - magSoFar;
    if ( magRemaining <= 0.0f ) return false;

    // calculate the magnitude of the force to add
    float magToAdd = newForce.Length();

    // if mag of force to add and total don't exceed vehicle's max force, add together
    if ( magToAdd < magRemaining )
        netForce += newForce;
    // else an amount of force that doesn't exceed the vehicle's max force
    else
        netForce += ( Vec2DNormalize( newForce ) * magRemaining );

    return true;
}

#pragma region BEHAVIOURS
Vector2D SteeringBehaviour::Arrive( Vector2D target )
{
    Vector2D toTarget = target - m_pVehicle->GetPosition();
    float dist = toTarget.Length();

    if ( dist > 0.0f )
    {
        // calculate the speed required to reach the target given the desired deceleration
        float speed = dist / 2.0f;

        // ensure speed doesn't exceed max
        speed = min( speed, m_pVehicle->GetMaxSpeed() );

        // continue like with 'seek' but don't normalise
        Vector2D newVel = toTarget * speed / dist;
        return ( newVel - m_pVehicle->GetVelocity() );
    }

    return Vector2D( 0, 0 );
}

Vector2D SteeringBehaviour::Flee( Vector2D target )
{
    // flee only if within range of target
    if ( m_bEnablePanic )
    {
        const float fleeDistSq = powf( 200.0f, 2.0f );
        if ( Vec2DDistanceSq( m_pVehicle->GetPosition(), target ) > fleeDistSq )
            return Vector2D( 0, 0 );
    }

    m_pVehicle->SetMaxSpeed( m_pVehicle->GetMaxSpeed() * 2.0f );
    Vector2D newVel = Vec2DNormalize( m_pVehicle->GetPosition() - target ) * m_pVehicle->GetMaxSpeed();
    return ( newVel - m_pVehicle->GetVelocity() );
}

Vector2D SteeringBehaviour::Seek( Vector2D target )
{
    Vector2D newVel = Vec2DNormalize( target - m_pVehicle->GetPosition() ) * m_pVehicle->GetMaxSpeed();
    return ( newVel - m_pVehicle->GetVelocity() );
}

Vector2D SteeringBehaviour::Pursuit( const Vehicle* agent )
{
    // if agent is ahead and facing target, seek for evader
    Vector2D toAgent = agent->GetPosition() - m_pVehicle->GetPosition();
    float heading = m_pVehicle->GetHeading().Dot( agent->GetHeading() );
    if ( ( toAgent.Dot( m_pVehicle->GetHeading() ) > 0.0f ) && ( heading < -0.95f ) )
        return Seek( agent->GetPosition() );

    // get dist between agent and pursuer / inverse sum of both velocities
    float lookAheadTime = toAgent.Length() / ( m_pVehicle->GetMaxSpeed() + agent->GetSpeed() );

    // seek to the predicted position of evader
    return Seek( agent->GetPosition() + agent->GetVelocity() * lookAheadTime );
}

Vector2D SteeringBehaviour::Wander()
{
    // time independent update rate
    float jitterIndependent = m_fWanderJitter * m_pVehicle->GetDeltaTime();

    // add a small random vector to the target's position
    m_vWanderTarget += Vector2D( RandomClamped() * jitterIndependent, RandomClamped() * jitterIndependent );
    m_vWanderTarget.Normalize();

    // increase vector length to same as radius of wander circle
    m_vWanderTarget *= m_fWanderRadius;

    // move target to a position in front of vehicle
    Vector2D targetPos = m_vWanderTarget + Vector2D( m_fWanderDistance, 0 );

    // convert target to world space
    Vector2D targetWorld = PointToWorldSpace( targetPos, m_pVehicle->GetHeading(), m_pVehicle->GetSide(), m_pVehicle->GetPosition() );
    return targetWorld - m_pVehicle->GetPosition();
}

Vector2D SteeringBehaviour::ObstacleAvoidance( const std::vector<DrawableGameObject*>& obstacles )
{
    // detection box length is proportional to the car's velocity
    m_fDBoxLength = 40.0f + ( m_pVehicle->GetSpeed() / m_pVehicle->GetMaxSpeed() ) * 40.0f;

    // tag obstacles within range for processing
    m_pVehicle->Manager()->TagObstaclesWithinViewRange( m_pVehicle, m_fDBoxLength );

    // closest obstacle
    DrawableGameObject* closestObstacle = nullptr;
    float distToClosest = MaxFloat;
    Vector2D closestObstaclePosition;

    std::vector<DrawableGameObject*>::const_iterator currentObstacle = obstacles.begin();
    while ( currentObstacle != obstacles.end() )
    {
        // if the obstacle is within range
        if ( ( *currentObstacle )->IsTagged() )
        {
            // calculate obstacle's local space position
            Vector2D localPosition = PointToLocalSpace( ( *currentObstacle )->GetPosition(), m_pVehicle->GetHeading(),
                m_pVehicle->GetSide(), m_pVehicle->GetPosition() );

            // if behind the car, ignore it
            if ( localPosition.x >= 0.0f )
            {
                // check for potential intersection
                float radius = ( *currentObstacle )->GetBoundingRadius() + m_pVehicle->GetBoundingRadius();
                if ( fabs( localPosition.y ) < radius )
                {
                    // line intersection test
                    float SqrtPart = sqrt( powf( radius, 2.0f ) - powf( localPosition.y, 2.0f ) );
                    float intersectionPosition = localPosition.x - SqrtPart;
                    if ( intersectionPosition <= 0.0f )
                        intersectionPosition = localPosition.x + SqrtPart;

                    // check to see if this obstacle is the closest
                    if ( intersectionPosition < distToClosest )
                    {
                        distToClosest = intersectionPosition;
                        closestObstacle = *currentObstacle;
                        closestObstaclePosition = localPosition;
                    }
                }
            }
        }
        ++currentObstacle;
    }

    // if there is an intersecting obstacle, get a steering force away from it
    Vector2D netForce;
    if ( closestObstacle )
    {
        // the closer the obstacle, the stronger the steering force
        float multiplier = 1.0f + ( m_fDBoxLength - closestObstaclePosition.x ) / m_fDBoxLength;
        netForce.y = ( closestObstacle->GetBoundingRadius() - closestObstaclePosition.y ) * multiplier;

        // apply braking force proportional to obstacles distance from car
        const float brakingForce = 0.2f;
        netForce.x = ( closestObstacle->GetBoundingRadius() - closestObstaclePosition.x ) * brakingForce;
    }

    return VectorToWorldSpace( netForce, m_pVehicle->GetHeading(), m_pVehicle->GetSide() );
}

Vector2D SteeringBehaviour::FollowPath()
{
    // if the car is close enough to the current target, then move to the next target
    if ( Vec2DDistanceSq( m_pPath->CurrentWaypoint(), m_pVehicle->GetPosition() ) < m_fWaypointSeekDistSq )
        m_pPath->SetNextWaypoint();

    if ( !m_pPath->Finished() )
        return Seek( m_pPath->CurrentWaypoint() );
    else
        return Arrive( m_pPath->CurrentWaypoint() );
}
#pragma endregion