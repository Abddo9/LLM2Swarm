#include "py_loop_function.h"

using namespace argos;
using namespace boost::python;

#define INIT_MODULE_LOOP_FUNCTION PyInit_libpy_loop_function_interface
extern "C" PyObject* INIT_MODULE_LOOP_FUNCTION();

// TODO: I had to add these lines and the line PyImport_AppendInittab("libpy_controller_interface", INIT_MODULE_CONTROLLER)
// in this file, otherwise I god an error that libpy_controller_interface is not a built-in module
#define INIT_MODULE_CONTROLLER PyInit_libpy_controller_interface
extern "C" PyObject* INIT_MODULE_CONTROLLER();

// TODO: I had to add these lines and the line PyImport_AppendInittab("libpy_qtuser_function_interface", INIT_MODULE_QTUSER_FUNCTION)
// in this file, otherwise I god an error that libpy_qtuser_function_interface is not a built-in module
#define INIT_MODULE_QTUSER_FUNCTION PyInit_libpy_qtuser_function_interface
extern "C" PyObject* INIT_MODULE_QTUSER_FUNCTION();

//boost::python::list allRobots;

BOOST_PYTHON_MODULE(libpy_loop_function_interface) {
    // Expose the CPyLoopFunction class
    //class_<CPyLoopFunction>("CPyLoopFunction")
        // Expose the AddEPuckEntityFromPython function
      //  .def("add_epuck_entity", &CPyLoopFunction::AddEPuckEntityFromPython)
    //;

}

CPyLoopFunction::CPyLoopFunction() : m_nextRobotID(0) {
  // init python

  // TODO: Remove from loop function and only call in controller
  // PyImport_AppendInittab("libpy_qtuser_function_interface", INIT_MODULE_QTUSER_FUNCTION); 
  PyImport_AppendInittab("libpy_controller_interface", INIT_MODULE_CONTROLLER); 
  // TODO: Remove from loop function and only call in controller

  PyImport_AppendInittab("libpy_loop_function_interface", INIT_MODULE_LOOP_FUNCTION);
  if (!Py_IsInitialized()) {
    Py_Initialize();
  }
  m_loop_interpreter = Py_NewInterpreter();
    // init main module and namespace
  m_loop_main = import("__main__");
  m_loop_namesp = m_loop_main.attr("__dict__");
}


void CPyLoopFunction::Init(TConfigurationNode& t_node) {
   float pos_tick=0.0;
  TConfigurationNode& tParams = GetNode(t_node, "params");
  
  /* Load script */
  std::string strScriptFileName;
  GetNodeAttributeOrDefault(tParams, "script", strScriptFileName, strScriptFileName);
  if (strScriptFileName == "") {
    THROW_ARGOSEXCEPTION("Loop function: Error loading python script \"" << strScriptFileName << "\""
      << std::endl);
  }
  // exec user script
  try {
    m_loop_script = exec_file(strScriptFileName.c_str(), m_loop_namesp, m_loop_namesp);

    std::cout << "Loop function: strScript:" << strScriptFileName << std::endl;
  } catch (error_already_set) {
    PyErr_Print();
  }


  // Iterate over all robots and add them to a boost list
  boost::python::list allRobots;    
  CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
  for(CSpace::TMapPerType::iterator it = m_cEpuck.begin(); it != m_cEpuck.end(); ++it)
  {
    /* Get handle to e-puck entity and controller */
    CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);

    CPyController& cController =  dynamic_cast<CPyController&>(cEpuck.GetControllableEntity().GetController());

    allRobots.append(cController.getActusensors());
  }
  m_loop_namesp["allrobots"]  = allRobots;
  
      //CRandom::CRNG* pcRNG = CRandom::CreateRNG("argos");
    
    
    
        CEPuckEntity* pcEPuck2 = nullptr;
        
            // Iterate over all e-puck entities
   
 //CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
  for(CSpace::TMapPerType::iterator it = m_cEpuck.begin(); it != m_cEpuck.end(); ++it)
  {

    CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);
    if (cEpuck.GetId() == "bc1" or cEpuck.GetId() == "bc0" or cEpuck.GetId() == "bc2" or cEpuck.GetId() == "bc3" or cEpuck.GetId() == "bc4" or cEpuck.GetId() == "bc5" or cEpuck.GetId() == "bc5" or cEpuck.GetId() == "bc0" or cEpuck.GetId() == "bc2") {
      pos_tick+=0.1;
            // Return the e-puck entity with the specified ID
            pcEPuck2= &cEpuck;
            

            //break;
        }
 
                         // If the foot-bot with ID 2 exists, move it to the new position
        if (pcEPuck2) {
            // Define the new position
            CVector3 newPosition(0.9-pos_tick, -0.93, 0.0); // Adjust the values as needed

            // Move the foot-bot to the new position
            MoveEntity(pcEPuck2->GetEmbodiedEntity(), newPosition, CQuaternion());
                    CEPuckEntity* pcEPuck2 = nullptr;
        }
  


  } 
  

  


  // To-do: find way to access environment handle to access CPyQTUserFunction from loop function
  /* Get handle to environment */
  // cEnvironment = dynamic_cast<CPyQTUserFunction&>();
  // m_environment = cEnvironment.getEnvironment();
  // m_loop_namesp["environment"]  = m_environment;
  
  try {
    // Import the wrapper's lib
    PyRun_SimpleString("import libpy_loop_function_interface as lib");
    object lib = import("libpy_loop_function_interface");
    

    // Launch Python init function
    object init_f = m_loop_main.attr("init");
    init_f();
  } catch (error_already_set) {
    PyErr_Print();
  }

}
/*

void CPyLoopFunction::AddEPuckEntity(const CVector3& position, const CQuaternion& orientation) {

     m_nextRobotID=m_nextRobotID+50;
         std::cout << "comes to add entity c++" << std::endl;
    std::string controllerID = "controller_" + std::to_string(m_nextRobotID);

        // Create an e-puck entity with consecutive IDs
    CEPuckEntity* pcEPuck = new CEPuckEntity(
        "epuck_" + std::to_string(m_nextRobotID), // Generate unique ID
        std::to_string(m_nextRobotID), // Empty string for the controller ID (assuming it's not needed)
        position,
        orientation
    );
       //CPyController& cController =  dynamic_cast<CPyController&>(pcEPuck->GetControllableEntity().GetController());
           //cController.Load("");

    //allRobots.append(cController.getActusensors());
 
  
  
}

*/
boost::python::list CPyLoopFunction::GetAllRobots() const {
    // Ensure that m_loop_namesp["allrobots"] returns a boost::python::list object
    boost::python::list allRobots;
    // Populate allRobots list...
    return allRobots;
}

