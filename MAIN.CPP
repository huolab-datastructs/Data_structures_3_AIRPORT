#include "../../C/UTILITY.H"
#include "PLANE.H"
#include "QUEUE.H"
#include "EXTQUEUE.H"
#include "RUNWAY.H"
#include "../../B/RANDOM.H"
#include "../../B/RANDOM.CPP"


 
void initialize(int & end_time, int & queue_limit, double & arrival_rate, double & departure_rate) {
// Pre: 
//  The user specifies the number of time units in the simulation,
//  the maximal queue sizes permitted,
//  and the expected arrival and departure rates for the airport
//
// Post:
//  The program prints instructions and initializes the parameters
//  end_time, queue_limit, arr                                                                                  ival_rate, and departure_rate to
//  the specified values.
//  Uses: utility function user_says_yes
//
// Modified by Alex Schrepfer, Feb 2001
 
   cout << "This program simulates an airport with only three runways." << endl
        << "There is one air port designated for takeoffs and another one" << endl
		  << "designated for landings. There is a third runway which takes" << endl
		  << "the overflow from the other two runways. The landing runway has" << endl
		  << "the right of way to the overlow runway first. If that runway is" << endl
		  << "clear, then a plane waiting to take off can use it.." << endl << endl;

   cout << "Up to what number of planes can be waiting to land or take off at any time? ";
   cin  >> queue_limit;

   cout << "How many units of time will the simulation run? ";
   cin  >> end_time;

   bool acceptable;
   do {
 
      cout << "Expected number of arrivals per unit time? ";
      cin  >> arrival_rate;
      cout << "Expected number of departures per unit time? ";
      cin  >> departure_rate;

      if (arrival_rate < 0.0 || departure_rate < 0.0)
         cout << "These rates must be nonnegative." << endl;
      else
         acceptable = true;

      if (acceptable && arrival_rate + departure_rate > 1.0)
         cout << endl << "Safety Warning: This airport will become saturated. " << endl;

   } while (!acceptable);
}
 
void run_idle(int time) {
// Post:
//  The specified time is printed with a message that the runway is idle.

   cout << time << ": Runway is idle." << endl;
}

void move_plane(Runway & current_runway, int current_time) {
// Pre:
//  The users specifies the runway and the current time.
//
// Post:
//  The program directs the current plane take off, land or idle depending
//  on the status of the current runway.
//
// Modified by Alex Schrepfer, Feb 2001

	Plane moving_plane;
	switch (current_runway.activity(current_time, moving_plane)) {
	//  Let at most one Plane onto the Runway at current_time.
		case land:
			moving_plane.land(current_time);
			break;
		case take_off:
			moving_plane.fly(current_time);
			break;
		case idle:
			run_idle(current_time);
	}
}

void main() {     //  Airport simulation program
// Pre: 
//  The user must supply the number of time intervals the simulation is to
//  run, the expected number of planes arriving, the expected number
//  of planes departing per time interval, and the
//  maximum allowed size for runway queues.
//
// Post:
//  The program performs a random simulation of the airport, showing
//  the status of the runway at each time interval, and prints out a
//  summary of airport operation at the conclusion.
// 
// Uses:
//  Classes Runway, Plane, Random and
//  functions run_idle, initialize.
//
// Modified by Alex Schrepfer, Feb 2001
 
   int end_time;            //  time to run simulation
   int queue_limit;         //  size of Runway queues
   int flight_number = 0;
   double arrival_rate, departure_rate;
   initialize(end_time, queue_limit, arrival_rate, departure_rate);
   Random variable;

   Runway landing(queue_limit);
   Runway takeoff(queue_limit);
   Runway overflow(queue_limit);

   for (int current_time = 0; current_time < end_time; current_time++) { //  loop over time intervals

      int number_arrivals = variable.poisson(arrival_rate);  //  current arrival requests
		for (int i = 0; i < number_arrivals; i++) {
         Plane current_plane(flight_number++, current_time, arriving);

			if (landing.can_land(current_plane) != success) {
				// separate so it wont call the overflow airport if the success is true for landing
				if (overflow.can_land(current_plane) != success)
					current_plane.refuse();
			}
		}

      int number_departures= variable.poisson(departure_rate); //  current departure requests
      for (int j = 0; j < number_departures; j++) {
         Plane current_plane(flight_number++, current_time, departing);
         if (takeoff.can_depart(current_plane) != success) {
				// if there are planes waiting to land, dont let this plane take off
				if ((overflow.can_depart(current_plane) != success) || (overflow.arrival_size() > 0))
					current_plane.refuse();
			}
		}
		// moves all the planes currenting awaiting on all 3 runways
		move_plane(landing, current_time);
		move_plane(takeoff, current_time);
		move_plane(overflow, current_time);
      
   }

	// outputs statistics for all three runways
	cout << endl << "Statistics for landing runway:" << endl;
   landing.shut_down(end_time);

	cout << endl << "Statistics for takeoff runway:" << endl;
	takeoff.shut_down(end_time);

	cout << endl << "Statistics for overflow runway:" << endl;
	overflow.shut_down(end_time);

}
