/*******************************************************************************
Indi observatory roof driver for an Odroid board with relays and switches attached to GPIO pins.
*******************************************************************************/
#include "odroidroof.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include <memory>

#include <indicom.h>

std::unique_ptr<OdroidRoof> rollOff(new OdroidRoof());

// This is the max ontime for the motors. Safety cut out. Although a lot of damage can be done on this time!!
#define MAX_ROLLOFF_DURATION    20    //TODO: Check if this is enough time.

void ISPoll(void *p);

void ISInit()
{
   static int isInit =0;

   if (isInit == 1)
       return;

    isInit = 1;
    if(rollOff.get() == 0) rollOff.reset(new OdroidRoof());

}

void ISGetProperties(const char *dev)
{
        ISInit();
        rollOff->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num)
{
        ISInit();
        rollOff->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num)
{
        ISInit();
        rollOff->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num)
{
        ISInit();
        rollOff->ISNewNumber(dev, name, values, names, num);
}

void ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n)
{
  INDI_UNUSED(dev);
  INDI_UNUSED(name);
  INDI_UNUSED(sizes);
  INDI_UNUSED(blobsizes);
  INDI_UNUSED(blobs);
  INDI_UNUSED(formats);
  INDI_UNUSED(names);
  INDI_UNUSED(n);
}

void ISSnoopDevice (XMLEle *root)
{
    ISInit();
    rollOff->ISSnoopDevice(root);
}

OdroidRoof::OdroidRoof()
{
  fullOpenLimitSwitch   = ISS_OFF;
  fullClosedLimitSwitch = ISS_OFF;
  MotionRequest=0;
  SetDomeCapability(DOME_CAN_ABORT | DOME_CAN_PARK);
}

/**
 * Init all properties
 */
bool OdroidRoof::initProperties()
{
    DEBUG(INDI::Logger::DBG_DEBUG, "Init props");
    INDI::Dome::initProperties();
    SetParkDataType(PARK_NONE);
    addAuxControls();
    IUFillText(&CurrentStateT[0],"State","Roof State",NULL);
    IUFillTextVector(&CurrentStateTP,CurrentStateT,1,getDeviceName(),"STATE","ROOF_STATE",MAIN_CONTROL_TAB,IP_RO,60,IPS_IDLE);
    return true;
}

bool OdroidRoof::ISSnoopDevice (XMLEle *root)
{
	return INDI::Dome::ISSnoopDevice(root);
}


bool OdroidRoof::SetupParms()
{
    DEBUG(INDI::Logger::DBG_DEBUG, "Setting up params");
    fullOpenLimitSwitch   = ISS_OFF;
    fullClosedLimitSwitch = ISS_OFF;
    std::string roofStateString = "UNKNOWN";
    if (getFullOpenedLimitSwitch()) {
        DEBUG(INDI::Logger::DBG_DEBUG, "Setting open flag on NOT PARKED");
        fullOpenLimitSwitch = ISS_ON;
        setDomeState(DOME_IDLE);
        roofStateString = "OPEN";
    }
    if (getFullClosedLimitSwitch()) {
        DEBUG(INDI::Logger::DBG_DEBUG, "Setting closed flag on PARKED");
        fullClosedLimitSwitch = ISS_ON;
        setDomeState(DOME_PARKED);
        if(isParked()) {
          roofStateString = "PARKED CLOSED";
        } else {
          roofStateString = "CLOSED";
        }
    }
    char status[32];
    strcpy(status, roofStateString.c_str());
    IUSaveText(&CurrentStateT[0], status);
    IDSetText(&CurrentStateTP, NULL);
    return true;
}


bool OdroidRoof::Connect()
{
    DEBUG(INDI::Logger::DBG_SESSION, "Connecting");
    return true;
}

OdroidRoof::~OdroidRoof()
{

}

const char * OdroidRoof::getDefaultName()
{
    return (char *)"Odroid Roof";
}

bool OdroidRoof::ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n)
{
	return INDI::Dome::ISNewSwitch(dev, name, states, names, n);
}


bool OdroidRoof::updateProperties()
{
    DEBUG(INDI::Logger::DBG_SESSION, "Updating props");
    INDI::Dome::updateProperties();

    if (isConnected())
    {
        SetupParms();
        defineText(&CurrentStateTP);
    } else
    {
		deleteProperty(CurrentStateTP.name);
	}

    return true;
}

