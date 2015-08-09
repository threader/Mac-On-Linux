
#ifndef __VideoDriverPrivate_H__
#include "VideoDriverPrivate.h"
#endif

#ifndef __VideoDriverPrototypes_H__
#include "VideoDriverPrototypes.h"
#endif


/*
 * Called on PBStatus, csCode = 43
 */
OSStatus
DriverGestaltHandler( CntrlParam *pb )
{
#define PB (*((DriverGestaltParam *) pb))
#define OPTIONS	(TheDriverDescription.driverOSRuntimeInfo)

	OSStatus status;
	Trace(DriverGestaltHandler);

	PB.driverGestaltResponse = 0;
	status = noErr;

	switch( PB.driverGestaltSelector ) {
	case kdgSync:
		PB.driverGestaltResponse = FALSE;	/* We handle asynchronous I/O */
		break;
	case kdgVersion:
		PB.driverGestaltResponse = 
			*((UInt32 *) &TheDriverDescription.driverType.version);
		break;
	case kdgDeviceType:
		PB.driverGestaltResponse = 'MOL_';
		break;
	case kdgInterface:
		PB.driverGestaltResponse = 'pci ';
		break;
	case kdgSupportsSwitching:			/* Support Power up/down switching? */
		PB.driverGestaltResponse = FALSE;	/* Not supported yet */
		break;
	case kdgSupportsPowerCtl:			/* TRUE if in high-power mode */
		PB.driverGestaltResponse = FALSE;	/* Power-switching is not supported */
		break;
	default:
		status = statusErr;			
		break;
	}
	return status;
#undef PB
}
