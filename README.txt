Production board was :
StmPulse-V1-G49804

*Note* the design has a fault.  R2 and R5 are attached to 5V but should be attached to +5V

I added an airwire to link them on my build.

*NOTE*  the trigger output on board StmPulse-V1-G49804 was only 3.3V whihc is not enough to trigger some modules.

*StmPulse-V2-InProgress.sch*
This fixes the above faults, it connects R2 and R5 to +5V and adds output transistors on the gates to give 5V.

StmPulse-V2-InProgress has *NOT* been built or tested.