/**
* Disconnect from the arduino
**/
bool OdroidRoof::Disconnect()
{
    DEBUG(INDI::Logger::DBG_SESSION, "Disconnecting");
    return true;
}

/**
 * TimerHit gets called by the indi client every 1sec when the roof is moving.
 */
void OdroidRoof::TimerHit()
{

    DEBUG(INDI::Logger::DBG_DEBUG, "Timer hit");
    if(isConnected() == false) return;  //  No need to reset timer if we are not connected anymore

   if (DomeMotionSP.s == IPS_BUSY)
   {
       // Abort called
       if (MotionRequest < 0)
       {
           DEBUG(INDI::Logger::DBG_SESSION, "Roof motion is stopped.");
           setDomeState(DOME_IDLE);
           std::string stateString = "ABORTED";
           char status[32];
           strcpy(status, stateString.c_str());
           IUSaveText(&CurrentStateT[0], status);
           IDSetText(&CurrentStateTP, NULL);
	       SetTimer(500);
           return;
       }

       // Roll off is opening
       if (DomeMotionS[DOME_CW].s == ISS_ON)
       {
           IDSetText(&CurrentStateTP, "OPENING");
           if (getFullOpenedLimitSwitch())
           {
               DEBUG(INDI::Logger::DBG_SESSION, "Roof is open.");
               setDomeState(DOME_UNPARKED);
               DEBUG(INDI::Logger::DBG_SESSION, "Sending ABORT to stop motion");
               //TODO: switch off relays here
               //SetParked(false);
               //calling setParked(false) here caauses the driver to crash with nothing logged (looks like possibly an issue writing parking data). Therefore the next 4 lines are doing what is done in indidome.cpp' function. We dont care about parking data anyway as we get the parked state directly from the roof stop-switches.
               IUResetSwitch(&ParkSP);
               ParkS[1].s = ISS_ON;
               ParkSP.s = IPS_OK;
               //IDSetSwitch(&ParkSP, NULL);
               std::string stateString = "OPEN";
               char status[32];
               strcpy(status, stateString.c_str());
               IUSaveText(&CurrentStateT[0], status);
               IDSetText(&CurrentStateTP, NULL);
               return;
           }
           if (CalcTimeLeft(MotionStart) <= 0) {
               DEBUG(INDI::Logger::DBG_SESSION, "Exceeded max motor run duration. Aborting.");
               Abort();
           }
       }
       // Roll Off is closing
       else if (DomeMotionS[DOME_CCW].s == ISS_ON)
       {
           if (getFullClosedLimitSwitch())
           {
                DEBUG(INDI::Logger::DBG_SESSION, "Sending ABORT to stop motion");
                //TODO: switch off relays here
                DEBUG(INDI::Logger::DBG_SESSION, "Roof is closed.");
                setDomeState(DOME_PARKED);
                //SetParked(true); FIXME: strange bug here that crashes the driver
                std::string stateString = "CLOSED";
                char status[32];
                strcpy(status, stateString.c_str());
                IUSaveText(&CurrentStateT[0], status);
                IDSetText(&CurrentStateTP, NULL);
                return;
           }
           if (CalcTimeLeft(MotionStart) <= 0) {
               DEBUG(INDI::Logger::DBG_SESSION, "Exceeded max motor run duration. Aborting.");
               Abort();
           }
       }
       SetTimer(500);
   }
}

bool OdroidRoof::saveConfigItems(FILE *fp)
{
    return INDI::Dome::saveConfigItems(fp);
}

/**
 * Move the roof. 
 *
 **/
