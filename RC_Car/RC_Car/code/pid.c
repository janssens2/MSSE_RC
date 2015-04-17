/*
 * pid.c
 *
 * Created: 4/16/2015 9:56:45 PM
 *  Author: janssens
 */ 

SPid *myPid1;
SPid *myPid2;

int16_t updatePID( SPid *pid, int32_t error, int32_t position )
{
	double pTerm = 0.0;
	double dTerm = 0.0;
	double iTerm = 0.0;
	
	if ( toDebug > 2 )
	{
		serial_print( "args err=%.1f position=%.1f iState=%.1f dState=%.1f", error, position, pid->iState, pid->dState );
	}
	
	pTerm = pid->pGain * error; // calculate proportional term

	pid->iState += error; // calculate integral state
	if ( pid->iState > pid->iMax )
	{
		pid->iState = pid->iMax;
	}
	else if ( pid->iState < pid->iMin )
	{
		pid->iState = pid->iMin;
	}
	
	iTerm = pid->iGain * pid->iState; // calculate the integral term
	
	//if ( pid->dState != 0.0 )
	//{
	dTerm = pid->dGain * (pid->dState - position);
	//}
	pid->dState = position;
	
	if ( toDebug > 2 )
	{
		serial_print( "p=%.1f d=%.1f i=%.1f", pTerm, dTerm, iTerm);
	}
	
	return (int16_t) (pTerm + dTerm + iTerm);
}