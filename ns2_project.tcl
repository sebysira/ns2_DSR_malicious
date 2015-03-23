# Define options
set val(chan)           Channel/WirelessChannel    ;#Channel Type
set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type
set val(mac)            Mac/802_11                 ;# MAC type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         500                         ;# max packets in ifq
set val(rxPower)        0.2                      ;#  (in W)
set val(txPower)        0.6                     ;#  (in W)
set val(energymodel)    EnergyModel               ;# 
set val(initialenergy)  200                       ;#  (in Joule)
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

set source_node_list {0 5 10 3 7 9 13 12}
set dest_node_list {1 6 11 4 8 10 14 0}

for {set i 0} {$i < [llength $source_node_list]} {incr i} {
    set udp($i) [new Agent/UDP]
    set nabbo_tcl [lindex $source_node_list $i]
    $ns attach-agent $node($nabbo_tcl) $udp($i)

    #create cbr
    set cbr($i) [new Application/Traffic/CBR]
    $cbr($i) set packetSize_ 512
    $cbr($i) set interval_ 0.1
    $cbr($i) set random_ 1
    $cbr($i) set maxpkts_ 100000
    $cbr($i) attach-agent $udp($i)

    #Create a Null agent (a traffic sink) on node 4
    set sink($i) [new Agent/LossMonitor]
    set nabbo_tcl [lindex $dest_node_list $i]
    $ns attach-agent $node($nabbo_tcl) $sink($i)

    #Connet source and dest Agents
    $ns connect $udp($i) $sink($i)
}


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

    close [open "result.txt" "w"]

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
$ns at 1.0 "$cbr(0) start"
$ns at 1.0 "$cbr(1) start"
$ns at 1.0 "$cbr(2) start"

# Tell ns/nam the simulation stop time
$ns at 500 "stampa"
# Start your simulation
$ns run
