#include "decision_making.h"

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/simulator/entities/cylinder_entity.h>
#include <argos3/plugins/simulator/entities/light_entity.h>
#include <argos3/plugins/simulator/entities/led_entity.h>
#include <argos3/plugins/simulator/media/led_medium.h>

#include <algorithm>
#include <cstring>
#include <cerrno>

/****************************************/
/****************************************/

static const Real ROOMC_MINX            = -1.5f;
static const Real ROOMC_MAXX            = 1.5f;
static const Real ROOMC_MINY            = -1.5f;
static const Real ROOMC_MAXY            = 1.5f;
static const Real ROOM0_MINX            = -1.5f;
static const Real ROOM0_MAXX            = 1.5f;
static const Real ROOM0_MINY            = 1.5f;
static const Real ROOM0_MAXY            = 4.5f;
static const Real ROOM1_MINX            = 1.5f;
static const Real ROOM1_MAXX            = 4.5f;
static const Real ROOM1_MINY            = -1.5f;
static const Real ROOM1_MAXY            = 1.5f;
static const Real ROOM2_MINX            = -1.5f;
static const Real ROOM2_MAXX            = 1.5f;
static const Real ROOM2_MINY            = -4.5f;
static const Real ROOM2_MAXY            = -1.5f;
static const Real ROOM3_MINX            = -4.5f;
static const Real ROOM3_MAXX            = -1.5f;
static const Real ROOM3_MINY            = -1.5f;
static const Real ROOM3_MAXY            = 1.5f;

static const UInt8  MAX_NUM_OBJECTS      = 12;
static const UInt8  MIN_NUM_OBJECTS      = 2;
static const Real OBJECT_RADIUS          = 0.1f;
static const Real OBJECT_DIAMETER        = OBJECT_RADIUS * 2.0f;
static const Real OBJECT_HEIGHT        = 0.15f;

/****************************************/
/****************************************/

CDecisionMaking::CDecisionMaking(): m_cFloorLevelRange(0.0f,1.0f), m_cLightIntensityRange(0.0f,1.0f),
    m_cNumObjectRange(MIN_NUM_OBJECTS,MAX_NUM_OBJECTS),m_fBestEval(0)
{

    m_fRoomLimits[0][0] = ROOM0_MINX;
    m_fRoomLimits[0][1] = ROOM0_MAXX;
    m_fRoomLimits[0][2] = ROOM0_MINY;
    m_fRoomLimits[0][3] = ROOM0_MAXY;
    m_fRoomLimits[1][0] = ROOM1_MINX;
    m_fRoomLimits[1][1] = ROOM1_MAXX;
    m_fRoomLimits[1][2] = ROOM1_MINY;
    m_fRoomLimits[1][3] = ROOM1_MAXY;
    m_fRoomLimits[2][0] = ROOM2_MINX;
    m_fRoomLimits[2][1] = ROOM2_MAXX;
    m_fRoomLimits[2][2] = ROOM2_MINY;
    m_fRoomLimits[2][3] = ROOM2_MAXY;
    m_fRoomLimits[3][0] = ROOM3_MINX;
    m_fRoomLimits[3][1] = ROOM3_MAXX;
    m_fRoomLimits[3][2] = ROOM3_MINY;
    m_fRoomLimits[3][3] = ROOM3_MAXY;

}

/****************************************/
/****************************************/

CDecisionMaking::~CDecisionMaking() {
    /* Nothing to do */
}

/****************************************/
/****************************************/

void CDecisionMaking::Init(TConfigurationNode& t_tree) {
    /* Get output file name from XML tree */
    GetNodeAttribute(t_tree, "output", m_strOutFile);
    /* Open the file for text writing */
    m_cOutFile.open(m_strOutFile.c_str(), std::ofstream::out | std::ofstream::trunc);
    if(m_cOutFile.fail()) {
        THROW_ARGOSEXCEPTION("Error opening file \"" << m_strOutFile << "\": " << ::strerror(errno));
    }

    m_pcRNG = CRandom::CreateRNG("argos");
    /* Write a header line */
    m_cOutFile << "#Clock\t";
    /* Initialize room configuration */
    size_t i;
    for(i = 0; i < NUM_ROOMS; ++i){
        m_fFloorGrayLevel[i] = m_pcRNG->Uniform(m_cFloorLevelRange);
        LOG << "Floor color in room " << i << ": " << m_fFloorGrayLevel[i] << std::endl;
        m_fLightIntensity[i] = m_pcRNG->Uniform(m_cLightIntensityRange);
        LOG << "Light intensity in room " << i << ": " << m_fLightIntensity[i] << std::endl;
        m_unNumObjects[i] = m_pcRNG->Uniform(m_cNumObjectRange);
        LOG << "Num objects in room " << i << ": " << m_unNumObjects[i] << " evaluated: " << ((Real)(m_unNumObjects[i] - MIN_NUM_OBJECTS) / (Real)(MAX_NUM_OBJECTS-MIN_NUM_OBJECTS)) << std::endl;
        m_fRoomEval[i] = (m_fFloorGrayLevel[i] + m_fLightIntensity[i] + ((Real)(m_unNumObjects[i] - MIN_NUM_OBJECTS) / (Real)(MAX_NUM_OBJECTS-MIN_NUM_OBJECTS))) / 3.0f;
        LOG << "Room " << i << " evaluation: " << m_fRoomEval[i] << std::endl;
        if(m_fRoomEval[i] >= m_fBestEval){
            m_fBestEval = m_fRoomEval[i];
            m_unBestRoom = i;
        }
        /* Header line */
        m_cOutFile << "Room" << i << "\t";
    }
    /* Header line */
    m_cOutFile << std::endl;
    LOG << "Best is room number " << m_unBestRoom << " with evaluation: " << m_fBestEval << std::endl;

    AddLights();
    AddObjects();
}

