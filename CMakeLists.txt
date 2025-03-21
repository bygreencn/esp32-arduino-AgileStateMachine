set(srcs 
	"src/AgileStateMachine.cpp"
	"src/State.cpp"
)

set(include_dirs 
		"include"
)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}")
