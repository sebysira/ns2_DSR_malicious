# Define options
set val(chan)           Channel/WirelessChannel    ;#Channel Type
set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type
set val(mac)            Mac/802_11                 ;# MAC type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         50                         ;# max packets in ifq
set val(rxPower)        0.2                      ;#  (in W)
set val(txPower)        0.6                     ;#  (in W)
set val(energymodel)    EnergyModel               ;# 
set val(initialenergy)  100                       ;#  (in Joule)
set val(sleeppower)     0.02                      ;# energia consumata in stato di sleep
set val(tp)             0.03                      ;# Energia consumata per la transizione dallo stato di sleep a quello di attivita'...
#set val(ifq)            Queue/DropTail/PriQueue    ;# interface queue type

# DumbAgent no routing!, AODV, DSDV, DSR 
set val(rp)	DSR; # 
if { $val(rp) == "DSR" } {
set val(ifq)            CMUPriQueue
} else {
set val(ifq)            Queue/DropTail/PriQueue
}

set val(x)	1000
set val(y)	1000
set val(nn)	15				   ;# number of nodes

# Create simulator
set ns [new Simulator]

# Create a topology in a 800m x 800m area
set topo [new Topography]
$topo load_flatgrid $val(x) $val(y)

# ns trace and nam trace
set f [open out.tr w]
$ns trace-all $f

set nf [open out.nam w]
$ns namtrace-all-wireless $nf $val(x) $val(y)

$ns use-newtrace

# Create God
set god [create-god $val(nn)]

set chan_1_ [new $val(chan)]

$ns node-config -adhocRouting $val(rp) \
                -llType $val(ll) \
                -macType $val(mac) \
                -ifqType $val(ifq) \
                -ifqLen $val(ifqlen) \
                -antType $val(ant) \
                -propType $val(prop) \
                -phyType $val(netif) \
                -topoInstance $topo \
                -agentTrace ON \
                -routerTrace ON \
                -macTrace ON \
                -movementTrace ON \
                -channel $chan_1_ \
				-energyModel $val(energymodel) \
 				-initialEnergy $val(initialenergy) \
 				-rxPower $val(rxPower) \
 				-txPower $val(txPower)

# Create mobile nodes
for {set i 0} {$i<$val(nn)} {incr i} {
	set node($i) [$ns node]
	# disable random motion for static network
	$node($i) random-motion 1
	$node($i) start
}

#Create a udp agent on node 0
set udp(0) [new Agent/UDP]
$ns attach-agent $node(0) $udp(0)

#set cbr(0) [new Application/Traffic/CBR]
#$cbr(0) set packetSize_ 512
#$cbr(0) set interval_ 0.25
#$cbr(0) set random_ 1
#$cbr(0) set maxpkts_ 100000
#$cbr(0) attach-agent $udp(0)


# Create a CBR traffic source on node 0
set vbr(0) [new Application/Traffic/VBR]
$vbr(0) set rate_ 448Kb
$vbr(0) set rate_dev_ 0.25
$vbr(0) set rate_time_ 2.0
$vbr(0) set burst_time_ 0.75
$vbr(0) set n_o_changes_ 10
$vbr(0) set time_dev_ 0.5
$vbr(0) set constant_ false
$vbr(0) set maxrate_ 648Kb
$vbr(0) set packetSize_ 1280
$vbr(0) set maxpkts_ 268435456
$vbr(0) attach-agent $udp(0)

#Create a Null agent (a traffic sink) on node 4
set sink(0) [new Agent/LossMonitor]
$ns attach-agent $node(1) $sink(0)

#Connet source and dest Agents
$ns connect $udp(0) $sink(0)

# Define node initial position in nam
for {set i 0} {$i < $val(nn)} {incr i} {
	$ns initial_node_pos $node($i) 30
}

proc load_perc {} {
    global node val
    set ns [Simulator instance]

    set fp [open "percentage.txt" "r"]
        
    for {set i 0} {$i < $val(nn)} {incr i} {
        set line [gets $fp]
        $ns at 0 "[$node($i) set ragent_] malicious $line"
        }
    close $fp
}

proc stampa {} {
    global node val
    set ns [Simulator instance]

    for {set i 0} {$i < $val(nn)} {incr i} {
        
        $ns at [$ns now] "[$node($i) set ragent_] stampa_file"
        }
    $ns at [$ns now] "finish"

}

proc finish {} {
    global f nf val node
    set ns [Simulator instance]
    $ns flush-trace
     
    close $f
    close $nf
    #puts "running nam..."
    exec nam out.nam &
    exit 0
}

$ns at 0 "load_perc"
$ns at 1.0 "$vbr(0) start"
# Tell ns/nam the simulation stop time
$ns at 500 "stampa"
# Start your simulation
$ns run