/****************************************/
/****************************************/
void CDecisionMaking::AddLights() {
    CLEDMedium* cLEDMedium = &CSimulator::GetInstance().GetMedium<CLEDMedium>("leds");

    CLightEntity* light = new CLightEntity("light_" + ToString(0),CVector3(0,3,0.7),CColor::YELLOW,m_fLightIntensity[0]);
    light->AddToMedium(*cLEDMedium);
    AddEntity(*light);

    light = new CLightEntity("light_" + ToString(1),CVector3(3,0,0.7),CColor::YELLOW,m_fLightIntensity[1]);
    light->AddToMedium(*cLEDMedium);
    AddEntity(*light);

    light = new CLightEntity("light_" + ToString(2),CVector3(0,-3,0.7),CColor::YELLOW,m_fLightIntensity[2]);
    light->AddToMedium(*cLEDMedium);
    AddEntity(*light);

    light = new CLightEntity("light_" + ToString(3),CVector3(-3,0,0.7),CColor::YELLOW,m_fLightIntensity[3]);
    light->AddToMedium(*cLEDMedium);
    AddEntity(*light);
}

/****************************************/
/****************************************/
void CDecisionMaking::AddObjects() {
    size_t room_index;
    size_t object_index;
    CCylinderEntity* pcCylinder;
    for(room_index = 0; room_index < NUM_ROOMS; ++room_index) {
        for(object_index = 0; object_index < m_unNumObjects[room_index]; ++object_index) {
            pcCylinder = new CCylinderEntity("object_" + ToString(room_index) +  "_" + ToString(object_index),
                                             CVector3(m_pcRNG->Uniform(CRange<Real>(m_fRoomLimits[room_index][0]+OBJECT_DIAMETER+OBJECT_RADIUS+0.1f,m_fRoomLimits[room_index][1]-OBJECT_DIAMETER-OBJECT_RADIUS-0.1f)),
                                             m_pcRNG->Uniform(CRange<Real>(m_fRoomLimits[room_index][2]+OBJECT_DIAMETER+OBJECT_RADIUS+0.1f,m_fRoomLimits[room_index][3]-OBJECT_DIAMETER-OBJECT_RADIUS-0.1f)),
                                             0),CQuaternion().FromEulerAngles(CRadians::ZERO,CRadians::ZERO,CRadians::ZERO),
                                             false,OBJECT_RADIUS,OBJECT_HEIGHT);
            pcCylinder->GetLEDEquippedEntity().AddLED(CVector3(0,0,0.16f),CColor::GREEN);
            CLEDMedium* cLEDMedium = &CSimulator::GetInstance().GetMedium<CLEDMedium>("leds");
            pcCylinder->GetLEDEquippedEntity().AddToMedium(*cLEDMedium);
            AddEntity(*pcCylinder);
        }
    }
}


/****************************************/
/****************************************/

void CDecisionMaking::Reset() {
    /* Close the output file */
    m_cOutFile.close();
    if(m_cOutFile.fail()) {
        THROW_ARGOSEXCEPTION("Error closing file \"" << m_strOutFile << "\": " << ::strerror(errno));
    }
    /* Open the file for text writing */
    m_cOutFile.open(m_strOutFile.c_str(), std::ofstream::out | std::ofstream::trunc);
    if(m_cOutFile.fail()) {
        THROW_ARGOSEXCEPTION("Error opening file \"" << m_strOutFile << "\": " << ::strerror(errno));
    }
    /* Write a header line */
    m_cOutFile << "#Clock\t";
    size_t i;
    for(i = 0; i < NUM_ROOMS; ++i){
        m_cOutFile << "Room" << i << "\t";
    }
    m_cOutFile << std::endl;
}

/****************************************/
/****************************************/

void CDecisionMaking::Destroy() {
    /* Close the output file */
    m_cOutFile.close();
    if(m_cOutFile.fail()) {
        THROW_ARGOSEXCEPTION("Error closing file \"" << m_strOutFile << "\": " << ::strerror(errno));
    }
}

/****************************************/
/****************************************/

void CDecisionMaking::PreStep() {
    /* Nothing to do */
}

/****************************************/
/****************************************/