IPState OdroidRoof::Move(DomeDirection dir, DomeMotionCommand operation)
{
    if (operation == MOTION_START)
    {
        updateProperties();
        // DOME_CW --> OPEN. If can we are ask to "open" while we are fully opened as the limit switch indicates, then we simply return false.
        if (dir == DOME_CW && fullOpenLimitSwitch == ISS_ON)
        {
            DEBUG(INDI::Logger::DBG_WARNING, "Roof is already fully opened.");
            return IPS_ALERT;
        }
        else if (dir == DOME_CCW && INDI::Dome::isLocked())
        {
            DEBUG(INDI::Logger::DBG_WARNING, "Cannot close dome when mount is locking. See: Telescope parkng policy, in options tab");
            return IPS_ALERT;
        }
        else if (dir == DOME_CW && getWeatherState() == IPS_ALERT)
        {
            DEBUG(INDI::Logger::DBG_WARNING, "Weather conditions are in the danger zone. Cannot open roof.");
            return IPS_ALERT;
        }
        else if (dir == DOME_CCW && fullClosedLimitSwitch == ISS_ON)
        {
            DEBUG(INDI::Logger::DBG_WARNING, "Roof is already fully closed.");
            return IPS_ALERT;
        }
        else if (dir == DOME_CW)
        {
            DEBUG(INDI::Logger::DBG_SESSION, "OPENING");
            //TODO: Switch on the relays here that will open the roof
        }
        else if (dir == DOME_CCW)
        {
            DEBUG(INDI::Logger::DBG_SESSION, "CLOSEING");
            //TODO: Switch on the relays here that will close the roof
        }

        MotionRequest = MAX_ROLLOFF_DURATION;
        gettimeofday(&MotionStart,NULL);
        SetTimer(500);
        DEBUG(INDI::Logger::DBG_SESSION, "return IPS_BUSY");
        return IPS_BUSY;
    }
    else
    {
        DEBUG(INDI::Logger::DBG_SESSION, "WTF WTF ");
        return (Dome::Abort() ? IPS_OK : IPS_ALERT);

    }
    DEBUG(INDI::Logger::DBG_SESSION, "return IPS_ALERT");
    return IPS_ALERT;

}

/**
 * Park the roof = close
 **/
IPState OdroidRoof::Park()
{
    IPState rc = INDI::Dome::Move(DOME_CCW, MOTION_START);
    if (rc==IPS_BUSY)
    {
        DEBUG(INDI::Logger::DBG_SESSION, "Roll off is parking...");
        return IPS_BUSY;
    }
    else
        return IPS_ALERT;
}

/**
 * Unpark the roof = open
 **/
IPState OdroidRoof::UnPark()
{
    IPState rc = INDI::Dome::Move(DOME_CW, MOTION_START);
    if (rc==IPS_BUSY) {
           DEBUG(INDI::Logger::DBG_SESSION, "Roll off is unparking...");
           return IPS_BUSY;
    }
    else
      return IPS_ALERT;
}

/**
 * Abort motion.
 **/
bool OdroidRoof::Abort()
{
    DEBUG(INDI::Logger::DBG_SESSION, "ABORT! Stopping motors");
    //TODO: switch off the relays here
    MotionRequest=-1;

    // If both limit switches are off, then we're neither parked nor unparked or a hardware failure (cable / rollers / jam).
    if (getFullOpenedLimitSwitch() == false && getFullClosedLimitSwitch() == false)
    {
        IUResetSwitch(&ParkSP);
        ParkSP.s = IPS_ALERT;
        IDSetSwitch(&ParkSP, NULL);
    }

    return true;
}

float OdroidRoof::CalcTimeLeft(timeval start)
{
    double timesince;
    double timeleft;
    struct timeval now;
    gettimeofday(&now,NULL);

    timesince=(double)(now.tv_sec * 1000.0 + now.tv_usec/1000) - (double)(start.tv_sec * 1000.0 + start.tv_usec/1000);
    timesince=timesince/1000;
    timeleft=MotionRequest-timesince;
    return timeleft;
}

/**
 * Get the state of the full open limit switch. This function will also switch off the motors as a safety override.
 **/
bool OdroidRoof::getFullOpenedLimitSwitch()
{
    DEBUG(INDI::Logger::DBG_SESSION, "Checking fully open switch");
    // TODO: Check the GPIO based switch here!!!!
    return false;

}

/**
 * Get the state of the full closed limit switch. This function will also switch off the motors as a safety override.
 **/
bool OdroidRoof::getFullClosedLimitSwitch()
{
    DEBUG(INDI::Logger::DBG_SESSION, "Checking fully closed switch");
    // TODO: Check the GPIO based switch here!!!!
    return false;
}