void CPyLoopFunction::Reset() {
  // launch python reset function
  try {
    object reset_f = m_loop_main.attr("reset");
    reset_f();
  } catch (error_already_set) {
    PyErr_Print();
  }
}


void CPyLoopFunction::Destroy() {
  
  // Launch Python destroy function
  try {
    object destroy_f = m_loop_main.attr("destroy");
    destroy_f();
  } catch (error_already_set) {
    PyErr_Print();
  }
}

void CPyLoopFunction::PreStep() {

  // Launch Python pre_step function
  try {
    object pre_step_f = m_loop_main.attr("pre_step");
    pre_step_f();
  } catch (error_already_set) {
    PyErr_Print();
  }
}

void CPyLoopFunction::PostStep() {
  // Launch Python post_step function
  
  if (GetSpace().GetSimulationClock()==99){

   float pos_tick=0.0;
 CEPuckEntity* pcEPuck2 = nullptr;
        
            // Iterate over all e-puck entities
   
 CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
  for(CSpace::TMapPerType::iterator it = m_cEpuck.begin(); it != m_cEpuck.end(); ++it)
  {

    CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);
    if (cEpuck.GetId() == "bc1" or cEpuck.GetId() == "bc0" or cEpuck.GetId() == "bc2" or cEpuck.GetId() == "bc3" or cEpuck.GetId() == "bc1" or cEpuck.GetId() == "bc2" or cEpuck.GetId() == "bc5" or cEpuck.GetId() == "bc4" or cEpuck.GetId() == "bc2") {
      pos_tick+=0.1;
            // Return the e-puck entity with the specified ID
            pcEPuck2= &cEpuck;
            

            //break;
        }
 
                         // If the foot-bot with ID 2 exists, move it to the new position
        if (pcEPuck2) {
            // Define the new position
            CVector3 newPosition(0.5, -0.0-pos_tick, 0.0); // Adjust the values as needed

            // Move the foot-bot to the new position
            MoveEntity(pcEPuck2->GetEmbodiedEntity(), newPosition, CQuaternion());
                    CEPuckEntity* pcEPuck2 = nullptr;
        }
  


  } 


        
  }

  
  
  try {
    object post_step_f = m_loop_main.attr("post_step");
    post_step_f();
  } catch (error_already_set) {
    PyErr_Print();
  }
}

bool CPyLoopFunction::IsExperimentFinished() {

// Launch Python is_experiment_finished function
  try {
    object is_experiment_finished_f = m_loop_main.attr("is_experiment_finished");
    return is_experiment_finished_f();
  } catch (error_already_set) {
    PyErr_Print();
    return true;
  }

}


void CPyLoopFunction::AddEPuckEntityFromPython(const boost::python::tuple& position, const boost::python::tuple& orientation) {
    // Extract position and orientation values from the Python tuples
    double posX = boost::python::extract<double>(position[0]);
    double posY = boost::python::extract<double>(position[1]);
    double posZ = boost::python::extract<double>(position[2]);

    double quatX = boost::python::extract<double>(orientation[0]);
    double quatY = boost::python::extract<double>(orientation[1]);
    double quatZ = boost::python::extract<double>(orientation[2]);
    double quatW = boost::python::extract<double>(orientation[3]);

    // Create a CVector3 and CQuaternion objects from the extracted values
    CVector3 pos(posX, posY, posZ);
    CQuaternion quat(quatX, quatY, quatZ, quatW);

    // Call the AddEPuckEntity method with the extracted position and orientation
    //AddEPuckEntity(pos, quat);
}



CColor CPyLoopFunction::GetFloorColor() {

// Launch Python is_experiment_finished function
  try {
    object get_floor_color_f = m_loop_main.attr("get_floor_color");

    std::cout << "Testing GetFloorColor" << std::endl;
    return CColor::WHITE;
  } catch (error_already_set) {
    PyErr_Print();
    return CColor::WHITE;
  }

}

void CPyLoopFunction::PostExperiment() {
  // Launch Python post_experiment function
  try {
    object post_experiment_f = m_loop_main.attr("post_experiment");
    post_experiment_f();
  } catch (error_already_set) {
    PyErr_Print();
  }
}


REGISTER_LOOP_FUNCTIONS(CPyLoopFunction, "py_loop_function")