void CDecisionMaking::PostStep() {
     /* Output a line for this step */
     UInt32 m_uNumberOfRobotsinRoom0=0;
     UInt32 m_uNumberOfRobotsinRoom1=0;
     UInt32 m_uNumberOfRobotsinRoom2=0;
     UInt32 m_uNumberOfRobotsinRoom3=0;
     CSpace::TMapPerType& cFBMap = GetSpace().GetEntitiesByType("foot-bot");
		for(CSpace::TMapPerType::iterator it = cFBMap.begin(); it != cFBMap.end(); ++it) {
			CFootBotEntity& footbotEntity = *any_cast<CFootBotEntity*>(it->second);
			Real Robot_X = footbotEntity.GetEmbodiedEntity().GetPosition().GetX();
			Real Robot_Y = footbotEntity.GetEmbodiedEntity().GetPosition().GetY();
			if((Robot_X>=ROOM0_MINX)&&(Robot_X<=ROOM0_MAXX)&&(Robot_Y<=ROOM0_MAXY)&&(Robot_Y>=ROOM0_MINY)){
			m_uNumberOfRobotsinRoom0+=1;
			}else if((Robot_X>=ROOM1_MINX)&&(Robot_X<=ROOM1_MAXX)&&(Robot_Y<=ROOM1_MAXY)&&(Robot_Y>=ROOM1_MINY)){
			m_uNumberOfRobotsinRoom1+=1;
			}else if((Robot_X>=ROOM2_MINX)&&(Robot_X<=ROOM2_MAXX)&&(Robot_Y<=ROOM2_MAXY)&&(Robot_Y>=ROOM2_MINY)){
			m_uNumberOfRobotsinRoom2+=1;
			}else if((Robot_X>=ROOM3_MINX)&&(Robot_X<=ROOM3_MAXX)&&(Robot_Y<=ROOM3_MAXY)&&(Robot_Y>=ROOM3_MINY)){
			m_uNumberOfRobotsinRoom3+=1;
			}
		}
     m_cOutFile << GetSpace().GetSimulationClock() << "\t"
                << m_uNumberOfRobotsinRoom0 << "\t"
                << m_uNumberOfRobotsinRoom1 << "\t"
                << m_uNumberOfRobotsinRoom2 << "\t"
                << m_uNumberOfRobotsinRoom3 << "\t"
                << std::endl;
}

/****************************************/
/****************************************/

CColor CDecisionMaking::GetFloorColor(const CVector2& c_position_on_plane) {
    /* Check if the given point is within the central room */
    if(c_position_on_plane.GetX() >= ROOMC_MINX && c_position_on_plane.GetX() <= ROOMC_MAXX &&
            c_position_on_plane.GetY() >= ROOMC_MINY && c_position_on_plane.GetY() <= ROOMC_MAXY) {
        /* Yes, it is - return black */
        return CColor::BLACK;
        /* Check if the given point is within room 1 */
    } else if(c_position_on_plane.GetX() >= m_fRoomLimits[0][0] && c_position_on_plane.GetX() <= m_fRoomLimits[0][1] &&
              c_position_on_plane.GetY() >= m_fRoomLimits[0][2] && c_position_on_plane.GetY() <= m_fRoomLimits[0][3]) {
        /* Yes, it is - return its floor gray level */
        return CColor(m_fFloorGrayLevel[0]*255,m_fFloorGrayLevel[0]*255,m_fFloorGrayLevel[0]*255);
        /* Check if the given point is within room 2*/
    } else if(c_position_on_plane.GetX() >= m_fRoomLimits[1][0] && c_position_on_plane.GetX() <= m_fRoomLimits[1][1] &&
              c_position_on_plane.GetY() >= m_fRoomLimits[1][2] && c_position_on_plane.GetY() <= m_fRoomLimits[1][3]) {
        /* Yes, it is - return its floor gray level */
        return CColor(m_fFloorGrayLevel[1]*255,m_fFloorGrayLevel[1]*255,m_fFloorGrayLevel[1]*255);
        /* Check if the given point is within room 3*/
    } else if(c_position_on_plane.GetX() >= m_fRoomLimits[2][0] && c_position_on_plane.GetX() <= m_fRoomLimits[2][1] &&
              c_position_on_plane.GetY() >= m_fRoomLimits[2][2] && c_position_on_plane.GetY() <= m_fRoomLimits[2][3]) {
        /* Yes, it is - return its floor gray level */
        return CColor(m_fFloorGrayLevel[2]*255,m_fFloorGrayLevel[2]*255,m_fFloorGrayLevel[2]*255);
        /* Check if the given point is within room 4*/
    } else if(c_position_on_plane.GetX() >= m_fRoomLimits[3][0] && c_position_on_plane.GetX() <= m_fRoomLimits[3][1] &&
              c_position_on_plane.GetY() >= m_fRoomLimits[3][2] && c_position_on_plane.GetY() <= m_fRoomLimits[3][3]) {
        /* Yes, it is - return its floor gray level */
        return CColor(m_fFloorGrayLevel[3]*255,m_fFloorGrayLevel[3]*255,m_fFloorGrayLevel[3]*255);
    }
    /* No, it isn't - return white */
    return CColor::WHITE;
}

/****************************************/
/****************************************/

/* Register this loop functions into the ARGoS plugin system */
REGISTER_LOOP_FUNCTIONS(CDecisionMaking, "decision_making");
