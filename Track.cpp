/*-------------------------------------------------------------------------*
 *---									---*
 *---		Track.cpp						---*
 *---									---*
 *---	    This file defines a class that represents a length of	---*
 *---	Track: where only 'MAX_ALLOWED_NUM_TRAINS_ON_TRACK' Train	---*
 *---	instances may be without incident. 				---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0		2013 May 10		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"headers.h"



//  PURPOSE:  To return a pointer to the next TrainLocation for '*trainPtr'
//  	after it leaves '*this'.
TrainLocation*	Track::nextLocPtr
				(Train*		trainPtr
				)
{
  //  I.  Application validity check:

  //  II.  Return track:
  return(termini[trainPtr->getDirection()]);
}


//  PURPOSE:  To make '*trainPtr' arrive at '*this'.  No return value.
void		Track::arrive	(Train*		trainPtr
				)
				throw()
{
  //  I.  Application validity check:

  //  II.  Make '*trainPtr' arrive at '*this':
  //  II.A.  Get lock on track:
  pthread_mutex_lock(getLockPtr());
  if  ( !trainPtr->getMassTransit().getShouldContinue() )
  {
    pthread_mutex_unlock(getLockPtr());
    pthread_cond_signal(&isAvailableCond);
    return;
  }


  //  II.B.  Wait until '*this' Track is clear:
  while  (getNumTrains() >= MAX_ALLOWED_NUM_TRAINS_ON_TRACK)
  {
    pthread_cond_wait(&isAvailableCond, getLockPtr());

    if  ( !trainPtr->getMassTransit().getShouldContinue() )
    {
      pthread_mutex_lock(getLockPtr());
      pthread_cond_signal(&isAvailableCond);
      return;
    }

  }

  //  II.C.  Put '*trainPtr' here:
  enqueue(trainPtr);
  trainPtr->setLocPtr(this);
  //  III.  Finished:
}


//  PURPOSE:  To make '*trainPtr' leave '*this'.  No return value.
void		Track::leave	(Train*		trainPtr
  				)
				throw()
{
  //  I.  Application validity check:

  //  II.  Make '*trainPtr' leave '*this':
  
  dequeue(trainPtr);
  trainPtr->setLocPtr(NULL);
  pthread_cond_signal(&isAvailableCond);
  pthread_mutex_unlock(getLockPtr());
  //  III.  Finished:
}
