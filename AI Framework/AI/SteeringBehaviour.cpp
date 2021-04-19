#include "SteeringBehaviour.h"
#include "Math/Transformations.h"
#include "AIManager.h"

SteeringBehaviour::SteeringBehaviour( Vehicle* vehicle ) : m_pVehicle( vehicle )
{
    double theta = RandFloat() * TwoPi;
    m_vWanderTarget = Vector2D( m_fWanderRadius * cos( theta ), m_fWanderRadius * sin( theta ) );

    m_pPath = new Path();
    m_pPath->LoopOn();
}

SteeringBehaviour::~SteeringBehaviour()
{
    delete m_pPath;
    m_pPath = nullptr;
}

Vector2D SteeringBehaviour::Calculate()
{
    // reset the steering force
    m_vSteeringForce.Zero();
    Vector2D force;

    if ( On( OBSTACLE ) )
    {
        force = ObstacleAvoidance( m_pVehicle->World()->GetObstacles() ) * m_fWeightObstacleAvoidance;
        if ( !AccumulateForce( m_vSteeringForce, force ) ) return m_vSteeringForce;
    }

    if ( On( FLEE ) )
    {
        force = Flee( m_pVehicle->World()->GetCrosshair() ) * m_fWeightFlee;
        if ( !AccumulateForce( m_vSteeringForce, force ) )
            return m_vSteeringForce;
    }

    if ( On( SEEK ) )
    {
        force = Seek( m_pVehicle->World()->GetCrosshair() ) * m_fWeightSeek;
        if ( !AccumulateForce( m_vSteeringForce, force ) )
            return m_vSteeringForce;
    }

    if ( On( ARRIVE ) )
    {
        force = Arrive( m_pVehicle->World()->GetCrosshair(), m_eDeceleration ) * m_fWeightArrive;
        if ( !AccumulateForce( m_vSteeringForce, force ) )
            return m_vSteeringForce;
    }

    if ( On( PURSUIT ) )
    {
        assert( m_pTargetAgent && "Pursuit target not assigned!" );
        force = Pursuit( m_pTargetAgent ) * m_fWeightPursuit;
        if ( !AccumulateForce( m_vSteeringForce, force ) )
            return m_vSteeringForce;
    }

    if ( On( WANDER ) )
    {
        force = Wander() * m_fWeightWander;
        if ( !AccumulateForce( m_vSteeringForce, force ) )
            return m_vSteeringForce;
    }

    if ( On( FOLLOW_PATH ) )
    {
        force = FollowPath() * m_fWeightFollowPath;
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
Vector2D SteeringBehaviour::Arrive( Vector2D TargetPos, Deceleration deceleration )
{
    Vector2D ToTarget = TargetPos - m_pVehicle->GetPosition();

    //calculate the distance to the target
    double dist = ToTarget.Length();

    if ( dist > 0 )
    {
        //because Deceleration is enumerated as an int, this value is required
        //to provide fine tweaking of the deceleration..
        const double DecelerationTweaker = 1.0;

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

Vector2D SteeringBehaviour::Flee( Vector2D TargetPos )
{
    Vector2D DesiredVelocity = Vec2DNormalize( m_pVehicle->GetPosition() - TargetPos )
        * m_pVehicle->GetMaxSpeed();

    return ( DesiredVelocity - m_pVehicle->GetVelocity() );
}

Vector2D SteeringBehaviour::Seek( Vector2D TargetPos )
{
    Vector2D DesiredVelocity = Vec2DNormalize( TargetPos - m_pVehicle->GetPosition() )
        * m_pVehicle->GetMaxSpeed();

    return ( DesiredVelocity - m_pVehicle->GetVelocity() );
}

Vector2D SteeringBehaviour::Pursuit( const Vehicle* evader )
{
    //if the evader is ahead and facing the agent then we can just seek
    //for the evader's current position.
    Vector2D ToEvader = evader->GetPosition() - m_pVehicle->GetPosition();

    double RelativeHeading = m_pVehicle->GetHeading().Dot( evader->GetHeading() );

    if ( ( ToEvader.Dot( m_pVehicle->GetHeading() ) > 0 ) &&
        ( RelativeHeading < -0.95 ) )  //acos(0.95)=18 degs
    {
        return Seek( evader->GetPosition() );
    }

    //Not considered ahead so we predict where the evader will be.

    //the lookahead time is propotional to the distance between the evader
    //and the pursuer; and is inversely proportional to the sum of the
    //agent's velocities
    double LookAheadTime = ToEvader.Length() /
        ( m_pVehicle->GetMaxSpeed() + evader->GetSpeed() );

    //now seek to the predicted future position of the evader
    return Seek( evader->GetPosition() + evader->GetVelocity() * LookAheadTime );
}

Vector2D SteeringBehaviour::Wander()
{
    //this behavior is dependent on the update rate, so this line must
    //be included when using time independent framerate.
    double JitterThisTimeSlice = m_fWanderJitter * m_pVehicle->GetDeltaTime();

    //first, add a small random vector to the target's position
    m_vWanderTarget += Vector2D( RandomClamped() * JitterThisTimeSlice,
        RandomClamped() * JitterThisTimeSlice );

    //reproject this new vector back on to a unit circle
    m_vWanderTarget.Normalize();

    //increase the length of the vector to the same as the radius
    //of the wander circle
    m_vWanderTarget *= m_fWanderRadius;

    //move the target into a position WanderDist in front of the agent
    Vector2D target = m_vWanderTarget + Vector2D( m_fWanderDistance, 0 );

    //project the target into world space
    Vector2D Target = PointToWorldSpace( target,
        m_pVehicle->GetHeading(),
        m_pVehicle->GetSide(),
        m_pVehicle->GetPosition() );

    //and steer towards it
    return Target - m_pVehicle->GetPosition();
}

Vector2D SteeringBehaviour::ObstacleAvoidance( const std::vector<DrawableGameObject*>& obstacles )
{
    //the detection box length is proportional to the agent's velocity
    m_fDBoxLength = 40.0f + ( m_pVehicle->GetSpeed() / m_pVehicle->GetMaxSpeed() ) * 40.0f;

    //tag all obstacles within range of the box for processing
    m_pVehicle->World()->TagObstaclesWithinViewRange( m_pVehicle, m_fDBoxLength );

    //this will keep track of the closest intersecting obstacle (CIB)
    DrawableGameObject* ClosestIntersectingObstacle = NULL;

    //this will be used to track the distance to the CIB
    double DistToClosestIP = MaxDouble;

    //this will record the transformed local coordinates of the CIB
    Vector2D LocalPosOfClosestObstacle;

    std::vector<DrawableGameObject*>::const_iterator curOb = obstacles.begin();

    while ( curOb != obstacles.end() )
    {
        //if the obstacle has been tagged within range proceed
        if ( ( *curOb )->IsTagged() )
        {
            //calculate this obstacle's position in local space
            Vector2D LocalPos = PointToLocalSpace( ( *curOb )->GetPosition(),
                m_pVehicle->GetHeading(),
                m_pVehicle->GetSide(),
                m_pVehicle->GetPosition() );

            //if the local position has a negative x value then it must lay
            //behind the agent. (in which case it can be ignored)
            if ( LocalPos.x >= 0 )
            {
                //if the distance from the x axis to the object's position is less
                //than its radius + half the width of the detection box then there
                //is a potential intersection.
                double ExpandedRadius = ( *curOb )->GetBoundingRadius() + m_pVehicle->GetBoundingRadius();

                if ( fabs( LocalPos.y ) < ExpandedRadius )
                {
                    //now to do a line/circle intersection test. The center of the 
                    //circle is represented by (cX, cY). The intersection points are 
                    //given by the formula x = cX +/-sqrt(r^2-cY^2) for y=0. 
                    //We only need to look at the smallest positive value of x because
                    //that will be the closest point of intersection.
                    double cX = LocalPos.x;
                    double cY = LocalPos.y;

                    //we only need to calculate the sqrt part of the above equation once
                    double SqrtPart = sqrt( ExpandedRadius * ExpandedRadius - cY * cY );

                    double ip = cX - SqrtPart;

                    if ( ip <= 0.0 )
                    {
                        ip = cX + SqrtPart;
                    }

                    //test to see if this is the closest so far. If it is keep a
                    //record of the obstacle and its local coordinates
                    if ( ip < DistToClosestIP )
                    {
                        DistToClosestIP = ip;

                        ClosestIntersectingObstacle = *curOb;

                        LocalPosOfClosestObstacle = LocalPos;
                    }
                }
            }
        }

        ++curOb;
    }

    //if we have found an intersecting obstacle, calculate a steering 
    //force away from it
    Vector2D SteeringForce;

    if ( ClosestIntersectingObstacle )
    {
        //the closer the agent is to an object, the stronger the 
        //steering force should be
        float multiplier = 1.0f + ( m_fDBoxLength - LocalPosOfClosestObstacle.x ) / m_fDBoxLength;

        //calculate the lateral force
        SteeringForce.y = ( ClosestIntersectingObstacle->GetBoundingRadius() -
            LocalPosOfClosestObstacle.y ) * multiplier;

        //apply a braking force proportional to the obstacles distance from
        //the vehicle. 
        const float BrakingWeight = 0.2f;
        SteeringForce.x = ( ClosestIntersectingObstacle->GetBoundingRadius() -
            LocalPosOfClosestObstacle.x ) * BrakingWeight;
    }

    //finally, convert the steering vector from local to world space
    return VectorToWorldSpace( SteeringForce, m_pVehicle->GetHeading(), m_pVehicle->GetSide() );
}

Vector2D SteeringBehaviour::FollowPath()
{
    //move to next target if close enough to current target (working in distance squared space)
    if ( Vec2DDistanceSq( m_pPath->CurrentWaypoint(), m_pVehicle->GetPosition() ) < m_fWaypointSeekDistSq )
        m_pPath->SetNextWaypoint();

    if ( !m_pPath->Finished() )
        return Seek( m_pPath->CurrentWaypoint() );
    else
        return Arrive( m_pPath->CurrentWaypoint(), NORMAL );
}