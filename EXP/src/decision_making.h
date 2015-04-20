#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <argos3/core/utility/math/rng.h>
#include <fstream>

using namespace argos;

static const UInt8  NUM_ROOMS            = 4;

class CDecisionMaking : public CLoopFunctions {

public:

   /**
    * Class constructor
    */
   CDecisionMaking();

   /** 
    * Class destructor
    */
   virtual ~CDecisionMaking();

   /**
    * Initializes the experiment.
    * It is executed once at the beginning of the experiment, i.e., when ARGoS is launched.
    * @param t_tree The parsed XML tree corresponding to the <loop_functions> section.
    */
   virtual void Init(TConfigurationNode& t_tree);

   /**
    * Resets the experiment to the state it was right after Init() was called.
    * It is executed every time you press the 'reset' button in the GUI.
    */
   virtual void Reset();

   /**
    * Undoes whatever Init() did.
    * It is executed once when ARGoS has finished the experiment.
    */
   virtual void Destroy();

   /**
    * Performs actions right before a simulation step is executed.
    */
   virtual void PreStep();

   /**
    * Performs actions right after a simulation step is executed.
    */
   virtual void PostStep();

   /**
    * Returns the color of the floor at the specified point on.
    * @param c_position_on_plane The position at which you want to get the color.
    * @see CColor
    */
   virtual CColor GetFloorColor(const CVector2& c_position_on_plane);


private:

   /**
    * The path of the output file.
    */
   std::string m_strOutFile;

   /**
    * The stream associated to the output file.
    */
   std::ofstream m_cOutFile;

   /**
    * This vector contains a list of positions of objects in the construction area
    */
   //std::vector<CVector3> m_vecConstructionObjectsInArea;

   /**
    * Minimum and maximum Y coordinate for the objects in the construction area
    */
   //Real m_fMinObjectY, m_fMaxObjectY;

   /**
    * Gray level of the floor of each room
    */
   Real m_fFloorGrayLevel[NUM_ROOMS];
   /**
    * Gray level range
    */
   CRange<Real> m_cFloorLevelRange;


   /**
    * Light intensity of each room
    */
   Real m_fLightIntensity[NUM_ROOMS];
   /**
    * Light intensity range
    */
   CRange<Real> m_cLightIntensityRange;

   /**
    * Number of red objects in each room
    */
   UInt16 m_unNumObjects[NUM_ROOMS];
   /**
    * Number of objects range
    */
   CRange<UInt32> m_cNumObjectRange;

   /**
    * X and Y limits for each room
    */
   Real m_fRoomLimits[NUM_ROOMS][4];

   /**
    * Room evaluations
    */
   Real m_fRoomEval[NUM_ROOMS];

   /**
    * Index of the best room
    */
   UInt8 m_unBestRoom;

   /**
    * Evaluation of the best room
    */
   Real m_fBestEval;

   /**
    * Random number generator
    */
   CRandom::CRNG* m_pcRNG;

   void AddLights();
   void AddObjects();
};
