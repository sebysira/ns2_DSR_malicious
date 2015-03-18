/*#################################################################################
 *########################### http://www.ns2blogger.blogspot.in/ ##################
 *#################################################################################
*/

#include <stdlib.h>
#include "random.h"
#include "trafgen.h"
#include "ranvar.h"

/* 
 * The VBR_Traffic class represents a variable bit rate traffic source.
 * Parameterized by rate, deviation of rate in percent, time periods of no
 * burst, time period of burst, number of rate changes during burst,
 * deviation of time periods in percent and constant rate variation or not.
 */
class VBR_Traffic : public TrafficGenerator {
 public:
	VBR_Traffic();
	virtual double next_interval(int&);
	//HACK so that udp agent knows interpacket arrival time within a burst
	inline double interval() { return (interval_); }
 protected:
	virtual void start();
	void init();
	double rate_;       /* Send rate during on time (bps). */
	double rate_dev_;   /* Size of random variation on rate. */
	double rate_time_;  /* Mean time of ordinary rate period. */
	double burst_time_; /* Mean time of burst rate period. */
	double n_o_changes_;/* Number of changes during burst time. */
	double time_dev_;   /* Deviation from time periods. */
	int constant_;      /* Perform constant bit rate variation?
			     * NOTICE: burst_time_ controls time between
			     * rate variation. */
	double maxrate_;    /* Maximum rate of burst / constant variation. */
	int maxpkts_;
	int seqno_;
	
private:
	double next_change; /* Time for next rate change. */
	double next_burst;  /* Time for next burst. */
	double next_rate;   /* Time for next ordinare rate. */
	double curr_rate;
	double interval_;   /* Packet inter-arrival time during burst (sec) */
};


static class VBRTrafficClass : public TclClass {
 public:
	VBRTrafficClass() : TclClass("Application/Traffic/VBR") {}
	TclObject* create(int, const char*const*) {
		return (new VBR_Traffic());
	}
} class_vbr_traffic;

VBR_Traffic::VBR_Traffic() : seqno_(0) {
	/* See comments in class definition above. */
	bind_bw("rate_", &rate_);               //default 448Kb
	bind("rate_dev_", &rate_dev_);          //default 0.25
	bind_time("rate_time_", &rate_time_);   //default 2.0
	bind_time("burst_time_", &burst_time_); //default 1.0
	bind("n_o_changes_", &n_o_changes_);    //default 10
	bind("time_dev_", &time_dev_);          //default 0.5
	bind_bool("constant_", &constant_);     //default false
	bind_bw("maxrate_", &maxrate_);         //default 648Kb
	bind("packetSize_", &size_);            //default 210
	bind("maxpkts_", &maxpkts_);            //default 268435456
}

void VBR_Traffic::init() {
        double now = Scheduler::instance().clock();
	//Start with burst.
	next_change = now;
	next_burst = now;
	next_rate = now + burst_time_ * (1 - Random::
					uniform(-time_dev_, time_dev_));
	curr_rate = rate_;

        // compute inter-packet interval 
	interval_ = (double)(size_ << 3)/(double)rate_;
	if (agent_) {
		//Use same packet type as for CBR.
		agent_->set_pkttype(PT_CBR);
	}
}

void VBR_Traffic::start()
{
        init();
        running_ = 1;
        timeout();
}

/*
 * Returns the time until the next packet is created (and also sets the size
 * in bytes of the next packet if it has changed in the tcl script).
 */
double VBR_Traffic::next_interval(int& size) {
        double now = Scheduler::instance().clock();

	if (next_change <= now) {
		// Randomize rate and deviation from desired time.
		double rand_time = Random::uniform(-time_dev_, time_dev_);
		double rand = Random::uniform(-(rate_dev_), rate_dev_);
		curr_rate = curr_rate + (maxrate_ - rate_) * rand;
		
		if (constant_) {
			// Set time for next rate change.
			next_change = now + burst_time_ * (1 - rand_time);
		} else if (next_burst <= next_rate) {
			// Set time for next rate change within burst.
			next_burst = now + burst_time_ * (1 - rand_time) /
				n_o_changes_;
		} else {
			/* End of burst period. Set times for next burst
			 * period. */
			next_change = now + rate_time_ * (1 - rand_time);
			next_burst = next_change;
			rand_time = Random::uniform(-time_dev_, time_dev_);
			next_rate = next_change + burst_time_ *
				(1 - rand_time);
			curr_rate = rate_;
		}
		
		// Limit the result.
		if (curr_rate > maxrate_)
			curr_rate = maxrate_;
		if (curr_rate < rate_)
			curr_rate = rate_;
		
	}
	
	interval_ = (double)(size_ << 3)/(double)curr_rate;
	double t = interval_;

	size = size_;
	if (++seqno_ < maxpkts_)
		return(t);
	else
		return(-1); 
}
