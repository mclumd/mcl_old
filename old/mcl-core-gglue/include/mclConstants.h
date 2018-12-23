#ifndef MCL_CODES_H
#define MCL_CODES_H

/////////////////////////////////////////////////////////////////
//
// GENERIC CODES AND DEFINES FOLLOW....
//

// these are indexes into the buildCore vector...
#define INDICATION_INDEX 0
#define FAILURE_INDEX    1
#define RESPONSE_INDEX   2

/////////////////////////////////////////////////////////////////
//
// FRAME CODES AND DEFINES FOLLOW....
//

/** framestate defines for tracking #mclFrame status.
 */
enum framestates { FRAME_NEW, //!< frame newly created
		   FRAME_RESPONSE_ISSUED, //!< MCL has made a recommendation
		   FRAME_RESPONSE_TAKEN,  //!< response probably durative and passive
		   FRAME_RESPONSE_ACTIVE, //!< MCL is monitoring the response
		   FRAME_RESPONSE_FAILED, //!< MCL or Host signal response failure
		   FRAME_RESPONSE_SUCCEEDED, //!< MCL or Host detect response success
		   FRAME_RESPONSE_IGNORED,//!< the Host is ignoring MCL
		   FRAME_RESPONSE_ABORTED,//!< the Host has aborted the resp.
		   FRAME_UPDATED,         //!< updated with feedback, ready for re-guide
		   FRAME_DEADEND,         //!< MCL is out of suggestions
		   FRAME_ERROR            //!< MCL has encountered an error
};

#endif
