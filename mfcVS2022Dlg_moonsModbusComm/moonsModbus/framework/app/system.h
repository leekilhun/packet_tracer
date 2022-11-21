#pragma once

/*
 * system.h
 * 
 * 작성자 : 이길훈
 * 22.04.04 
 * system header
 */

 /* 아래 순서대로 절차를 진행하며 가능한 interface를 상속한다 */
 /*
  system_def.h   <-   system.h
                         |
                      hal_xx.h
                         |
                      engine_xx.h
                         |
                      control_xx.h
                         |
                      ap_xx.h

 */



#include "system_def.h"

/* object*/
#include "system_obj.h"

/* data */
//#include "system_dat.h"

/* io*/
//#include "system_io.h"


/* mechanical object*/
//#include "cylinder.h"
//#include "vacuum.h"

//#include "system_op.h"
//#include "system_auto.h"
/* control object*/

/* ap object*/


namespace AP_SYS
{ 

    using namespace AP_OBJ;
    using namespace GMoons;
    using namespace std;

    class MSystem
    {
        /****************************************************
         *	data
         ****************************************************/
        char m_ProjectDir[_MAX_DIR];

        MSystemObject* m_pObjinfo;
        IComm* m_pSerialComm[static_cast<int>(SERIAL_OBJ::_max)];
        
        std::shared_ptr<GMoons_comm> m_pMoons_comm;
        std::shared_ptr<GMoons_motor> m_pMoonsMotor;

    public:

        /****************************************************
         *	Constructor
         ****************************************************/
    public:
        MSystem();
        virtual ~MSystem();

        /****************************************************
         *	func
         ****************************************************/
    private:
        void init();
        void terminate();
        void destroy();
        void startThreads();
        void stopThreads();
     
        errno_t _createSerialCommComponents();
        errno_t _createBasicHWComponents();
        errno_t _createMotionComponets();
        errno_t _createEngineComponets();
        errno_t _createApComponts();
   

    public:
        errno_t Initialize();
        IComm* GetSerialCommComponent(SERIAL_OBJ id) const;
        GMoons_comm* GetMoonsCommModbusComponent() const;
        GMoons_motor* GetMoonsMotorComponent() const;

    };

}