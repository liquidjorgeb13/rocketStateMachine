#include <vector>
#include <iostream>
#include <math.h>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>
// for func_state and func_state_machine
#include <boost/msm/front/euml/state_grammar.hpp>


using namespace std;
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;
// for And_ operator
using namespace msm::front::euml;

// Some data to learn how to pass data 
// to/from between "the outside" and the FSM
struct Mission
{
  Mission(std::string name, std::vector<double> coord, std::string s)
    : destinationName(name), destinationEcef(coord),
      song(s)
  {}

  std::string destinationName;
  std::vector<double> destinationEcef;
  std::string song;
};

// ******************************* EVENTS **************************************

// Rocket gets mission data on ignition
struct ignitionStarted 
{
  ignitionStarted(Mission m)
    : mission(m)
  {
    /*go to TowerDetached state*/
  }
  
  Mission mission;
}; 

struct liftoff{ /*go to MidAir state*/ };  

// *****************************************************************************
// ******************************* STATES **************************************

// -------- Empty State --------------------
struct Empty_tag {};

typedef msm::front::euml::func_state<Empty_tag> EmptySt;

// -------- TowerDetached State --------------------
// As soon as there is ignition, detach from tower
struct TowerDetached_Entry
{
  template <class Event, class FSM, class STATE>
  void operator()(Event const&, FSM&, STATE&)
  {
    std::cout << "Tower detaching..." << std::endl;
  }
};

struct TowerDetached_Exit
{
  template <class Event, class FSM, class STATE>
  void operator()(Event const&, FSM&, STATE&)
  {
    std::cout << "Tower detached..." << std::endl;
  }
};

struct TowerDetached_tag {};

typedef msm::front::euml::func_state<TowerDetached_tag, TowerDetached_Entry, TowerDetached_Exit> TowerDetachedSt;


// -------- MidAir State --------------------
// Rocket not touching the ground anymore
struct MidAir_Entry
{
  template <class Event, class FSM, class STATE>
  void operator()(Event const&, FSM&, STATE&)
  {
    std::cout << "Entering MidAir State - Leaving the floor..." << std::endl;
  }
};

struct MidAir_Exit
{
  template <class Event, class FSM, class STATE>
  void operator()(Event const&, FSM&, STATE&)
  {
    std::cout << "Tower detached..." << std::endl;
  }
};

struct MidAir_tag {};
typedef msm::front::euml::func_state<MidAir_tag, MidAir_Entry, MidAir_Exit> MidAirSt;

// ----------- DEFINE INITIAL STATE ----------
typedef TowerDetachedSt initial_state;

// *****************************************************************************
// ****************** TRANSITION ACTIONS FUNCTORS ******************************

struct injectFuel
{
  template <class EVT,class FSM,class SourceState,class TargetState>
  void operator()(EVT const& evt, FSM& fsm,SourceState& ,TargetState& )
  {
    cout << "injecting fuel..." << std::endl;
  }
};

struct initAutoPilot
{
  template <class EVT,class FSM,class SourceState,class TargetState>
  void operator()(EVT const&, FSM&,SourceState& ,TargetState& )
  {
    cout << "Initializing autopilot..." << std::endl;
  }
};

struct blastMusic
{
  template <class EVT,class FSM,class SourceState,class TargetState>
  void operator()(EVT const& ev, FSM&,SourceState& srcEv,TargetState& targetSt)
  {
    cout << "Setting the mood to the sound of " << "FILL THIIIIIS" << std::endl;
    //cout << "Setting the mood to the sound of " << ev.song << std::endl;
  }
};

// *****************************************************************************
// ******************************* GUARD FUNCTORS ******************************

struct goodFuelPump
{
  template <class EVT,class FSM,class SourceState,class TargetState>
  bool operator()(EVT const& evt,FSM& fsm,SourceState& src,TargetState& tgt)
  {
    //return fsm.goodFuelPump;
    return true;
  }
};

struct destinationInRange 
{
  template <class EVT,class FSM,class SourceState,class TargetState>
  bool operator()(EVT const& evt,FSM& fsm,SourceState& src,TargetState& tgt)
  {
    //float x = fsm.mission.destinationEcef.at(0);
    //float y = fsm.mission.destinationEcef.at(1);
    //float z = fsm.mission.destinationEcef.at(2);

    
    //float range = sqrt( x*x + y*y + z*z);
    //return range < fsm.travelDistanceLimit;
    return true;
  }
};

struct goodAccelerometer
{
  template <class EVT,class FSM,class SourceState,class TargetState>
  bool operator()(EVT const& evt,FSM& fsm,SourceState& src,TargetState& tgt)
  {
    //return fsm.goodIMU;
    return true;
  }
};


// *****************************************************************************
// ******************************* FSM DEFINITION ******************************

struct rocket_transition_table : mpl::vector<
  //    Start            Event               Next               Action                     Guard
  //  +----------------+-------------------+------------------+---------------------------+----------------------+
  Row < EmptySt         , ignitionStarted  , TowerDetachedSt  , injectFuel                , goodFuelPump         >,
  //  +----------------+-------------------+------------------+---------------------------+----------------------+
  Row < TowerDetachedSt , liftoff          , MidAirSt         , ActionSequence_
                                                                 <mpl::vector<
                                                                 initAutoPilot,
                                                                 blastMusic> >            , And_<
                                                                                            destinationInRange,
                                                                                            goodAccelerometer>   >
  //  +----------------+-------------------+------------------+---------------------------+----------------------+


> {};

struct rocket_tag {};
typedef msm::front::euml::func_state_machine<rocket_tag,
                                             rocket_transition_table,
                                             EmptySt> rocket_;

typedef msm::back::state_machine<rocket_> rocket;



int main()
{
  Mission mission("Moon", {2.0, 2.0, 2.0}, "Motley Crue - Kickstart my Heart");

  rocket dogeRocket;
  
  dogeRocket.start();
  dogeRocket.process_event(ignitionStarted(mission));

  dogeRocket.process_event(liftoff());
  
  return 0;
}






