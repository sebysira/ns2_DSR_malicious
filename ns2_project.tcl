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
set val(initialenergy)  100                      ;#  (in Joule)
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

set val(x)	1200
set val(y)	1200
set val(nn)	20				   ;# number of nodes

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
    set sum_confirmed($i) 0.00
    set sum_sended($i) 0.00
	# disable random motion for static network
	$node($i) random-motion 1
	$node($i) start
}

set source_node_list {0 2 4 6 8 10 12 14 16 18}
set dest_node_list {1 3 5 7 9 11 13 15 17 19}

for {set i 0} {$i < [llength $source_node_list]} {incr i} {
    #Create udp agent
    set udp($i) [new Agent/UDP]
    set source [lindex $source_node_list $i]
    $ns attach-agent $node($source) $udp($i)

    #create cbr
    set cbr($i) [new Application/Traffic/CBR]
    $cbr($i) set packetSize_ 512
    $cbr($i) set interval_ 0.2
    $cbr($i) set random_ 1
    $cbr($i) set maxpkts_ 100000
    $cbr($i) attach-agent $udp($i)

    #Create a Null agent (a traffic sink)
    set sink($i) [new Agent/LossMonitor]
    set dest [lindex $dest_node_list $i]
    $ns attach-agent $node($dest) $sink($i)

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

proc print_result {} {
    global node val
    set ns [Simulator instance]

    close [open "result.txt" "w"]
    close [open "valuation.txt" "w"]

    for {set i 0} {$i < $val(nn)} {incr i} {    
        $ns at [$ns now] "[$node($i) set ragent_] printResult"
    }
    
    $ns at [$ns now] "print_final_result"
}

proc print_final_result {} {
    global sum_sended sum_confirmed val
    set ns [Simulator instance]
    set fp [open "valuation.txt" "r"]

    while { [gets $fp line] >= 0 } {
        set wordList [regexp -inline -all -- {\S+} $line]
        lassign $wordList field1 field2 field3
        set sum_sended($field1) [expr $sum_sended($field1) + $field2]
        set sum_confirmed($field1) [expr $sum_confirmed($field1) + $field3]
    }

    close $fp
    set fp [open "valuation.txt" "w"]

    for {set i 0} {$i < $val(nn)} {incr i} { 
        if {$sum_sended($i) == 0} {
            set valuation($i) -1
        } else {
            set valuation($i) [expr round((1 - $sum_confirmed($i) / $sum_sended($i)) * 100)] 
        }
        set valuation($i) [string map { . , } $valuation($i)]
        puts $fp $valuation($i)
    }

    close $fp
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

for {set i 0} {$i < [llength $source_node_list]} {incr i} {
    $ns at 1.0 "$cbr($i) start"
}

# Tell ns/nam the simulation stop time
$ns at 500 "print_result"
# Start your simulation
$ns run